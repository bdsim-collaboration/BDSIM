#include "BDSGlobalConstants.hh" 
#include "BDSExecOptions.hh"
#include "BDSDebug.hh"
#include "BDSParticle.hh"
#include "BDSRunManager.hh"
#include "BDSSamplerSD.hh"
#include "BDSSamplerHit.hh"
#include "BDSTrajectory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ParticleDefinition.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4ThreeVector.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"

#include "G4AffineTransform.hh"

#include <vector>

#include "G4SDManager.hh"

BDSSamplerSD::BDSSamplerSD(G4String name, G4String type):
  G4VSensitiveDetector(name),
  itsHCID(-1),
  SamplerCollection(nullptr),
  itsType(type)
{
  itsCollectionName="Sampler_"+type;
  collectionName.insert(itsCollectionName);
}

BDSSamplerSD::~BDSSamplerSD()
{;}

void BDSSamplerSD::Initialize(G4HCofThisEvent* HCE)
{
  // Create Sampler hits collection
  SamplerCollection = new BDSSamplerHitsCollection(SensitiveDetectorName,itsCollectionName);

  // Record id for use in EventAction to save time
  if (itsHCID < 0)
    {itsHCID = G4SDManager::GetSDMpointer()->GetCollectionID(itsCollectionName);}
  HCE->AddHitsCollection(itsHCID,SamplerCollection);
}

