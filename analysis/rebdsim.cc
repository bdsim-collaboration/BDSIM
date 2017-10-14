/**
 * @file rebdsim.cc
 */

#include <iostream>

#include "TChain.h"
#include "TFile.h"

#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventModel.hh"

#include "Analysis.hh"
#include "Config.hh"
#include "DataLoader.hh"
#include "BeamAnalysis.hh"
#include "EventAnalysis.hh"
#include "ModelAnalysis.hh"
#include "OptionsAnalysis.hh"
#include "RebdsimTypes.hh"
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

  std::string configFilePath = std::string(argv[1]); //create a string from arguments so able to use find_last_of and substr  methods
  std::string configFileExtension =  configFilePath.substr(configFilePath.find_last_of(".") + 1) ;
  if(configFileExtension != "txt")
  {
    std::cout << "Unrecognised filetype: '." << configFileExtension << "'" <<  std::endl;
    std::cout << "Make sure the config file is plain text with the .txt extension!" << std::endl;
    exit(1);
  }

  std::cout << "rebdsim> configuration file name : " << configFilePath << std::endl;

  std::string inputFilePath = "";  // default of "" means use ones specified in analysisConfig.txt
  std::string outputFileName = "";

  if (argc > 2)
    {inputFilePath = std::string(argv[2]);}
  if (argc > 3)
    {outputFileName = std::string(argv[3]);}

  try
    {Config::Instance(configFilePath, inputFilePath, outputFileName);}
  catch (std::string error)
    {
      std::cout << error << std::endl;
      exit(1);
    }

  bool allBranches = Config::Instance()->AllBranchesToBeActivated();
  const RBDS::BranchMap* branchesToActivate = &(Config::Instance()->BranchesToBeActivated());

  std::vector<Analysis*> analyses;

  bool debug = Config::Instance()->Debug();
  DataLoader dl = DataLoader(Config::Instance()->InputFilePath(),
			     debug,
			     Config::Instance()->ProcessSamplers(),
			     allBranches,
			     branchesToActivate);

  BeamAnalysis*    beaAnalysis = new BeamAnalysis(dl.GetBeam(), dl.GetBeamTree(), debug);
  EventAnalysis*   evtAnalysis = new EventAnalysis(dl.GetEvent(),
                                                   dl.GetEventTree(),
                                                   Config::Instance()->ProcessSamplers(),
                                                   debug,
                                                   Config::Instance()->PrintModuloFraction(),
  Config::Instance()->GetOptionBool("emittanceonthefly"));
  RunAnalysis*     runAnalysis = new RunAnalysis(dl.GetRun(), dl.GetRunTree(), debug);
  OptionsAnalysis* optAnalysis = new OptionsAnalysis(dl.GetOptions(), dl.GetOptionsTree(), debug);
  ModelAnalysis*   modAnalysis = new ModelAnalysis(dl.GetModel(), dl.GetModelTree(), debug);

  analyses.push_back(beaAnalysis);
  analyses.push_back(evtAnalysis);
  analyses.push_back(runAnalysis);
  analyses.push_back(optAnalysis);
  analyses.push_back(modAnalysis);

  for (auto& analysis : analyses)
    {analysis->Execute();}

  // write output
  try
    {
      TFile* outputFile = new TFile(Config::Instance()->OutputFileName().c_str(),"RECREATE");
      for (auto& analysis : analyses)
	{analysis->Write(outputFile);}

      outputFile->Close();
    }
  catch (std::string error)
    {
      std::cout << error << std::endl;
      exit(1);
    }
  return 0;
}
