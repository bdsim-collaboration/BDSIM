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

  const G4Track           *aTrack      = step->GetTrack();
  const G4StepPoint        *prePoint   = step->GetPreStepPoint();
  const G4StepPoint       *postPoint   = step->GetPostStepPoint();
  const G4VProcess         *preProcess = prePoint->GetProcessDefinedStep();
  const G4VProcess        *postProcess = postPoint->GetProcessDefinedStep();
  G4VPhysicalVolume        *preVolume  = prePoint->GetPhysicalVolume();
  G4VPhysicalVolume       *postVolume  = postPoint->GetPhysicalVolume();

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
  BDSPhysicalVolumeInfo* preVolInfo  = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(preVolume);
  BDSPhysicalVolumeInfo* postVolInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(postVolume);


  if(preVolume)
    G4cout << preVolume->GetName() << " " << preVolInfo << G4endl;
  if(postVolume)
    G4cout << postVolume->GetName()<< " " << postVolInfo << G4endl;
  if(preVolInfo)
    G4cout << preVolInfo << G4endl;
  //    preS  =  preVolInfo->GetSPos() + prePosLocal.z();
  if(postVolInfo)
    G4cout << postVolInfo << G4endl;
  // postS = postVolInfo->GetSPos() + postPosLocal.z();
#endif

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

}

std::ostream& operator<< (std::ostream& out, BDSTrajectoryPoint const &p)
{
  out << p.GetPosition();
  return out;
}
