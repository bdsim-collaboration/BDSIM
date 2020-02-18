/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "BDSHitSamplerLink.hh"
#include "BDSLinkRegistry.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSSDSamplerLink.hh"
#include "BDSUtilities.hh"

#include "G4AffineTransform.hh"
#include "G4DynamicParticle.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Types.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"

#include <vector>

BDSSDSamplerLink::BDSSDSamplerLink(G4String name):
  BDSSensitiveDetector("samplerlink/" + name),
  samplerLinkCollection(nullptr),
  itsCollectionName(name),
  itsHCID(-1),
  registry(nullptr)
{
  collectionName.insert(name);
}

BDSSDSamplerLink::~BDSSDSamplerLink()
{;}

void BDSSDSamplerLink::Initialize(G4HCofThisEvent* HCE)
{
  // Create SamplerLink hits collection
  samplerLinkCollection = new BDSHitsCollectionSamplerLink(GetName(),itsCollectionName);

  // Record id for use in EventAction to save time - slow string lookup by collection name
  if (itsHCID < 0)
    {itsHCID = G4SDManager::GetSDMpointer()->GetCollectionID(samplerLinkCollection);}
  HCE->AddHitsCollection(itsHCID,samplerLinkCollection);
}

G4bool BDSSDSamplerLink::ProcessHits(G4Step* aStep, G4TouchableHistory* /*readOutTH*/)
{
  // Do not store hit if the particle pre step point is not on the boundary
  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();
  if (postStepPoint->GetStepStatus() != fGeomBoundary)
    {return false;} // this step was not stored
  
  G4Track* track    = aStep->GetTrack();
  const G4DynamicParticle* dp = track->GetDynamicParticle();
  G4int TrackID     = track->GetTrackID();           // unique ID of track
  G4int ParentID    = track->GetParentID();          // unique ID of track's mother
  G4double T        = track->GetGlobalTime();        // time since beginning of event
  G4double energy   = track->GetTotalEnergy();       // total track energy
  G4double charge   = dp->GetCharge(); // dynamic effective charge
  G4ThreeVector pos = track->GetPosition();          // current particle position (global)
  G4ThreeVector mom = track->GetMomentumDirection(); // current particle direction (global) (unit)
  G4double weight   = track->GetWeight();            // weighting
  G4int nElectrons  = dp->GetTotalOccupancy();
  G4double mass     = dp->GetMass();

  // The copy number from the physical volume is used as our unique sampler ID
  G4int samplerID = track->GetVolume()->GetCopyNo();

  // Initialize variables for the local position and direction
  G4ThreeVector localPosition;
  G4ThreeVector localDirection;
  
  // Get coordinate transform and prepare local coordinates
  G4Transform3D globalToLocal = G4Transform3D::Identity;
  if (registry)
    {registry->TransformInverse(samplerID);}
  if (globalToLocal != G4Transform3D::Identity)
    {
      // The global to local transform is defined in the registry.
      // Cast 3 vector to 'point' to transform position (required to be explicit for * operator)
      localPosition  = globalToLocal * (HepGeom::Point3D<G4double>)pos;
      // Now, if the sampler is infinitely thin, the local z should be 0, but it's finite.
      // Account for this by purposively setting local z to be 0.
      localPosition.setZ(0.0);
      // Cast 3 vector to 3 vector to transform vector (required to be explicit for * operator)
      localDirection = globalToLocal * (HepGeom::Vector3D<G4double>)mom;
    }

  G4int    PDGtype     = track->GetDefinition()->GetPDGEncoding();
  G4String pName       = track->GetDefinition()->GetParticleName();

  BDSParticleCoordsFull coords(localPosition.x(),
			       localPosition.y(),
			       localPosition.z(),
			       localDirection.x(),
			       localDirection.y(),
			       localDirection.z(),
			       T,
			       localPosition.z(), // s = z here
			       energy,
			       weight);

  BDSHitSamplerLink* smpHit = new BDSHitSamplerLink(samplerID,
						    coords,
						    mass,
						    charge,
						    PDGtype,
						    ParentID,
						    TrackID,
						    nElectrons);
  
  samplerLinkCollection->insert(smpHit);
  return true; // the hit was stored
}

G4VHit* BDSSDSamplerLink::last() const
{
  BDSHitSamplerLink* lastHit = samplerLinkCollection->GetVector()->back();
  return dynamic_cast<G4VHit*>(lastHit);
}
