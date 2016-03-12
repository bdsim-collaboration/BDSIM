#include "BDSTrajectoryPoint.hh"
#include <map>
#include "G4Allocator.hh"
#include "G4ProcessType.hh"
#include "G4VProcess.hh"

#include "globals.hh" // geant4 types / globals

#include <ostream>

G4Allocator<BDSTrajectoryPoint> bdsTrajectoryPointAllocator;

BDSTrajectoryPoint::BDSTrajectoryPoint()
{
  _currentProcess      = nullptr;
  _isScatteringProcess = false;
  _trackID             = -1;
}

BDSTrajectoryPoint::BDSTrajectoryPoint(const G4Track* aTrack):
  G4TrajectoryPoint(aTrack->GetPosition())
{
  _currentProcess      = nullptr;
  _isScatteringProcess = false;
  _trackID             = -1;
  _vertexPosition      = aTrack->GetVertexPosition();
  _trackID             = aTrack->GetTrackID();
  _currentProcess      = aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep();

  G4ProcessType ptype = fNotDefined;
  if(_currentProcess)
    {ptype = _currentProcess->GetProcessType();}
  
  _isScatteringProcess = false;
  // If the process type is not undefined or transportation...
  if(!((ptype == fNotDefined) || (ptype == fTransportation))) 
    {
      // ...and the particle changed momentum during the step, then this is a "scattering"
      // (momentum-changing non-transportation) process.
      G4ThreeVector deltaP = aTrack->GetStep()->GetDeltaMomentum();
      if (deltaP.x() != 0 || deltaP.y() != 0 || deltaP.z() != 0)
	{_isScatteringProcess = true;}
    }
}

BDSTrajectoryPoint::~BDSTrajectoryPoint()
{;}

std::ostream& operator<< (std::ostream& out, BDSTrajectoryPoint const &p)
{
  if(p._currentProcess)
    {
      out << "BDSTrajectoryPoint: ";
      out << "current process = " << p._currentProcess->GetProcessName() << ", ";
      G4String result = "isn't";
      if (p._isScatteringProcess)
	{result = "is";}
      out << result << " a scattering process" << G4endl;
    }
  return out;
}

void BDSTrajectoryPoint::printData()
{
  G4cout << *(this) << G4endl;
}
