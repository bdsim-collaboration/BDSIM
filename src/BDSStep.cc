#include "BDSStep.hh"


BDSStep::BDSStep():
  preStepPoint(G4ThreeVector()),
  postStepPoint(G4ThreeVector())
{;}

BDSStep::BDSStep(G4ThreeVector preStepPointIn, G4ThreeVector postStepPointIn):
  preStepPoint(preStepPointIn),
  postStepPoint(postStepPointIn)
{;}

BDSStep::~BDSStep()
{;}
