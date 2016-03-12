#include "BDSTrajectoryPoint.hh"
#include <map>
#include "G4Allocator.hh"
#include "G4ProcessType.hh"
#include "G4VProcess.hh"

#include "globals.hh" // geant4 types / globals

#include <ostream>

G4Allocator<BDSTrajectoryPoint> bdsTrajectoryPointAllocator;

BDSTrajectoryPoint::BDSTrajectoryPoint(){
  _currentProcess=nullptr;
  _isScatteringProcess=false;
  _trackID = -1;
}
BDSTrajectoryPoint::BDSTrajectoryPoint(const G4Track* aTrack):G4TrajectoryPoint(aTrack->GetPosition())
{
  _currentProcess=nullptr;
  _isScatteringProcess=false;
  _trackID = -1;
  //  G4cout << "Getting current process..." << G4endl;
  if(aTrack){
    _vertexPosition=aTrack->GetVertexPosition();
    _trackID = aTrack->GetTrackID();
    if(aTrack->GetStep()){
      _currentProcess = aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep();
      //      G4cout << "Getting current process type..." << G4endl;
      G4ProcessType ptype;
      if(_currentProcess){
	ptype = _currentProcess->GetProcessType();
      } else {
	ptype = fNotDefined;
      }
      //      G4cout << "Getting isScattering..." << G4endl;
      _isScatteringProcess = false;
      if(!((ptype == fNotDefined) || (ptype == fTransportation))){  //If the process type is not undefined or transportation...
	if ( aTrack -> GetStep() -> GetDeltaMomentum().x() != 0){ //...and the particle changed momentum during the step..
	  _isScatteringProcess = true; //...then this is a "scattering" (momentum-changing non-transportation) process.
	}
	if ( aTrack -> GetStep() -> GetDeltaMomentum().y() != 0){ //same for y and z components of momentum.
	  _isScatteringProcess = true; 
	}
	if ( aTrack -> GetStep() -> GetDeltaMomentum().z() != 0){ //...and the particle changed momentum during the step..
	  _isScatteringProcess = true; 
	}
      }
    }
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
