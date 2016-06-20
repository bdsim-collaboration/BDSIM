#include "G4Allocator.hh"
#include "G4ProcessType.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4NavigationHistory.hh"

#include "BDSAuxiliaryNavigator.hh"
#include "BDSDebug.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSTrajectoryPoint.hh"

#include <ostream>

G4Allocator<BDSTrajectoryPoint> bdsTrajectoryPointAllocator;

// Don't use transform caching in the aux navigator as it's used for all over the geometry here.
BDSAuxiliaryNavigator* BDSTrajectoryPoint::auxNavigator = new BDSAuxiliaryNavigator(false);
G4int BDSTrajectoryPoint::numberOfPoints = 0;

BDSTrajectoryPoint::BDSTrajectoryPoint():
  G4TrajectoryPoint(G4ThreeVector())
{
  numberOfPoints++;
  preProcessType     = -1;
  preProcessSubType  = -1;
  postProcessType    = -1;
  postProcessSubType = -1;
  preS               = -1000;
  postS              = -1000;
}

BDSTrajectoryPoint::BDSTrajectoryPoint(const G4Step* step):
  G4TrajectoryPoint(step->GetPostStepPoint()->GetPosition())
{
  numberOfPoints++;
  preProcessType     = -1;
  preProcessSubType  = -1;
  postProcessType    = -1;
  postProcessSubType = -1;
  preS               = -1000;
  postS              = -1000;
  
  const G4StepPoint  *prePoint   = step->GetPreStepPoint();
  const G4StepPoint *postPoint   = step->GetPostStepPoint();
  const G4VProcess   *preProcess = prePoint->GetProcessDefinedStep();
  const G4VProcess  *postProcess = postPoint->GetProcessDefinedStep();

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

  preWeight  = prePoint->GetWeight();
  postWeight = postPoint->GetWeight();
  energy     = step->GetTotalEnergyDeposit();
  preEnergy  = prePoint->GetKineticEnergy();
  postEnergy = postPoint->GetKineticEnergy();

  // s position for pre and post step point
  G4VPhysicalVolume* curvilinearVol = auxNavigator->LocateGlobalPointAndSetup(step);
  BDSPhysicalVolumeInfo* info = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(curvilinearVol);

  if (info)
  {
    G4ThreeVector prePosLocal = auxNavigator->ConvertToLocal(prePoint->GetPosition());
    G4ThreeVector posPosLocal = auxNavigator->ConvertToLocal(postPoint->GetPosition());

    G4double sCentre = info->GetSPos();
    preS = sCentre + prePosLocal.z();
    postS = sCentre + posPosLocal.z();
  }
}

BDSTrajectoryPoint::~BDSTrajectoryPoint()
{
  if (numberOfPoints == 0)
    {delete auxNavigator; auxNavigator = nullptr;}
  numberOfPoints--;
}

std::ostream& operator<< (std::ostream& out, BDSTrajectoryPoint const &p)
{
  out << p.GetPosition();
  return out;
}
