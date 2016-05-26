#include "G4Allocator.hh"
#include "G4ProcessType.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4NavigationHistory.hh"


#include "BDSTrajectoryPoint.hh"
#include "BDSDebug.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSEnergyCounterSD.hh"

#include <ostream>

G4Allocator<BDSTrajectoryPoint> bdsTrajectoryPointAllocator;

BDSTrajectoryPoint::BDSTrajectoryPoint():
  G4TrajectoryPoint(G4ThreeVector())
{
  preProcessType        = -1;
  preProcessSubType     = -1;
  postProcessType       = -1;
  postProcessSubType    = -1;
}

BDSTrajectoryPoint::BDSTrajectoryPoint(const G4Step* step):
  G4TrajectoryPoint(step->GetPostStepPoint()->GetPosition())
{
  preProcessType     = -1;
  preProcessSubType  = -1;
  postProcessType    = -1;
  postProcessSubType = -1;

  // const G4Track            *aTrack      = step->GetTrack();
  const G4StepPoint        *prePoint   = step->GetPreStepPoint();
  const G4StepPoint       *postPoint   = step->GetPostStepPoint();
  const G4VProcess         *preProcess = prePoint->GetProcessDefinedStep();
  const G4VProcess        *postProcess = postPoint->GetProcessDefinedStep();
  // G4VPhysicalVolume        *preVolume  = prePoint->GetPhysicalVolume();
  // G4VPhysicalVolume       *postVolume  = postPoint->GetPhysicalVolume();

  if(preProcess)
  {
    preProcessType    = preProcess->GetProcessType();
    preProcessSubType = preProcess->GetProcessSubType();
  }

  if(postProcess)
  {
    postProcessType    = postProcess->GetProcessType();
    postProcessSubType = postProcess->GetProcessSubType();
  }

  preWeight   = prePoint->GetWeight();
  postWeight  = postPoint->GetWeight();

  energy      = step->GetTotalEnergyDeposit();
  preEnergy   = prePoint->GetKineticEnergy();
  postEnergy  = postPoint->GetKineticEnergy();

  G4AffineTransform preT;
  G4AffineTransform postT;

  preT  =  prePoint->GetTouchableHandle()->GetHistory()->GetTopTransform();
  postT = postPoint->GetTouchableHandle()->GetHistory()->GetTopTransform();


  G4ThreeVector  prePos      =  prePoint->GetPosition();
  G4ThreeVector postPos      = postPoint->GetPosition();

  G4ThreeVector  prePosLocal = preT.TransformPoint(prePos);
  G4ThreeVector postPosLocal = preT.TransformPoint(postPos);

#if 0
  G4cout << __METHOD_NAME__ << "Append point" << G4endl;

  // G4cout << *BDSPhysicalVolumeInfoRegistry::Instance() << G4endl;
  if(preVolume)
    G4cout << __METHOD_NAME__ << "pre Volume name : " << preVolume->GetName() << " " << G4endl;
  if(postVolume)
    G4cout << __METHOD_NAME__ << "postVolume name : " << postVolume->GetName()<< " " << G4endl;

  G4Navigator *nav = BDSEnergyCounterSD::GetAuxilliaryNavigator();
  G4VPhysicalVolume* preVolumeNav  = nav->LocateGlobalPointAndSetup(prePos);
  G4VPhysicalVolume* postVolumeNav = nav->LocateGlobalPointAndSetup(postPos);


  BDSPhysicalVolumeInfo* preVolInfo  = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(preVolumeNav);
  BDSPhysicalVolumeInfo* postVolInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(postVolumeNav);

  if(preVolInfo)
    G4cout << __METHOD_NAME__ << "pre Volume info : " << preVolInfo  << " " << preVolInfo->GetName() << G4endl;
  if(postVolInfo)
    G4cout << __METHOD_NAME__ << "postVolume info : " << postVolInfo << " " << postVolInfo->GetName() << G4endl;

  if(preVolInfo)
  {
    preS = preVolInfo->GetSPos() + prePosLocal.z();
    G4cout << __METHOD_NAME__ << "pre Volume s    : " << preS << G4endl;
  }
  if(postVolInfo)
  {
    postS = postVolInfo->GetSPos() + postPosLocal.z();
    G4cout << __METHOD_NAME__ << "postVolume s    : " << postS << G4endl;
  }
#endif

#if 0
  // If the process type is not undefined or transportation...
  if(!((preProcess->GetProcessType() == fNotDefined) ||
       (preProcess->GetProcessType() == fTransportation)))
  {
    // ...and the particle changed momentum during the step, then this is a "scattering"
    // (momentum-changing non-transportation) process.
    G4ThreeVector pBefore = step->GetPreStepPoint()->GetMomentum();
    G4ThreeVector pAfter  = step->GetPostStepPoint()->GetMomentum();
    G4ThreeVector deltaP  = pAfter - pBefore;
  }
#endif
}

std::ostream& operator<< (std::ostream& out, BDSTrajectoryPoint const &p)
{
  out << p.GetPosition();
  return out;
}
