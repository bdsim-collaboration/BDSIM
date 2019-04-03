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

#include "BDSScorerQuantity3D.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"
#include "G4UnitsTable.hh"

#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSUtilities.hh"
#include "BDSScorerConversionLoader.hh"

#include <fstream>

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif


BDSScorerQuantity3D::BDSScorerQuantity3D(const G4String scorer_name,
					 const G4String filename,
					 G4int ni,
					 G4int nj,
					 G4int nk,
					 G4int depi,
					 G4int depj,
					 G4int depk):
  G4VPrimitiveScorer(scorer_name),
  HCID3D(-1),EvtMap3D(nullptr), fDepthi(depi),fDepthj(depj),fDepthk(depk)
{
  fNi=ni;
  fNj=nj;
  fNk=nk;
  
  if (filename.empty())
    {throw BDSException(__METHOD_NAME__, "no conversionFactorFile provided for \"" + scorer_name + "\" - required");}

  G4String filePath = BDS::GetFullPath(filename);
  if (filePath.rfind("gz") != std::string::npos)
    {
#ifdef USE_GZSTREAM
      BDSScorerConversionLoader<igzstream> loader;
      conversionFactor = loader.Load(filePath);
#else
      G4cout << "Compressed file loading - but BDSIM not compiled with ZLIB." << G4endl; exit(1);
#endif
    }
  else
    {
      BDSScorerConversionLoader<std::ifstream> loader;
      conversionFactor = loader.Load(filePath);
    }
}

BDSScorerQuantity3D::~BDSScorerQuantity3D()
{
}

G4bool BDSScorerQuantity3D::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
    G4double stepLength = aStep->GetStepLength()/CLHEP::cm;
    G4double radiation_quantity;

    /// Fluence computation
    if ( stepLength == 0. ) return FALSE;

    const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
    G4VSolid* solid = touchable->GetSolid();
    G4double cubicVolume = solid->GetCubicVolume()/CLHEP::cm3;

    G4double CellFlux = (stepLength / cubicVolume);
    CellFlux *= aStep->GetPreStepPoint()->GetWeight();

    G4double energy = (aStep->GetPreStepPoint()->GetKineticEnergy());
    G4double factor = conversionFactor->Value(energy);
    radiation_quantity = CellFlux*factor;
    G4int index = GetIndex(aStep);

    EvtMap3D->add(index,radiation_quantity);
    return true;
}

void BDSScorerQuantity3D::Initialize(G4HCofThisEvent* HCE)
{
    EvtMap3D = new G4THitsMap<G4double>(detector->GetName(),
                                        GetName());
    if ( HCID3D < 0 ) HCID3D = GetCollectionID(0);
    HCE->AddHitsCollection(HCID3D,EvtMap3D);
}

void BDSScorerQuantity3D::EndOfEvent(G4HCofThisEvent*)
{;}

void BDSScorerQuantity3D::clear()
{

    EvtMap3D->clear();
}

G4int BDSScorerQuantity3D::GetIndex(G4Step* aStep)
{
    const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
    G4int i = touchable->GetReplicaNumber(fDepthi);
    G4int j = touchable->GetReplicaNumber(fDepthj);
    G4int k = touchable->GetReplicaNumber(fDepthk);

    if(i<0||j<0||k<0)
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

    return i*fNj*fNk+j*fNk+k;
}
