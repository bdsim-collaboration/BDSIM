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
#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTGeant4Data.hh"

#include "TTree.h"

class BDSOutputROOTGeant4Data;

#ifndef __ROOTBUILD__
#include "BDSParticleCoordsFull.hh"
#include "BDSSamplerHit.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#endif

templateClassImp(BDSOutputROOTEventSampler)

template <class U>
BDSOutputROOTGeant4Data* BDSOutputROOTEventSampler<U>::particleTable = nullptr;

template <class U>
BDSOutputROOTEventSampler<U>::BDSOutputROOTEventSampler():
  samplerName("sampler")
{
  Flush();
}

template <class U>
BDSOutputROOTEventSampler<U>::BDSOutputROOTEventSampler(std::string samplerNameIn):
  samplerName(samplerNameIn)
{
  Flush();
}

template
<class U> BDSOutputROOTEventSampler<U>::~BDSOutputROOTEventSampler()
{;}

#ifndef __ROOTBUILD__
template <class U>
void BDSOutputROOTEventSampler<U>::Fill(const BDSSamplerHit* hit)
{
  // get single values
  n++;
  z = (U) (hit->GetZ() / CLHEP::m);
  S = (U) (hit->GetS() / CLHEP::m);

  energy.push_back((U &&) (hit->GetTotalEnergy() / CLHEP::GeV));
  x.push_back((U &&) (hit->GetX() / CLHEP::m));
  y.push_back((U &&) (hit->GetY() / CLHEP::m));

  xp.push_back((U &&) (hit->GetXPrime() / CLHEP::radian));
  yp.push_back((U &&) (hit->GetYPrime() / CLHEP::radian));
  zp.push_back((U &&) (hit->GetZPrime() / CLHEP::radian));

  T.push_back((U &&) (hit->GetT() / CLHEP::ns));
  modelID = hit->GetBeamlineIndex();

  weight.push_back((U &&) hit->GetWeight());
  partID.push_back(hit->GetPDGtype());
  parentID.push_back(hit->GetParentID());
  trackID.push_back(hit->GetTrackID());
  turnNumber.push_back(hit->GetTurnsTaken());
}

template <class U>
void BDSOutputROOTEventSampler<U>::Fill(const BDSParticleCoordsFull& coords,
					const G4int pdgID,
					const G4int turnsTaken,
					const G4int beamlineIndex)
{
  n++;
  energy.push_back((U &&) (coords.totalEnergy / CLHEP::GeV));  
  x.push_back((U &&)  (coords.x  / CLHEP::m));
  y.push_back((U &&)  (coords.y  / CLHEP::m));
  z = (U) (coords.z / CLHEP::m);
  xp.push_back((U &&) (coords.xp / CLHEP::radian));
  yp.push_back((U &&) (coords.yp / CLHEP::radian));
  zp.push_back((U &&) (coords.zp / CLHEP::radian));
  T.push_back((U &&) (coords.T / CLHEP::ns));
  weight.push_back((const U &) coords.weight);
  partID.push_back(pdgID);
  parentID.push_back(0);
  modelID = beamlineIndex;
  turnNumber.push_back(turnsTaken);
  S = (U) (coords.s / CLHEP::GeV);
}

//#else
//void BDSOutputROOTEventSampler::SetBranchAddress(TTree *)
//{}
#endif

template <class U> void BDSOutputROOTEventSampler<U>::SetBranchAddress(TTree *)
{;}

template <class U> void BDSOutputROOTEventSampler<U>::Flush()
{
  n = 0;
  energy.clear();
  x.clear();
  y.clear();
  z = 0.0;
  xp.clear();
  yp.clear();
  zp.clear();
  T.clear();
  weight.clear();
  partID.clear();
  parentID.clear();
  trackID.clear();
  turnNumber.clear();
  S = 0.0;
  modelID = -1;

  charge.clear();
  mass.clear();
  rigidity.clear();
  isIon.clear();
  ionA.clear();
  ionZ.clear();
}

template <class U>
std::vector<int> BDSOutputROOTEventSampler<U>::getCharge()
{
  std::vector<int> result;
  result.reserve(n);
  if (!particleTable)
    {return result;}
  for (const auto& pid : partID)
    {result.push_back(particleTable->Charge(pid));}
  return result;
}
#include "globals.hh"
template <class U>
std::vector<U> BDSOutputROOTEventSampler<U>::getMass()
{
  std::vector<U> result(n);
  if (!particleTable)
    {return result;}
  for (const auto& pid : partID)
    {result.push_back((U)particleTable->Mass(pid));}
  return result;
}

template <class U>
std::vector<U> BDSOutputROOTEventSampler<U>::getRigidity()
{
  std::vector<U> result(n);
  if (!particleTable)
    {return result;}
  for (int i = 0; i < n; ++i)
    {result.push_back((U)particleTable->Rigidity(partID[i], energy[i]));}
  return result;
}

template <class U>
std::vector<bool> BDSOutputROOTEventSampler<U>::getIsIon()
{
  std::vector<bool> result(n);
  if (!particleTable)
    {return result;}
  for (const auto& pid : partID)
    {result.push_back(particleTable->IsIon(pid));}
  return result;
}

template <class U>
std::vector<int> BDSOutputROOTEventSampler<U>::getIonA()
{
  std::vector<int> result(n);
  if (!particleTable)
    {return result;}
  for (const auto& pid : partID)
    {result.push_back(particleTable->IonA(pid));}
  return result;
}

template <class U>
std::vector<int> BDSOutputROOTEventSampler<U>::getIonZ()
{
  std::vector<int> result(n);
  if (!particleTable)
    {return result;}
  for (const auto& pid : partID)
    {result.push_back(particleTable->IonZ(pid));}
  return result;
}

template <class U>
void BDSOutputROOTEventSampler<U>::FillCMR()
{
  if (!particleTable)
    {return;}
  for (int i = 0; i < n; ++i)
    {// loop over all existing entires in the branch vectors
      auto& pid = partID[i];
      auto& pInfo = particleTable->GetParticleInfo(pid);
      charge.push_back(pInfo.charge);
      mass.push_back(pInfo.mass);
      rigidity.push_back(pInfo.rigidity(energy[i]));
    }
}

template <class U>
void BDSOutputROOTEventSampler<U>::FillCMRI()
{
  if (!particleTable)
    {return;}
  for (int i = 0; i < n; ++i)
    {// loop over all existing entires in the branch vectors
      auto& pid = partID[i];
      if (particleTable->IsIon(pid))
	{
	  auto& ionInfo = particleTable->GetIonInfo(pid);
	  charge.push_back(ionInfo.charge);
	  mass.push_back(ionInfo.mass);
	  rigidity.push_back(ionInfo.rigidity(energy[i]));
	  isIon.push_back(true);
	  ionA.push_back(ionInfo.a);
	  ionZ.push_back(ionInfo.z);
	}
      else
	{// particle
	  auto& pInfo = particleTable->GetParticleInfo(pid);
	  charge.push_back(pInfo.charge);
	  mass.push_back(pInfo.mass);
	  rigidity.push_back(pInfo.rigidity(energy[i]));
	  isIon.push_back(false);
	  ionA.push_back(0);
	  ionZ.push_back(0);
	}
    }
}

template class BDSOutputROOTEventSampler<float>;
template class BDSOutputROOTEventSampler<double>;