G4bool BDSSamplerSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*readOutTH */)
{
  G4Track* theTrack         = aStep->GetTrack();
  BDSTrajectory* bdsTraj    = new BDSTrajectory(theTrack);
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  
  //Do not store hit if the particle is not on the boundary 
  if(preStepPoint->GetStepStatus()!=fGeomBoundary)
    {
      delete bdsTraj;
      return false;
    }
  //unique ID of track
  G4int TrackID = theTrack->GetTrackID();
  //unique ID of track's mother
  G4int ParentID = theTrack->GetParentID();
  //time since track creation
  G4double t = theTrack->GetGlobalTime();
  //total track energy 
  G4double energy = theTrack->GetTotalEnergy();
  //Turn Number
  G4int turnstaken = BDSGlobalConstants::Instance()->GetTurnsTaken();  
  
  //current particle position (global)
  G4ThreeVector pos = theTrack->GetPosition();
  //total track length
  G4double s = theTrack->GetTrackLength();
  if(ParentID != 0) s = pos.z();
  //G4ThreeVector pos = preStepPoint->GetPosition();
  //current particle direction (global)
  G4ThreeVector momDir = theTrack->GetMomentumDirection();
  //G4ThreeVector momDir = preStepPoint->GetMomentumDirection();
  
  // Get Translation and Rotation of Sampler Volume w.r.t the World Volume
  // as described in Geant4 FAQ's: http://geant4.cern.ch/support/faq.shtml
  G4AffineTransform tf(preStepPoint->GetTouchableHandle()->GetHistory()->GetTopTransform());
  //      const G4RotationMatrix Rot=tf.NetRotation();
  //      const G4ThreeVector Trans=-tf.NetTranslation();

  // As with BDSEnergyCounterSD??? TODO Check
  G4VPhysicalVolume *theVolume = aStep->GetPostStepPoint()->GetPhysicalVolume();
  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(theVolume);



  //Old method - works for standard Samplers, but not samplers within a deeper
  //hierarchy of volumes (e.g. Mokka built samplers)
  //const G4RotationMatrix* Rot=theTrack->GetVolume()->GetFrameRotation();
  //const G4ThreeVector Trans=theTrack->GetVolume()->GetFrameTranslation();
  
  //      G4ThreeVector LocalPosition=pos+Trans; 
  //      G4ThreeVector LocalDirection=Rot*momDir; 
  G4ThreeVector LocalPosition  = tf.TransformPoint(pos);
  G4ThreeVector LocalDirection = tf.TransformAxis(momDir);
  BDSParticle   local(LocalPosition,LocalDirection,energy,t);

  G4int nEvent = BDSRunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  
  nEvent+=BDSGlobalConstants::Instance()->GetEventNumberOffset();
  
  //  G4int nSampler    = theTrack->GetVolume()->GetCopyNo();
  G4String SampName = theTrack->GetVolume()->GetName();
  // remove end part "_pv"
  G4String removeEnd = "_pv";
  SampName = SampName.substr(0,SampName.find(removeEnd));

  G4int    PDGtype = theTrack->GetDefinition()->GetPDGEncoding();
  G4String pName   = theTrack->GetDefinition()->GetParticleName();
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Sampler name:  " << SampName << G4endl;
  G4cout << __METHOD_NAME__ << "Particle name: " << pName    << G4endl;  
  G4cout << __METHOD_NAME__ << "PDG encoding:  " << PDGtype  << G4endl;  
  G4cout << __METHOD_NAME__ << "TrackID:       " << TrackID  << G4endl;  
#endif
  
  G4ThreeVector vtx               = theTrack->GetVertexPosition();
  G4ThreeVector dir               = theTrack->GetVertexMomentumDirection();
  G4ThreeVector posLastScatter    = bdsTraj->GetPositionOfLastScatter(theTrack);
  G4ThreeVector momDirLastScatter = bdsTraj->GetMomDirAtLastScatter(theTrack);
  G4double timeLastScatter        = bdsTraj->GetTimeAtLastScatter(theTrack);
  G4double energyLastScatter      = bdsTraj->GetEnergyAtLastScatter(theTrack);
  G4double vertexEnergy           = theTrack->GetVertexKineticEnergy() + theTrack->GetParticleDefinition()->GetPDGMass();
  G4double vertexTime             = bdsTraj->GetTimeAtVertex(theTrack);

  // store production/scatter point
  BDSParticle lastScatter(posLastScatter,momDirLastScatter,energyLastScatter,timeLastScatter);

  //production point
  BDSParticle production(vtx,dir,vertexEnergy,vertexTime);

  // global point
  BDSParticle global(pos,momDir,energy,t);

  G4double weight = theTrack->GetWeight();
 
  // process that creating the particle
  G4String process = "";
  if(theTrack->GetCreatorProcess()) 
    {process = theTrack->GetCreatorProcess()->GetProcessName();}
  
  BDSSamplerHit* smpHit = new BDSSamplerHit(SampName,
                                            BDSGlobalConstants::Instance()->GetInitialPoint(),
                                            production,
                                            lastScatter,
                                            local,
                                            global,
                                            s,
                                            weight,
                                            PDGtype,
                                            nEvent,
                                            ParentID,
                                            TrackID,
                                            turnstaken,
                                            itsType,
                                            process,
                                            theInfo->GetBeamlineIndex()); // TODO Check with LN why this does not work for samplers
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " Sampler : " << SampName << G4endl;
  G4cout << __METHOD_NAME__ << " Storing hit: E, x, y, z, xPrime, yPrime" << G4endl;
  G4cout << __METHOD_NAME__ << " " << energy <<" "  << LocalPosition.x() << " " << LocalPosition.y() << " " << LocalPosition.z() << " " << LocalDirection.x() << " " << LocalDirection.y() << G4endl;
  G4cout << __METHOD_NAME__ << " Storing hit: E, x, y, z, xPrime, yPrime" << G4endl;
  G4cout << __METHOD_NAME__ << " " << energy <<" "  << pos.x() << " " << pos.y() << " " << pos.z() << " " << LocalDirection.x() << " " << LocalDirection.y() << G4endl;
  G4cout << __METHOD_NAME__ << " entries in hits collection before inserting hit: " << SamplerCollection->entries() << G4endl;
#endif
  SamplerCollection->insert(smpHit);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " entries in hits collection after inserting hit: " << SamplerCollection->entries() << G4endl;
#endif

  delete bdsTraj;
  
  //The hit was stored, so the return value is "true".
  return true;
}
