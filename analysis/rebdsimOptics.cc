/**
 * @file rebdsimOptics.cc
 */

#include <iostream>
#include <string>

#include "DataLoader.hh"
#include "EventAnalysis.hh"

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
    {usage(); exit(1);}

  std::string inputFileName  = std::string(argv[1]);
  std::string outputFileName = std::string(argv[2]);

  DataLoader dl = DataLoader(inputFileName, false, true);
  EventAnalysis* evtAnalysis = new EventAnalysis(dl.GetEvent(), dl.GetEventTree(), true);
  evtAnalysis->Execute();

  TFile* outputFile = new TFile(outputFileName.c_str(), "RECREATE");

  auto modelTree = dl.GetModelTree();
  modelTree->CloneTree(-1, "fast");
  outputFile->Write();

  evtAnalysis->Write(outputFile);
  outputFile->Close();

  return 0;
}
