/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
 * @file rebdsim.cc
 */

#include <iostream>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include "BDSOutputROOTEventHeader.hh"
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventModel.hh"

#include "Analysis.hh"
#include "BeamAnalysis.hh"
#include "Config.hh"
#include "DataLoader.hh"
#include "EventAnalysis.hh"
#include "ModelAnalysis.hh"
#include "OptionsAnalysis.hh"
#include "RebdsimTypes.hh"
#include "RunAnalysis.hh"

int main(int argc, char *argv[])
{
  // check input
  if (argc < 2 || argc > 4)
    {
      std::cout << "usage: rebdsim <analysisConfig> (<dataFile>) (<outputFile>)" << std::endl;
      std::cout << " <datafile> (optional) - root file to operate on" << std::endl;
      std::cout << " <outputfile> (optional) - output file name for analysis" << std::endl;
      std::cout << " if no <datafile> and <outputfile> are specified, those from <analysisConfig> are used." << std::endl;
      exit(1);
    }

  std::string configFilePath = std::string(argv[1]); //create a string from arguments so able to use find_last_of and substr  methods
  std::string configFileExtension =  configFilePath.substr(configFilePath.find_last_of(".") + 1) ;
  if (configFileExtension != "txt")
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

  // parse input file with options and histogram definitions
  try
    {Config::Instance(configFilePath, inputFilePath, outputFileName);}
  catch (std::string error)
    {
      std::cout << error << std::endl;
      exit(1);
    }

  Config* config = Config::Instance();
  
  bool allBranches = config->AllBranchesToBeActivated();
  const RBDS::BranchMap* branchesToActivate = &(config->BranchesToBeActivated());
  
  bool debug = config->Debug();
  DataLoader dl = DataLoader(config->InputFilePath(),
			     debug,
			     config->ProcessSamplers(),
			     allBranches,
			     branchesToActivate,
			     config->GetOptionBool("backwardscompatible"));

  BeamAnalysis*    beaAnalysis = new BeamAnalysis(dl.GetBeam(),
						  dl.GetBeamTree(),
						  config->PerEntryBeam(),
						  debug);
  EventAnalysis*   evtAnalysis = new EventAnalysis(dl.GetEvent(),
                                                   dl.GetEventTree(),
						   config->PerEntryEvent(),
						   config->ProcessSamplers(),
                                                   debug,
                                                   config->PrintModuloFraction(),
						   config->GetOptionBool("emittanceonthefly"),
                                                   (long int)config->GetOptionNumber("eventstart"),
                                                   (long int)config->GetOptionNumber("eventend"));
  RunAnalysis*     runAnalysis = new RunAnalysis(dl.GetRun(),
						 dl.GetRunTree(),
						 config->PerEntryRun(),
						 debug);
  OptionsAnalysis* optAnalysis = new OptionsAnalysis(dl.GetOptions(),
						     dl.GetOptionsTree(),
						     config->PerEntryOption(),
						     debug);
  ModelAnalysis*   modAnalysis = new ModelAnalysis(dl.GetModel(),
						   dl.GetModelTree(),
						   config->PerEntryModel(),
						   debug);

  std::vector<Analysis*> analyses = {beaAnalysis,
				     evtAnalysis,
				     runAnalysis,
				     optAnalysis,
				     modAnalysis};

  for (auto& analysis : analyses)
    {analysis->Execute();}

  // write output
  try
    {
      TFile* outputFile = new TFile(config->OutputFileName().c_str(),"RECREATE");

      // add header for file type and version details
      outputFile->cd();
      BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
      headerOut->Fill(); // updates time stamp
      headerOut->SetFileType("REBDSIM");
      TTree* headerTree = new TTree("Header", "REBDSIM Header");
      headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
      headerTree->Fill();
      outputFile->Write(0,TObject::kOverwrite);
      
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
