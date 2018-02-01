/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * @file rebdsimCombine.cc
 */
#include "FileMapper.hh"
#include "HistogramAccumulatorMerge.hh"
#include "HistogramAccumulatorSum.hh"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
  if (argc < 3)
    {
      std::cout << "usage: rebdsimCombine result.root file1.root file2.root ..." << std::endl;
      exit(1);
    }

  std::string outputFile = std::string(argv[1]);
  // output file must be opened before histograms are created because root does
  // everything statically behind the scenes
  TFile* output = new TFile(outputFile.c_str(), "RECREATE");
  
  // build input file list
  std::vector<std::string> inputFiles;
  for (int i = 2; i < argc; ++i)
    {inputFiles.push_back(std::string(argv[i]));}
  
  // checks
  if (inputFiles.size() == 1)
    {
      std::cout << "Only one input file provided \"" << inputFiles[0] << "\" - no point." << std::endl;
      exit(1);
    }

  // ensure new histograms are written to file
  TH1::AddDirectory(true);
  TH2::AddDirectory(true);
  TH3::AddDirectory(true);

  // map first file and prepare histograms
  std::vector<std::string> histMeanPaths;
  std::vector<std::string> histSumPaths;
  std::vector<HistogramAccumulatorMerge*> histMeans;
  std::vector<HistogramAccumulatorSum*>   histSums;
  unsigned int nHistMeans;
  unsigned int nHistSums;

  HistogramMap* histMap = nullptr;
  
  // loop over files and accumulate
  TFile* f = nullptr;
  for (const auto& file : inputFiles)
    {
      f = new TFile(file.c_str());
      if (RBDS::IsREBDSIMOutputFile(f))
	{
	  if (!histMap)
	    {// initialise file map
	      histMap       = new HistogramMap(f); // map out file
	      histMeanPaths = histMap->HistogramMeanPaths(); // get list of histograms
	      histSumPaths  = histMap->HistogramSumPaths();
	      nHistMeans    = (unsigned long) histMeans.size();
	      nHistSums     = (unsigned long) histSums.size();
	      for (const auto& hist : histMeanPaths)
		{// initialise accumulators
		  TH1* h = static_cast<TH1*>(f->Get(hist.c_str()));
		  int nDim = RBDS::DetermineDimensionality(h);
		  histMeans.push_back(new HistogramAccumulatorMerge(h, nDim,
								    h->GetName(),
								    h->GetTitle()));
		}
	      for (const auto& hist : histSumPaths)
		{// initialise accumulators
		  TH1* h = static_cast<TH1*>(f->Get(hist.c_str()));
		  int nDim = RBDS::DetermineDimensionality(h);
		  histSums.push_back(new HistogramAccumulatorSum(h, nDim,
								 h->GetName(),
								 h->GetTitle()));

		}
	    }
	  
	  for (unsigned int i = 0; i < nHistMeans; ++i)
	    {
	      TH1* h = static_cast<TH1*>(f->Get(histMeanPaths[i].c_str()));
	      if (!h)
		{RBDS::WarningMissingHistogram(histMeanPaths[i], file); continue;}
	      histMeans[i]->Accumulate(h);
	    }
	  for (unsigned int i = 0; i < nHistSums; ++i)
	    {
	      TH1* h = static_cast<TH1*>(f->Get(histSumPaths[i].c_str()));
	      if (!h)
		{RBDS::WarningMissingHistogram(histSumPaths[i], file); continue;}
	      histSums[i]->Accumulate(h);
	    }
	}// else skip and close file
	  
      f->Close();
      delete f;
    }

  output->Write();
  output->Close();
  delete output;

  for (auto mean : histMeans)
    {delete mean;}
  for (auto sum : histSums)
    {delete sum;}

  return 0;
}
