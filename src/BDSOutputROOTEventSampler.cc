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
#include "CLHEP/Units/SystemOfUnits.h"
#include "BDSSamplerHit.hh"
#endif

templateClassImp(BDSOutputROOTEventSampler)

template <class T>
BDSOutputROOTGeant4Data* BDSOutputROOTEventSampler<T>::particleTable = nullptr;

template <class T>
BDSOutputROOTEventSampler<T>::BDSOutputROOTEventSampler():samplerName("sampler")
{
  Flush();
}

template <class T>
BDSOutputROOTEventSampler<T>::BDSOutputROOTEventSampler(std::string samplerNameIn):
  samplerName(samplerNameIn)
{
  Flush();
}

template
<class T> BDSOutputROOTEventSampler<T>::~BDSOutputROOTEventSampler()
{;}

#ifndef __ROOTBUILD__ 
template <class T>
void BDSOutputROOTEventSampler<T>::Fill(G4double E,
					G4double xIn,
					G4double yIn,
					G4double zIn,
					G4double xpIn,
					G4double ypIn,
					G4double zpIn,
					G4double tIn,
					G4double weightIn,
					G4int    partIDIn,
					G4int    /*nEvent*/,
					G4int    TurnsTaken,
					G4int    beamlineIndex)
{
  n++;
  z = (T) (zIn / CLHEP::m);
  S = (T) (0 / CLHEP::m);

  energy.push_back((T &&) (E / CLHEP::GeV));
  x.push_back((T &&) (xIn / CLHEP::m));
  y.push_back((T &&) (yIn / CLHEP::m));

  xp.push_back((T &&) (xpIn / CLHEP::radian));
  yp.push_back((T &&) (ypIn / CLHEP::radian));
  zp.push_back((T &&) (zpIn / CLHEP::radian));
  t.push_back((T &&) (tIn / CLHEP::ns));
  weight.push_back((const T &) weightIn);
  partID.push_back(partIDIn);
  parentID.push_back(0);
  turnNumber.push_back(TurnsTaken);
  modelID = beamlineIndex;
}

template <class T>
void BDSOutputROOTEventSampler<T>::Fill(const BDSSamplerHit* hit)
{
  // get single values
  n++;
  z = (T) (hit->GetZ() / CLHEP::m);
  S = (T) (hit->GetS() / CLHEP::m);

  energy.push_back((T &&) (hit->GetTotalEnergy() / CLHEP::GeV));
  x.push_back((T &&) (hit->GetX() / CLHEP::m));
  y.push_back((T &&) (hit->GetY() / CLHEP::m));

  xp.push_back((T &&) (hit->GetXPrime() / CLHEP::radian));
  yp.push_back((T &&) (hit->GetYPrime() / CLHEP::radian));
  zp.push_back((T &&) (hit->GetZPrime() / CLHEP::radian));

  t.push_back((T &&) (hit->GetT() / CLHEP::ns));
  modelID = hit->GetBeamlineIndex();

  weight.push_back((T &&) hit->GetWeight());
  partID.push_back(hit->GetPDGtype());
  parentID.push_back(hit->GetParentID());
  trackID.push_back(hit->GetTrackID());
  turnNumber.push_back(hit->GetTurnsTaken());
}
//#else
//void BDSOutputROOTEventSampler::SetBranchAddress(TTree *)
//{}
#endif

template <class T> void BDSOutputROOTEventSampler<T>::SetBranchAddress(TTree *)
{;}

template <class T> void BDSOutputROOTEventSampler<T>::Flush()
{
  n = 0;
  energy.clear();
  x.clear();
  y.clear();
  z = 0.0;
  xp.clear();
  yp.clear();
  zp.clear();
  t.clear();
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

template <class T>
std::vector<int> BDSOutputROOTEventSampler<T>::getCharge()
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
template <class T>
std::vector<T> BDSOutputROOTEventSampler<T>::getMass()
{
  std::vector<T> result(n);
  if (!particleTable)
    {return result;}
  for (const auto& pid : partID)
    {result.push_back((T)particleTable->Mass(pid));}
  return result;
}

template <class T>
std::vector<T> BDSOutputROOTEventSampler<T>::getRigidity()
{
  std::vector<T> result(n);
  if (!particleTable)
    {return result;} 
  for (int i = 0; i < n; ++i)
    {result.push_back((T)particleTable->Rigidity(partID[i], energy[i]));}
  return result;
}

template <class T>
std::vector<bool> BDSOutputROOTEventSampler<T>::getIsIon()
{
  std::vector<bool> result(n);
  if (!particleTable)
    {return result;}
  for (const auto& pid : partID)
    {result.push_back(particleTable->IsIon(pid));}
  return result;
}

template <class T>
std::vector<int> BDSOutputROOTEventSampler<T>::getIonA()
{
  std::vector<int> result(n);
  if (!particleTable)
    {return result;}
  for (const auto& pid : partID)
    {result.push_back(particleTable->IonA(pid));}
  return result;
}

template <class T>
std::vector<int> BDSOutputROOTEventSampler<T>::getIonZ()
{
  std::vector<int> result(n);
  if (!particleTable)
    {return result;}
  for (const auto& pid : partID)
    {result.push_back(particleTable->IonZ(pid));}
  return result;
}

template <class T>
void BDSOutputROOTEventSampler<T>::FillCMR()
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

template <class T>
void BDSOutputROOTEventSampler<T>::FillCMRI()
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
