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
#include "Event.hh"
#include "RebdsimTypes.hh"

#include "BDSOutputROOTEventCollimator.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTEventSampler.hh"

#include <vector>

#include "TChain.h"

ClassImp(Event)

Event::Event():
  dataVersion(0),
  debug(false),
  processSamplers(false),
  usePrimaries(false)
{
  CommonCtor();
}

Event::Event(int  dataVersionIn,
	     bool debugIn,
	     bool processSamplersIn):
  dataVersion(dataVersionIn),
  debug(debugIn),
  processSamplers(processSamplersIn)
{
  CommonCtor();
}

Event::~Event()
{
  delete Primary;
  delete Eloss;
  delete PrimaryFirstHit;
  delete PrimaryLastHit;
  delete TunnelHit;
  delete Trajectory;
  delete Histos;
  delete Info;
  for (auto s : Samplers)
    {delete s;}
  for (auto c : collimators)
    {delete c;}
}

void Event::CommonCtor()
{
#ifdef __ROOTDOUBLE__
  Primary         = new BDSOutputROOTEventSampler<double>();
#else
  Primary         = new BDSOutputROOTEventSampler<float>();
#endif
  Eloss           = new BDSOutputROOTEventLoss();
  PrimaryFirstHit = new BDSOutputROOTEventLoss();
  PrimaryLastHit  = new BDSOutputROOTEventLoss();
  TunnelHit       = new BDSOutputROOTEventLoss();
  Trajectory      = new BDSOutputROOTEventTrajectory();
  Histos          = new BDSOutputROOTEventHistograms();
  Info            = new BDSOutputROOTEventInfo();
}

#ifdef __ROOTDOUBLE__
BDSOutputROOTEventSampler<double>* Event::GetSampler(const std::string& name)
#else
BDSOutputROOTEventSampler<float>* Event::GetSampler(const std::string& name)
#endif
{
  auto found = samplerMap.find(name);
  if (found != samplerMap.end())
    {return found->second;}
  else
    {return nullptr;}
}

#ifdef __ROOTDOUBLE__
BDSOutputROOTEventSampler<double>* Event::GetSampler(int index)
#else
BDSOutputROOTEventSampler<float>* Event::GetSampler(int index)
#endif
{
  if (index >= (int) Samplers.size())
    {return nullptr;}
  else
    {return Samplers[index];}
}

BDSOutputROOTEventCollimator* Event::GetCollimator(const std::string& name)
{
  auto found = collimatorMap.find(name);
  if (found != collimatorMap.end())
    {return found->second;}
  else
    {
      auto found2 = collimatorMap.find("COLL_" + name);
      if (found2 != collimatorMap.end())
	{return found2->second;}
      else
	{return nullptr;}
    }
}

BDSOutputROOTEventCollimator* Event::GetCollimator(int index)
{
  if (index >= (int) collimators.size())
    {return nullptr;}
  else
    {return collimators[index];}
}

