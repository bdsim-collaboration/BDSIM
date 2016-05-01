#include "G4Allocator.hh"
#include "G4ProcessType.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include "BDSTrajectoryPoint.hh"
#include "BDSDebug.hh"

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

  const G4Track    *aTrack      = step->GetTrack();
  const G4VProcess *preProcess  = step->GetPreStepPoint()->GetProcessDefinedStep();
  const G4VProcess *postProcess = step->GetPostStepPoint()->GetProcessDefinedStep();

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
