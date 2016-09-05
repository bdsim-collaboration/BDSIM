#include "BDSStep.hh"


BDSStep::BDSStep():
  preStepPoint(G4ThreeVector()),
  postStepPoint(G4ThreeVector()),
  volumeForTransform(nullptr)
{;}

BDSStep::BDSStep(G4ThreeVector preStepPointIn,
		 G4ThreeVector postStepPointIn,
		 G4VPhysicalVolume* volumeForTransformIn):
  preStepPoint(preStepPointIn),
  postStepPoint(postStepPointIn),
  volumeForTransform(volumeForTransformIn)
{;}

BDSStep::~BDSStep()
{;}
