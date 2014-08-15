#include "BDSTrajectoryPoint.hh"
#include <map>
#include "G4Allocator.hh"
#include "G4ProcessType.hh"
#include "G4VProcess.hh"

G4Allocator<BDSTrajectoryPoint> bdsTrajectoryPointAllocator;

BDSTrajectoryPoint::BDSTrajectoryPoint(){
  _currentProcess=NULL;
  _isScatteringProcess=false;
}
BDSTrajectoryPoint::BDSTrajectoryPoint(const G4Track* aTrack):G4TrajectoryPoint(aTrack->GetPosition())
{
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

BDSTrajectoryPoint::~BDSTrajectoryPoint(){
}

void BDSTrajectoryPoint::printData(){
  G4cout << "BDSTrajectoryPoint> printData" << G4endl;
  if(_currentProcess){
    G4cout << "_currentProcess = " << _currentProcess->GetProcessName() << G4endl;
    G4cout << "_isScatteringProcess = " << _isScatteringProcess << G4endl;
  }
}
