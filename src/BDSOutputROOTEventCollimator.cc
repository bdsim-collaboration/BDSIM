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
#include "BDSOutputROOTEventCollimator.hh"
#include "BDSOutputROOTGeant4Data.hh"

#ifndef __ROOTBUILD__
#include "BDSCollimatorHit.hh"
#include "BDSEnergyCounterHit.hh"

#include "globals.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#endif

#include <set>
#include <vector>

class BDSOutputROOTGeant4Data;

ClassImp(BDSOutputROOTEventCollimator)

BDSOutputROOTGeant4Data* BDSOutputROOTEventCollimator::particleTable = nullptr;

BDSOutputROOTEventCollimator::BDSOutputROOTEventCollimator():
  primaryInteracted(false),
  primaryStopped(false),
  n(0),
  totalEnergyDeposited(0)
{;}

BDSOutputROOTEventCollimator::~BDSOutputROOTEventCollimator()
{;}

void BDSOutputROOTEventCollimator::Flush()
{
  primaryInteracted = false;
  primaryStopped    = false;
  n                 = 0;
  totalEnergy.clear();
  energyDeposited.clear();
  xIn.clear();
  yIn.clear();
  zIn.clear();
  xpIn.clear();
  ypIn.clear();
  zpIn.clear();
  T.clear();
  weight.clear();
  partID.clear();
  parentID.clear();
  turn.clear();
  impactParameterX.clear();
  impactParameterY.clear();
  isIon.clear();
  ionZ.clear();
  ionA.clear();
  turnSet.clear();
  charge.clear();
  kineticEnergy.clear();
  mass.clear();
  rigidity.clear();
  totalEnergyDeposited = 0;
}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventCollimator::Fill(const BDSCollimatorHit* hit)
{
  n++;
  totalEnergy.push_back(hit->totalEnergy / CLHEP::GeV);
  const G4ThreeVector& pos = hit->preStepPosition;
  const G4ThreeVector& mom = hit->preStepMomentum;
  xIn.push_back(pos.x() / CLHEP::m);
  yIn.push_back(pos.y() / CLHEP::m);
  zIn.push_back(pos.z() / CLHEP::m);
  xpIn.push_back(mom.x() / CLHEP::rad);
  ypIn.push_back(mom.y() / CLHEP::rad);
  zpIn.push_back(mom.z() / CLHEP::rad);

  BDSEnergyCounterHit* eHit = hit->energyDepositionHit;
  if (eHit)
    {
      G4double eDep = eHit->GetEnergy() / CLHEP::GeV;
      G4double w    = eHit->GetWeight();
      totalEnergyDeposited += eDep * w;
      
      primaryInteracted = primaryInteracted || eHit->GetParentID() == 0;

      energyDeposited.push_back(eDep);
      T.push_back(eHit->GetGlobalTime() / CLHEP::ns);
      weight.push_back(w);
      partID.push_back(eHit->GetPartID());
      parentID.push_back(eHit->GetParentID());
      G4int tn = eHit->GetTurnsTaken();
      turn.push_back(tn);
      turnSet.insert(tn);
    }
}

void BDSOutputROOTEventCollimator::FillExtras(G4bool fillIonInfo,
					      G4bool fillAllExtras)
{
  if (!particleTable)
    {return;}

  if (!fillIonInfo || !fillAllExtras)
    {return;}
  
  for (int i = 0; i < n; ++i)
    {// loop over all existing entires in the branch vectors
      auto& pid = partID[i];
      if (particleTable->IsIon(pid))
        {
          auto& ionInfo = particleTable->GetIonInfo(pid);
	  if (fillIonInfo)
	    {
	      isIon.push_back(true);
	      ionA.push_back(ionInfo.a);
	      ionZ.push_back(ionInfo.z);
	    }
	  if (fillAllExtras)
	    {
	      charge.push_back(ionInfo.charge);
	      mass.push_back(ionInfo.mass);
	      rigidity.push_back(ionInfo.rigidity(totalEnergy[i], ionInfo.charge));
	      kineticEnergy.push_back(particleTable->KineticEnergy(pid, totalEnergy[i]));
	    }
        }
      else
        {// particle
	  auto& pInfo = particleTable->GetParticleInfo(pid);
	  if (fillIonInfo)
	    {
	      isIon.push_back(false);
	      ionA.push_back(0);
	      ionZ.push_back(0);
	    }
	  if (fillAllExtras)
	    {
	      charge.push_back(pInfo.charge);
	      mass.push_back(pInfo.mass);
	      rigidity.push_back(pInfo.rigidity(totalEnergy[i], pInfo.charge));
	      kineticEnergy.push_back(particleTable->KineticEnergy(pid, totalEnergy[i]));
	    }
        }
    }
}

#endif
