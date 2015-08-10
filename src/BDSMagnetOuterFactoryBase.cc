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
  poleSolid          = nullptr;
  yokeSolid          = nullptr;
  containerSolid     = nullptr;
  poleLV             = nullptr;
  yokeLV             = nullptr;
  containerLV        = nullptr;
  yokePV             = nullptr;
  outerVisAttributes = nullptr;
  outerUserLimits    = nullptr;

  allLogicalVolumes.clear();
  allPhysicalVolumes.clear();
  allRotationMatrices.clear();
  allSolids.clear();
  allVisAttributes.clear();
  allUserLimits.clear();
}
