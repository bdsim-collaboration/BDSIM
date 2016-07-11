/**
 * @file rebdsim.cc
 */

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
  if(argc < 2 || argc > 4)
  {
    std::cout << "usage: rebdsim <analysisConfig> (<dataFile>) (<outputFile>)" << std::endl;
    std::cout << " <datafile> (optional) - root file to operate on" << std::endl;
    std::cout << " <outputfile> (optional) - output file name for analysis" << std::endl;
    std::cout << " if no <datafile> and <outputfile> are specified, those from <analysisConfig> are used." << std::endl;
    exit(1);
  }

  std::string configFilePath = argv[1]; //create a string from arguments so able to use find_last_of and substr  methods
  std::string configFileExtension =  configFilePath.substr(configFilePath.find_last_of(".") + 1) ;
  if(configFileExtension != "txt")
  {
    std::cout << "Unrecognised filetype: '." << configFileExtension << "'" <<  std::endl;
    std::cout << "Make sure the config file is plain text with the .txt extension!" << std::endl;
    exit(1);
  }

  std::cout << "rebdsim> ConfigFileName : " << argv[1] << std::endl;

  std::string fileName = "";
  std::string inputFilePath = "";
  std::string outputFileName = "";

  if (argc > 2)
    {inputFilePath = std::string(argv[2]);}
  if (argc > 3)
    {outputFileName = std::string(argv[3]);}

  try {
    Config::Instance(argv[1], argv[2], argv[3]);
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
  runAnalysis.Process();

  // write output
  TFile *outputFile = new TFile(Config::Instance()->OutputFileName().c_str(),"RECREATE");
  evtAnalysis.Write(outputFile);
  runAnalysis.Write(outputFile);
  outputFile->Close();

  return 0;
}
