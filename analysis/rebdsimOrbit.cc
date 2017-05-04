/**
 * @file rebdsimOrbit.cc
 */

#include <iostream>
#include <string>

#include "DataLoader.hh"
#include "EventAnalysisOrbit.hh"

#include "TFile.h"

void usage()
{ 
  std::cout << "usage: rebdsimOrbit <dataFile> <outputfile> <index>"  << std::endl;
  std::cout << " <datafile>   - root file to operate on ie run1.root" << std::endl;
  std::cout << " <outputfile> - name of output file ie optics.root"   << std::endl;
  std::cout << " <index>      - index of orbit to pull from file (default 1)" << std::endl;
  std::cout << "This only works on a single file - no wildcards."     << std::endl;
}

int main(int argc, char* argv[])
{
  if (argc < 2 || argc > 5)
    {usage(); exit(1);}

  std::string inputFileName  = std::string(argv[1]);
  std::string outputFileName = std::string(argv[2]);
  int index = 1;
  if (argc == 4)
    {index = std::stoi(argv[3]);}

  DataLoader dl = DataLoader(inputFileName, false, true, true);
  EventAnalysisOrbit* evtAnalysis = new EventAnalysisOrbit(dl.GetEvent(),
							   dl.GetEventTree(),
							   true, true);
  evtAnalysis->ExtractOrbit(index);

  TFile* outputFile = new TFile(outputFileName.c_str(), "RECREATE");
  evtAnalysis->WriteOrbit(outputFile);
  outputFile->Close();

  return 0;
}
