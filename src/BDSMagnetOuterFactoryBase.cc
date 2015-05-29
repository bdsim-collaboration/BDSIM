#include "BDSMagnetOuterFactoryBase.hh"

#include "BDSGlobalConstants.hh"

BDSMagnetOuterFactoryBase::BDSMagnetOuterFactoryBase()
{
  lengthSafety       = BDSGlobalConstants::Instance()->GetLengthSafety();
  checkOverlaps      = BDSGlobalConstants::Instance()->GetCheckOverlaps();
  nSegmentsPerCircle = 50;
  maxStepFactor      = 0.5;

  // initialise variables and pointers that'll be used by the factory
  CleanUp();
}

void BDSMagnetOuterFactoryBase::CleanUp()
{
  poleSolid        = NULL;
  yokeSolid        = NULL;
  containerSolid   = NULL;
  poleLV           = NULL;
  yokeLV           = NULL;
  containerLV      = NULL;
}
