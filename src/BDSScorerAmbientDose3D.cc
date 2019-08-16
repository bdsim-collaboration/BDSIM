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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSHistBinMapper3D.hh"
#include "BDSScorerConversionLoader.hh"
#include "BDSScorerAmbientDose3D.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4PhysicsVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"
#include "G4UnitsTable.hh"

#include <fstream>
#include <string>

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

BDSScorerAmbientDose3D::BDSScorerAmbientDose3D(const G4String            scorer_name,
					       const BDSHistBinMapper3D* mapperIn,
					       const G4String            pathname,
					       G4int ni,
					       G4int nj,
					       G4int nk,
					       G4int depi,
					       G4int depj,
					       G4int depk):
  G4VPrimitiveScorer(scorer_name),
  HCID3D(-1),
  EvtMap3D(nullptr),
  fDepthi(depi),fDepthj(depj),fDepthk(depk),
  mapper(mapperIn)
{
  fNi = ni;
  fNj = nj;
  fNk = nk;
  
  if (pathname.empty())
    {throw BDSException(__METHOD_NAME__, "no conversionFactorPath provided for \"" + scorer_name + "\" - required");}
  
  G4String filePath = BDS::GetFullPath(pathname);
  
  BDSScorerConversionLoader<std::ifstream> loader;
  
  //TODO use an option instead of hardcoding the filename
  conversionFactor_protons = loader.Load(filePath+"/h10protons.dat");
  conversionFactor_neutrons = loader.Load(filePath+"/h10neutrons.dat");
  conversionFactor_gammas = loader.Load(filePath+"/h10photons.dat");
  conversionFactor_electrons = loader.Load(filePath+"/h10electrons.dat");
}

BDSScorerAmbientDose3D::~BDSScorerAmbientDose3D()
{;}

G4bool BDSScorerAmbientDose3D::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  G4double stepLength = aStep->GetStepLength()/CLHEP::cm;
  G4double radiation_quantity = 0;
  
  // fluence computation
  if (!BDS::IsFinite(stepLength))
    {return false;}
  
  const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
  G4VSolid* solid = touchable->GetSolid();
  G4double cubicVolume = 1;
  auto search = volumeCache.find(solid);
  if (search == volumeCache.end())
    {// cache the volume of the volume to avoid repeated calculation
      // this is simple for a cube, but expensive for a cuttubs
      cubicVolume = solid->GetCubicVolume()/CLHEP::cm3;
      volumeCache[solid] = cubicVolume;
    }
  else
    {cubicVolume = search->second;}
  
  G4double CellFlux = stepLength / cubicVolume;
  CellFlux *= aStep->GetPreStepPoint()->GetWeight();
  
  G4double energy = aStep->GetPreStepPoint()->GetKineticEnergy();
  
  G4double factor = GetConversionFactor(aStep->GetTrack()->GetDefinition()->GetPDGEncoding(), energy);
  radiation_quantity = CellFlux*factor;
  G4int index = GetIndex(aStep);
  
  EvtMap3D->add(index,radiation_quantity);
  
  return true;
}

G4double BDSScorerAmbientDose3D::GetConversionFactor(G4int particleID, G4double energy)
{
  switch (particleID){
  case 2212 : //protons
    return conversionFactor_protons->Value(energy);
    break;
  case 2112 : //neutrons
    return conversionFactor_neutrons->Value(energy);
    break;
  case 22 : //gamma
    return conversionFactor_gammas->Value(energy);
    break;
  case 11 : //electron
    return conversionFactor_electrons->Value(energy);
    break;
  default:
    return 0;
    break;
  }
}

void BDSScorerAmbientDose3D::Initialize(G4HCofThisEvent* HCE)
{
  EvtMap3D = new G4THitsMap<G4double>(detector->GetName(),
				      GetName());
  if (HCID3D < 0)
    {HCID3D = GetCollectionID(0);}
  HCE->AddHitsCollection(HCID3D,EvtMap3D);
}

void BDSScorerAmbientDose3D::EndOfEvent(G4HCofThisEvent* /*HEC*/)
{;}

void BDSScorerAmbientDose3D::clear()
{
  EvtMap3D->clear();
}

G4int BDSScorerAmbientDose3D::GetIndex(G4Step* aStep)
{
  const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
  G4int i = touchable->GetReplicaNumber(fDepthi);
  G4int j = touchable->GetReplicaNumber(fDepthj);
  G4int k = touchable->GetReplicaNumber(fDepthk);
  
  if (i<0 || j<0 || k<0)
    {
      G4ExceptionDescription ED;
      ED << "GetReplicaNumber is negative" << G4endl
	 << "touchable->GetReplicaNumber(fDepthi) returns i,j,k = "
	 << i << "," << j << "," << k << " for volume "
	 << touchable->GetVolume(fDepthi)->GetName() << ","
	 << touchable->GetVolume(fDepthj)->GetName() << ","
	 << touchable->GetVolume(fDepthk)->GetName() << G4endl;
      G4Exception("PSRadiationQuantity3D::GetIndex","DetPS0006",JustWarning,ED);
    }
  
  G4int globalIndex = mapper->GlobalFromIJKIndex(i,j,k); // x,y,z
  //G4int oldResult = i*fNj*fNk+j*fNk+k;
  return globalIndex;
}
