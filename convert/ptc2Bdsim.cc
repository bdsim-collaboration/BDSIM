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
 * @file ptc2Bdsim.cc
 */

#include "Particles.hh"
#include "TfsFile.hh"

#include "BDSOutputROOTEventHeader.hh"
#include "BDSOutputROOTEventSampler.hh"

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"

int main(int argc, char *argv[])
{
  // check input
  if (argc != 4 )
    {
      std::cout << "usage: ptc2BDSIM ptcOutput outputFile particleName" << std::endl;
      std::cout << " <ptcOutput>    - output file of PTC (Tfs format)" << std::endl;
      std::cout << " <outputFile>   - desired output file name for BDSIM format file" << std::endl;
      std::cout << " <particleName> - one of e- e+ proton" << std::endl;
      exit(1);
    }
  
  std::string inputFileName  = std::string(argv[1]);
  std::string outputFileName = std::string(argv[2]);
  std::string particleName   = std::string(argv[3]);

  double mass  = 0;
  int    pdgID = 0;
  try
    {
      auto pInfo = BDSC::GetParticleInfo(particleName);
      mass  = pInfo.mass;
      pdgID = pInfo.pdgID;
    }
  catch (std::string& error)
    {
      std::cout << error << std::endl;
      exit(1);
    }
  
  // load ptc file
  PTC::TfsFile* input = new PTC::TfsFile(inputFileName);
  input->Load();    

  TFile* outputFile = new TFile(outputFileName.c_str(),"RECREATE");
  outputFile->cd();
  TTree* eventOutputTree = new TTree("Event","BDSIM event");

  // shortcut for handiness
  typedef BDSOutputROOTEventSampler<double> samplerd;

  // setup local objects, branches and link to output file
  std::vector<samplerd*> localSamplers;
  for (const auto& sampler : *input)
    {
      samplerd* outputSampler = new samplerd();
      std::string sampName = sampler.name;
      outputSampler->samplerName = sampName;
      eventOutputTree->Branch((sampName+".").c_str(),
			      "BDSOutputROOTEventSampler",
			      outputSampler,32000,0);
      localSamplers.push_back(outputSampler);
    }
  
  // we can only loose particles so the number of entries in the first
  // is the number of 'events' or entries we'll use
  int nEntries  = (int)input->segments[0].size();
  int nSamplers = (int)input->size(); // we know our localSamplers has the same size
  for (int entry = 0; entry < nEntries; ++entry)
    {
      for (int samplerIndex = 0; samplerIndex < nSamplers; ++samplerIndex)
	{
	  const auto& inputSampler = input->segments[samplerIndex];
	  if (entry > (int)inputSampler.size())
	    {continue;} // no hit on this sampler
	  const auto& data = inputSampler.observations[entry];

	  auto lSampler = localSamplers[samplerIndex];
	  lSampler->n = 1;
	  
	  lSampler->energy.push_back(0);
	  
	  lSampler->x.push_back(data.x);
	  lSampler->y.push_back(data.y);
	  lSampler->z = 0;                   // local z always 0

	  lSampler->xp.push_back(0);
	  lSampler->yp.push_back(0);
	  lSampler->zp.push_back(0);
	  lSampler->t.push_back(0);
	  
	  lSampler->weight.push_back(1);
	  lSampler->partID.push_back(pdgID);
	  lSampler->parentID.push_back(0);
	  lSampler->trackID.push_back(0);
	  lSampler->modelID = 0;
	  lSampler->turnNumber.push_back(0);
	  lSampler->S = data.s;
	}
      outputFile->cd();
      eventOutputTree->Fill();
    }

  // add header for file type and version details
  outputFile->cd();
  BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
  headerOut->Fill(); // updates time stamp
  headerOut->SetFileType("REBDSIM");
  TTree* headerTree = new TTree("Header", "REBDSIM Header");
  headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
  headerTree->Fill();
  outputFile->Write(0,TObject::kOverwrite);

  outputFile->Close();

  delete outputFile;
  delete eventOutputTree;
  for (auto& s : localSamplers)
    {delete s;}
  delete input;

  return 0;
}
