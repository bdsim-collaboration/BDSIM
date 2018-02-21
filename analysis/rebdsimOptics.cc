/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
 * @file rebdsimOptics.cc
 */

#include <iostream>
#include <string>

#include "DataLoader.hh"
#include "EventAnalysis.hh"

#include "BDSOutputROOTEventHeader.hh"

#include "TFile.h"
#include "TChain.h"

void usage()
{ 
  std::cout << "usage: rebdsimOptics <dataFile> <outputfile>"         << std::endl;
  std::cout << " <datafile>   - root file to operate on ie run1.root" << std::endl;
  std::cout << " <outputfile> - name of output file ie optics.dat"    << std::endl;
  std::cout << "This only works on a single file - no wildcards."     << std::endl;
}

int main(int argc, char* argv[])
{
  if (argc != 3)
    {
      std::cout << "Incorrect number of arguments." << std::endl;
      usage();
      exit(1);
    }

  std::string inputFileName  = std::string(argv[1]);
  std::string outputFileName = std::string(argv[2]);

  DataLoader dl = DataLoader(inputFileName, false, true);
  EventAnalysis* evtAnalysis = new EventAnalysis(dl.GetEvent(), dl.GetEventTree(), false, true);
  evtAnalysis->Execute();

  TFile* outputFile = new TFile(outputFileName.c_str(), "RECREATE");

  // add header for file type and version details
  outputFile->cd();
  BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
  headerOut->Fill(); // updates time stamp
  headerOut->SetFileType("REBDSIM");
  TTree* headerTree = new TTree("Header", "REBDSIM Header");
  headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
  headerTree->Fill();

  auto modelTree = dl.GetModelTree();
  modelTree->CloneTree(-1, "fast");
  outputFile->Write();

  evtAnalysis->Write(outputFile);
  outputFile->Close();

  return 0;
}
