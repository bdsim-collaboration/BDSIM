/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
 * @file rebdsimCombineRun.cc
 */
#include "FileMapper.hh"
#include "HistogramAccumulatorMerge.hh"
#include "HistogramAccumulatorSum.hh"
#include "HistogramAccumulatorV.hh"
#include "Run.hh"

#include "BDSOutputROOTEventHeader.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSVersionData.hh"

#include "TDirectory.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
  if (argc < 3)
    {
      std::cout << "usage: rebdsimCombineRun result.root file1.root file2.root ..." << std::endl;
      exit(1);
    }

  std::string outputFile = std::string(argv[1]);
  // output file must be opened before histograms are created because root does
  // everything statically behind the scenes
  TFile* output = new TFile(outputFile.c_str(), "RECREATE");
  
  // add header for file type and version details
  output->cd();
  BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
  headerOut->Fill(); // updates time stamp
  headerOut->SetFileType("REBDSIMCOMBINE");
  TTree* headerTree = new TTree("Header", "REBDSIM Header");
  headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
  headerTree->Fill();
  output->Write(nullptr,TObject::kOverwrite);

  // setup output directory
  std::string runDir = "Run/MergedHistograms";
  output->mkdir(runDir.c_str());
  output->cd(runDir.c_str());
  
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

  // initialise file map
  TFile* f = nullptr;
  try
    {f = new TFile(inputFiles[0].c_str(), "READ");}
  catch (const std::exception& e)
    {std::cerr << e.what() << std::endl; return 1;}
  int* dataVersionOnFile = new int(0);
  if (!RBDS::IsBDSIMOutputFile(f, dataVersionOnFile))
    {std::cerr << "First file not a BDSIM output file - not proceeding" << std::endl; exit(1);}
  Run*   runFirst     = new Run();
  TTree* runFirstTree = static_cast<TTree*>(f->Get("Run"));
  runFirst->SetBranchAddress(runFirstTree);
  runFirstTree->GetEntry(0);
  output->cd(runDir.c_str());
  BDSOutputROOTEventHistograms* baseHistograms = new BDSOutputROOTEventHistograms(*runFirst->Histos);
  f->Close();
  delete f;

  typedef HistogramAccumulatorV<HistogramAccumulatorMerge> AccumulatorMerge;
  output->cd(runDir.c_str());
  AccumulatorMerge* accumulator = new AccumulatorMerge(baseHistograms);
  delete baseHistograms;
  
  // loop over files and accumulate
  for (const auto& file : inputFiles)
    {
      f = new TFile(file.c_str());
      if (RBDS::IsBDSIMOutputFile(f))
	{
	  std::cout << "Accumulating> " << file << std::endl;
	  
	  Run* runLocal = new Run();
	  TTree* runTreeFile = static_cast<TTree*>(f->Get("Model"));
	  if (!runTreeFile)
	    {
	      std::cerr << "No Run tree in file " << file << " -> skipping" << std::endl;
	      delete runLocal;
	    }
	  runLocal->SetBranchAddress(runTreeFile);
	  runTreeFile->GetEntry(0);
	  accumulator->Accumulate(runLocal->Histos);
	  delete runLocal;
	}
      else
	{std::cout << "Skipping " << file << " as not a BDSIM output file" << std::endl;}
      f->Close();
      delete f;
    }
  
  // terminate and write output
  BDSOutputROOTEventHistograms* result = accumulator->Terminate();

  delete accumulator;

  output->Write(nullptr,TObject::kOverwrite);
  output->Close();
  delete output;
  
  return 0;
}
