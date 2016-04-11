#include "BDSTrajectoryPoint.hh"
#include <map>
#include "G4Allocator.hh"
#include "G4ProcessType.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include <ostream>

G4Allocator<BDSTrajectoryPoint> bdsTrajectoryPointAllocator;

BDSTrajectoryPoint::BDSTrajectoryPoint():
  G4TrajectoryPoint(G4ThreeVector())
{
  currentProcess      = nullptr;
  isScatteringProcess = false;
  trackID             = -1;
  vertexPosition      = G4ThreeVector();
}

BDSTrajectoryPoint::BDSTrajectoryPoint(const G4Step* step):
  G4TrajectoryPoint(step->GetPostStepPoint()->GetPosition())
{
  const G4Track* aTrack = step->GetTrack();
  currentProcess      = nullptr;
  isScatteringProcess = false;
  trackID             = -1;
  vertexPosition      = aTrack->GetVertexPosition();
  trackID             = aTrack->GetTrackID();
  currentProcess      = step->GetPostStepPoint()->GetProcessDefinedStep();

  G4ProcessType ptype = fNotDefined;
  if(currentProcess)
    {ptype = currentProcess->GetProcessType();}
  
  isScatteringProcess = false;
  // If the process type is not undefined or transportation...
  if(!((ptype == fNotDefined) || (ptype == fTransportation))) 
    {
      // ...and the particle changed momentum during the step, then this is a "scattering"
      // (momentum-changing non-transportation) process.
      G4ThreeVector pBefore = step->GetPreStepPoint()->GetMomentum();
      G4ThreeVector pAfter  = step->GetPostStepPoint()->GetMomentum();
      G4ThreeVector deltaP  = pAfter - pBefore;
      if (deltaP.x() != 0 || deltaP.y() != 0 || deltaP.z() != 0)
	{isScatteringProcess = true;}
    }
}

std::ostream& operator<< (std::ostream& out, BDSTrajectoryPoint const &p)
{
  if(p.currentProcess)
    {
      out << "BDSTrajectoryPoint: ";
      out << "current process = " << p.currentProcess->GetProcessName() << ", that ";
      G4String result = "isn't";
      if (p.isScatteringProcess)
	{result = "is";}
      out << result << " a scattering process" << G4endl;
    }
  return out;
}
