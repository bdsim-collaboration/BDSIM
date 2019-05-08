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
#include "BDSAuxiliaryNavigator.hh"
#include "BDSHitEnergyDeposition.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSSDApertureImpacts.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AffineTransform.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepStatus.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"
#include "Randomize.hh"

BDSSDApertureImpacts::BDSSDApertureImpacts(G4String name):
  G4VSensitiveDetector("aperture_impacts/"+name),
  colName(name),
  hits(nullptr),
  HCIDe(-1),
  auxNavigator(new BDSAuxiliaryNavigator())
{
  collectionName.insert(colName);
}

BDSSDApertureImpacts::~BDSSDApertureImpacts()
{
  delete auxNavigator;
}

void BDSSDApertureImpacts::Initialize(G4HCofThisEvent* HCE)
{
  hits = new BDSHitsCollectionApertureImpact(GetName(),colName);
  if (HCIDe < 0)
    {HCIDe = G4SDManager::GetSDMpointer()->GetCollectionID(hits);}
  HCE->AddHitsCollection(HCIDe,hits);
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Hits Collection ID: " << HCIDe << G4endl;
#endif
}

G4bool BDSSDApertureImpacts::ProcessHits(G4Step* aStep,
					 G4TouchableHistory* /*th*/)
{
  // check if pre step point is on geometry boundary - ie first step into a volume
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  G4StepStatus preStepStatus = preStepPoint->GetStepStatus();
  if (preStepStatus != G4StepStatus::fGeomBoundary)
    {return false;} // wasn't on a boundary - don't generate a hit

  // check if momentum is away from accelerator axis - ie leaving vacuum and entering
  // beam pip, not coming in from outside
  // get local coordinates
  BDSStep localPosMom = auxNavigator->ConvertToLocal(preStepPoint->GetPosition(),
  preStepPoint->GetMomentum(),
  0.1*CLHEP::mm);

  G4ThreeVector momLocal = localPosMom.PostStepPoint();
  G4ThreeVector localUnitZ(0,0,1);
  G4ThreeVector perpPart = localUnitZ.perpPart(momLocal);
  G4cout << "perpPart " << perpPart << G4endl;

  G4VSolid* preStepSolid = preStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetSolid();
  G4ThreeVector surfaceNormal = preStepSolid->SurfaceNormal(preStepPoint->GetPosition());
  G4double dotProduct = surfaceNormal.dot(localPosMom.PreStepPoint());
  G4bool leavingBeamPipe = true;
  if (!leavingBeamPipe)
    {return false;}

  G4ThreeVector momLocalUnit = momLocal.unit();
  G4double S = 0;
  G4double x = 0;
  G4double y = 0;
  G4int    turnsTaken    = BDSGlobalConstants::Instance()->TurnsTaken();
  G4int    beamlineIndex = -1;

  G4Track* track = aStep->GetTrack();

  // volume is from curvilinear coordinate parallel geometry
  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(localPosMom.VolumeForTransform());
  
  // declare lambda for updating parameters if info found (avoid duplication of code)
  auto UpdateParams = [&](BDSPhysicalVolumeInfo* info)
    {
      S             = info->GetSPos();
      beamlineIndex = info->GetBeamlineIndex();
    };
  
  if (theInfo)
    {UpdateParams(theInfo);}
  else
    {
      // Try again but with the pre step point only
      G4ThreeVector unitDirection = (posafter - posbefore).unit();
      BDSStep stepLocal2 = auxNavigator->ConvertToLocal(posbefore, unitDirection);
      theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(stepLocal2.VolumeForTransform());
      if (theInfo)
	{UpdateParams(theInfo);}
      else
	{
	  // Try yet again with just a slight shift (100um is bigger than any padding space).
	  G4ThreeVector shiftedPos = posbefore + 0.1*CLHEP::mm*unitDirection;
	  stepLocal2 = auxNavigator->ConvertToLocal(shiftedPos, unitDirection);
	  theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(stepLocal2.VolumeForTransform());
	  if (theInfo)
	    {UpdateParams(theInfo);}
	  else
	    {
#ifdef BDSDEBUG
	      G4cerr << "No volume info for ";
	      auto vol = stepLocal.VolumeForTransform();
	      if (vol)
		{G4cerr << vol->GetName() << G4endl;}
	      else
		{G4cerr << "Unknown" << G4endl;}
#endif
	      // unphysical default value to allow easy identification in output
	      sAfter        = -1000;
	      sBefore       = -1000;
	      beamlineIndex = -2;
	    }
	}
    }

  turnsTaken  =
  
  //create hits and put in hits collection of the event
  BDSHitApertureImpact* hit = new BDSHitApertureImpact(preStepPoint->GetTotalEnergy(),
                                                       preStepPoint->GetKineticEnergy(),
                                                       S,
                                                       x,
                                                       y,
                                                       momLocalUnit.x(),
                                                       momLocalUnit.y(),
                                                       preStepPoint->GetWeight(),
                                                       preStepPoint->GetGlobalTime(),
                                                       track->GetDefinition()->GetPDGEncoding(),
                                                       track->GetTrackID(),
                                                       track->GetParentID(),
                                                       turnsTaken,
                                                       beamlineIndex);
  
  // don't worry, won't add 0 energy tracks as filtered at top by if statement
  hits->insert(hit);
   
  return true;
}
