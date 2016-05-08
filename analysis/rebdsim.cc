#include <iostream>

#include "TChain.h"
#include "TFile.h"

#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventModel.hh"

#include "Config.hh"
#include "DataLoader.hh"
#include "EventAnalysis.hh"
#include "RunAnalysis.hh"


int main(int argc, char *argv[])
{
  // check input
  if(argc != 2)
  {
    std::cout << "usage rebdsim <ConfigFileName>" << std::endl;
    exit(1);
  }

  std::cout << "rebdsim> ConfigFileName : " << argv[1] << std::endl;

  try {
    Config::Instance(argv[1]);
  }
  catch(std::string e) {
    std::cout << e << std::endl;
    exit(1);
  }

  DataLoader dl = DataLoader();
  EventAnalysis evtAnalysis = EventAnalysis(dl.GetEvent(), dl.GetEventTree());

  // process events
  evtAnalysis.Initialise();
  evtAnalysis.Process();
  evtAnalysis.SimpleHistograms();
  evtAnalysis.Terminate();

  RunAnalysis runAnalysis = RunAnalysis(dl.GetRun(), dl.GetRunTree());

  // write output
  TFile *outputFile = new TFile(Config::Instance()->OutputFileName().c_str(),"RECREATE");
  evtAnalysis.Write(outputFile);
  runAnalysis.Write(outputFile);
  outputFile->Close();

}
