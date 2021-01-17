/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
 * @file bdsimCombine.cc
 */
#include "BDSOutputROOTEventHeader.hh"

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include <exception>
#include <glob.h>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
  if (argc < 3)
    {
      std::cout << "usage: bdsimCombine result.root file1.root file2.root ..." << std::endl;
      exit(1);
    }

  // build input file list
  std::vector<std::string> inputFiles;
  for (int i = 2; i < argc; ++i)
    {inputFiles.push_back(std::string(argv[i]));}
  if (inputFiles[0].find("*") != std::string::npos)
    {
      std::vector<std::string> fileNamesTemp;
      glob_t glob_result;
      glob(inputFiles[0].c_str(),GLOB_TILDE,nullptr,&glob_result);
      for (unsigned int i=0; i<glob_result.gl_pathc; ++i)
	{fileNamesTemp.push_back(glob_result.gl_pathv[i]);}
      globfree(&glob_result);
      inputFiles = fileNamesTemp;
    }

  // checks
  if (inputFiles.size() == 1)
    {
      std::cout << "Only one input file provided \"" << inputFiles[0] << "\" - no point." << std::endl;
      exit(1);
    }

  std::string outputFile = std::string(argv[1]);
  if (outputFile.find("*") != std::string::npos)
    {
      std::cerr << "First argument for output file \"" << outputFile << "\" contains an *." << std::endl;
      std::cerr << "Should only be a singular file - check order of arguments." << std::endl;
      exit(1);
    }

  TChain* eventsMerged = new TChain("Event");
  for (const auto& filename : inputFiles)
    {eventsMerged->Add(filename.c_str());}
  eventsMerged->Merge(outputFile.c_str());

  /*
  // output file must be opened before histograms are created because root does
  // everything statically behind the scenes
  TFile* output = new TFile(outputFile.c_str(), "RECREATE");
  
  // add header for file type and version details
  output->cd();
  BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
  headerOut->Fill(std::vector<std::string>(), inputFiles); // updates time stamp
  headerOut->SetFileType("BDSIM");
  TTree* headerTree = new TTree("Header", "BDSIM Header");
  headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
  headerTree->Fill();
  output->Write(nullptr,TObject::kOverwrite);

  TFile* f = nullptr; // temporary variable

  output->Write(nullptr,TObject::kOverwrite);
  output->Close();
  delete output;
  */
  std::cout << "Combined result of " << inputFiles.size() << " files written to: " << outputFile << std::endl;
  
  return 0;
}
