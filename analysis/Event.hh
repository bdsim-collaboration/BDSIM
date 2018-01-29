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
#ifndef EVENT_H
#define EVENT_H

#include <vector>

#include "TROOT.h"
#include "TChain.h"

#include "BDSOutputROOTEventSampler.hh"

#include "RebdsimTypes.hh"

class BDSOutputROOTEventHistograms;
class BDSOutputROOTEventInfo;
class BDSOutputROOTEventLoss;
class BDSOutputROOTEventTrajectory;

/**
 * @brief Event loader.
 * 
 * @author Stewart Boogert
 */

class Event
{
public:
  Event();
  Event(bool debugIn,
	bool processSamplersIn = false);
  void CommonCtor();
  virtual ~Event();

  /// @{ Accessor
#ifdef __ROOTDOUBLE__  
  BDSOutputROOTEventSampler<double>* GetPrimaries() {return Primary;}
#else
  BDSOutputROOTEventSampler<float>*  GetPrimaries() {return Primary;}
#endif
  BDSOutputROOTEventLoss*            GetLoss()             {return Eloss;}
  BDSOutputROOTEventLoss*            GetPrimaryFirstHit()  {return PrimaryFirstHit;}
  BDSOutputROOTEventLoss*            GetPrimaryLastHit()   {return PrimaryLastHit;}
  BDSOutputROOTEventLoss*            GetTunnelHit()        {return TunnelHit;}
  BDSOutputROOTEventTrajectory*      GetTrajectory()       {return Trajectory;}
  BDSOutputROOTEventHistograms*      GetHistograms()       {return Histos;}
  BDSOutputROOTEventInfo*            GetInfo()             {return Info;}
  /// @}

  /// Set the branch addresses to address the contents of the file. The vector
  /// of sampler names is used to turn only the samplers required. 
  void SetBranchAddress(TTree* t,
			const RBDS::VectorString* samplerNames     = nullptr,
			bool                      allBranchesOn    = false,
			const RBDS::VectorString* branchesToTurnOn = nullptr);

  /// @{ Local variable ROOT data is mapped to.
#ifdef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<double>* Primary;
#else
  BDSOutputROOTEventSampler<float>* Primary;
#endif
  BDSOutputROOTEventLoss*       Eloss;
  BDSOutputROOTEventLoss*       PrimaryFirstHit;
  BDSOutputROOTEventLoss*       PrimaryLastHit;
  BDSOutputROOTEventLoss*       TunnelHit;
  BDSOutputROOTEventTrajectory* Trajectory;
#ifdef __ROOTDOUBLE__
  std::vector<BDSOutputROOTEventSampler<double>*> Samplers;
#else
  std::vector<BDSOutputROOTEventSampler<float>*>  Samplers;
#endif
  BDSOutputROOTEventHistograms* Histos;
  BDSOutputROOTEventInfo*       Info;
  /// @}

private:
  ClassDef(Event,1);

  bool debug           = false;
  bool processSamplers = false;
};

#endif
