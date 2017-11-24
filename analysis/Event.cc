/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#include "Event.hh"
#include "RebdsimTypes.hh"

#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTEventSampler.hh"

#include <vector>

#include "TChain.h"

ClassImp(Event)

Event::Event():
  debug(false),
  processSamplers(false)
{
  CommonCtor();
}

Event::Event(bool debugIn,
	     bool processSamplersIn):
  debug(debugIn),
  processSamplers(processSamplersIn)
{
  CommonCtor();
}

Event::~Event()
{
  delete primaries;
  delete eloss;
  delete primaryFirstHit;
  delete primaryLastHit;
  delete tunnelHit;
  delete trajectory;
  delete histos;
  delete info;
  for (auto s : samplers)
    {delete s;}
}

void Event::CommonCtor()
{
#ifdef __ROOTDOUBLE__
  primaries       = new BDSOutputROOTEventSampler<double>();
#else
  primaries       = new BDSOutputROOTEventSampler<float>();
#endif
  eloss           = new BDSOutputROOTEventLoss();
  primaryFirstHit = new BDSOutputROOTEventLoss();
  primaryLastHit  = new BDSOutputROOTEventLoss();
  tunnelHit       = new BDSOutputROOTEventLoss();
  trajectory      = new BDSOutputROOTEventTrajectory();
  histos          = new BDSOutputROOTEventHistograms();
  info            = new BDSOutputROOTEventInfo();
}

void Event::SetBranchAddress(TTree *t,
			     const RBDS::VectorString* samplerNames,
			     bool                      allBranchesOn,
			     const RBDS::VectorString* branchesToTurnOn)
{
  if(debug)
    {std::cout << "Event::SetBranchAddress" << std::endl;}

  // turn off all branches except standard output branches.
  t->SetBranchStatus("*", 0);
  if (((*t).GetListOfBranches()->FindObject("Primary.")) != nullptr)
    {t->SetBranchStatus("Primary*",       1);}
  t->SetBranchStatus("Eloss*",          1);
  t->SetBranchStatus("Histos*",         1);
  t->SetBranchStatus("PrimaryFirstHit*",1);
  t->SetBranchStatus("PrimaryLastHit*", 1);
  t->SetBranchStatus("TunnelHit*",      1);
  t->SetBranchStatus("Trajectory*",     1);
  t->SetBranchStatus("Info*",           1);

  if (allBranchesOn)
    {t->SetBranchStatus("*", 1);}
  else if (branchesToTurnOn)
    {
      for (auto name : *branchesToTurnOn)
	{
	  std::string nameStar = name + "*";
	  if (debug)
	    {std::cout << "Event::SetBranchAddress> Turning on branch \"" << nameStar << "\"" << std::endl;}
	  t->SetBranchStatus(nameStar.c_str(), 1);
	}
    }
  
  // only set address of primary branch if it exists
  if (((*t).GetListOfBranches()->FindObject("Primary.")) != nullptr)
    {t->SetBranchAddress("Primary.",     &primaries);}
  t->SetBranchAddress("Eloss.",          &eloss);
  t->SetBranchAddress("Histos.",         &histos);
  t->SetBranchAddress("PrimaryFirstHit.",&primaryFirstHit);
  t->SetBranchAddress("PrimaryLastHit.", &primaryLastHit);
  t->SetBranchAddress("TunnelHit.",      &tunnelHit);
  t->SetBranchAddress("Trajectory.",     &trajectory);
  t->SetBranchAddress("Info.",           &info);

  if(debug)
    {
      std::cout << "Event::SetBranchAddress> Primary.         " << primaries       << std::endl;
      std::cout << "Event::SetBranchAddress> Eloss.           " << eloss           << std::endl;
      std::cout << "Event::SetBranchAddress> PrimaryFirstHit. " << primaryFirstHit << std::endl;
      std::cout << "Event::SetBranchAddress> PrimaryLastHit.  " << primaryLastHit  << std::endl;
      std::cout << "Event::SetBranchAddress> TunnelHit.       " << tunnelHit       << std::endl;
      std::cout << "Event::SetBranchAddress> Trajectory.      " << trajectory      << std::endl;
      std::cout << "Event::SetBranchAddress> Histos.          " << histos          << std::endl;
      std::cout << "Event::SetBranchAddress> Info.            " << info            << std::endl;
    }

  if (processSamplers && samplerNames)
    {
      unsigned int nrSamplers = samplerNames->size();
      samplers.resize(nrSamplers); // reserve and nominally instantiate instances.
      for (unsigned int i=0; i < nrSamplers; ++i)
	{
	  const auto sampName = (*samplerNames)[i];
#ifdef __ROOTDOUBLE__
	  samplers[i] = new BDSOutputROOTEventSampler<double>(sampName);
#else
	  samplers[i] = new BDSOutputROOTEventSampler<float>(sampName);
#endif
	  t->SetBranchAddress(sampName.c_str(), &samplers[i]);
      t->SetBranchStatus((sampName+"*").c_str(), 1);
	  if(debug)
	    {std::cout << "Event::SetBranchAddress> " << (*samplerNames)[i] << " " << samplers[i] << std::endl;}
	}
    }
}
