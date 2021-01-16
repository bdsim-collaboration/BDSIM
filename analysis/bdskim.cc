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
 * @file bdskim.cc
 */
#include "SelectionLoader.hh"

#include "TFile.h"
#include "TTree.h"

#include <exception>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
  if (argc != 4)
    {
      std::cout << "usage: bdskim skimfile.txt input_bdsim_raw.root output_bdsim_raw.root" << std::endl;
      exit(1);
    }

  std::string skimFile   = std::string(argv[2]);
  std::string inputFile  = std::string(argv[3]);
  std::string outputFile = std::string(argv[4]);

  // load selection
  std::string selection = RBDS::LoadSelection(skimFile);

  TFile* input  = new TFile(inputFile.c_str(),  "READ");
  
  // output file must be opened before histograms are created because root does
  // everything statically behind the scenes
  TFile* output = new TFile(outputFile.c_str(), "RECREATE");

  std::vector<std::string> treeNames = {"Header", "Options", "Model", "Run", "Beam"};
  for (const auto& tn : treeNames)
    {
      TTree* original = (TTree*)input->Get(tn.c_str());
      auto clone = original->CloneTree();
      clone->AutoSave();
    }

  TTree* allEvents = (TTree*)input->Get("Event");
  TTree* selectEvents = allEvents->CopyTree(selection.c_str());
  selectEvents->Write();
  delete output;
  
  /*
  // add header for file type and version details
  output->cd();
  BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
  headerOut->Fill(std::vector<std::string>(), inputFiles); // updates time stamp
  headerOut->SetFileType("REBDSIMCOMBINE");
  TTree* headerTree = new TTree("Header", "REBDSIM Header");
  headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
  headerTree->Fill();
  output->Write(nullptr,TObject::kOverwrite);
  */
  return 0;
}