void Event::SetBranchAddress(TTree* t,
			     const RBDS::VectorString* samplerNamesIn,
			     bool                      allBranchesOn,
			     const RBDS::VectorString* branchesToTurnOn,
			     const RBDS::VectorString* collimatorNamesIn)
{
  if(debug)
    {std::cout << "Event::SetBranchAddress" << std::endl;}

  // turn off all branches except standard output branches.
  t->SetBranchStatus("*", 0);

  // turn on only what we need to speed up analysis as with more things
  // on, more data is loaded from the file for each GetEntry().
  // these objects are small - always load
  // the primary is optionally stored (e.g. not stored for tracking comparison files)
  if (((*t).GetListOfBranches()->FindObject("Primary.")) != nullptr)
    {
      usePrimaries = true;
      t->SetBranchStatus("Primary*",  1);
      t->SetBranchAddress("Primary.", &Primary);
    }

  t->SetBranchStatus("Info*", 1);
  t->SetBranchAddress("Info.", &Info);
  
  t->SetBranchStatus("PrimaryFirstHit*", 1);
  t->SetBranchAddress("PrimaryFirstHit.", &PrimaryFirstHit);

  t->SetBranchStatus("PrimaryLastHit*", 1);
  t->SetBranchAddress("PrimaryLastHit.", &PrimaryLastHit);
  
  if (allBranchesOn)
    {
      t->SetBranchStatus("*", 1);
      t->SetBranchAddress("Eloss.", &Eloss);
      t->SetBranchAddress("Histos.", &Histos);
      t->SetBranchAddress("TunnelHit.", &TunnelHit);
      t->SetBranchAddress("Trajectory.", &Trajectory);

      if (dataVersion > 3)
	{SetBranchAddressCollimators(t, collimatorNamesIn);}
    }
  else if (branchesToTurnOn)
    {
      for (const auto& name : *branchesToTurnOn)
	{
	  std::string nameStar = name + "*";
	  if (debug)
	    {std::cout << "Event::SetBranchAddress> Turning on branch \"" << nameStar << "\"" << std::endl;}
	  t->SetBranchStatus(nameStar.c_str(), 1);

	  // we can't automatically do this as SetBranchAddress must use the pointer
	  // of the object type and not the base class (say TObject) so there's no
	  // way to easily map these -> ifs
	  if (name == "Eloss")
	    {t->SetBranchAddress("Eloss.", &Eloss);}
	  else if (name == "Histos")
	    {t->SetBranchAddress("Histos.", &Histos);}
	  else if (name == "TunnelHit")
	    {t->SetBranchAddress("TunnelHit.", &TunnelHit);}
	  else if (name == "Trajectory")
	    {t->SetBranchAddress("Trajectory.", &Trajectory);}
	  else if (name.substr(0,4) == "COLL")
	    {SetBranchAddressCollimatorSingle(t, name);}
	}
    }

  if (debug)
    {
      std::cout << "Event::SetBranchAddress> Primary.         " << Primary       << std::endl;
      std::cout << "Event::SetBranchAddress> Eloss.           " << Eloss           << std::endl;
      std::cout << "Event::SetBranchAddress> PrimaryFirstHit. " << PrimaryFirstHit << std::endl;
      std::cout << "Event::SetBranchAddress> PrimaryLastHit.  " << PrimaryLastHit  << std::endl;
      std::cout << "Event::SetBranchAddress> TunnelHit.       " << TunnelHit       << std::endl;
      std::cout << "Event::SetBranchAddress> Trajectory.      " << Trajectory      << std::endl;
      std::cout << "Event::SetBranchAddress> Histos.          " << Histos          << std::endl;
      std::cout << "Event::SetBranchAddress> Info.            " << Info            << std::endl;
    }

  if (processSamplers && samplerNamesIn)
    {
      unsigned int nrSamplers = samplerNamesIn->size();
      Samplers.resize(nrSamplers); // reserve and nominally instantiate instances.
      for (unsigned int i=0; i < nrSamplers; ++i)
	{
	  const auto sampName = (*samplerNamesIn)[i];
#ifdef __ROOTDOUBLE__
	  Samplers[i] = new BDSOutputROOTEventSampler<double>(sampName);
#else
	  Samplers[i] = new BDSOutputROOTEventSampler<float>(sampName);
#endif
	  samplerNames.push_back(sampName);  // cache the name in a vector
	  samplerMap[sampName] = Samplers[i];// cache the sampler in a map
	    
	  t->SetBranchAddress(sampName.c_str(), &Samplers[i]);
	  t->SetBranchStatus((sampName+"*").c_str(), 1);
	  if(debug)
	    {std::cout << "Event::SetBranchAddress> " << (*samplerNamesIn)[i] << " " << Samplers[i] << std::endl;}
	}
    }
}


void Event::SetBranchAddressCollimators(TTree* t,
					const RBDS::VectorString* collimatorNamesIn)
{
  if (collimatorNamesIn)
    {
      for (const auto& name : *collimatorNamesIn)
	{
	  collimators.resize((unsigned int)collimatorNamesIn->size());
	  SetBranchAddressCollimatorSingle(t, name);
	}
    }
}

void Event::SetBranchAddressCollimatorSingle(TTree* t,
					     const std::string& name)
{
  BDSOutputROOTEventCollimator* col = new BDSOutputROOTEventCollimator();
  collimators.push_back(col);
  collimatorNames.push_back(name);
  collimatorMap[name] = col;

  t->SetBranchAddress((name + ".").c_str(), &collimators.back());
  t->SetBranchStatus((name+".*").c_str(), 1);
}
