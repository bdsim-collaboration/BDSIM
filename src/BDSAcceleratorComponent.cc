#include "BDSAcceleratorComponent.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSReadOutGeometry.hh"
#include "BDSUtilities.hh"

#include <cmath>


struct BDSBeamPipeInfo;
class BDSTiltOffset;

BDSAcceleratorComponent::BDSAcceleratorComponent(G4String         nameIn,
						 G4double         arcLengthIn,
						 G4double         angleIn,
						 G4String         typeIn,
						 BDSTiltOffset    tiltOffsetIn,
						 G4int            precisionRegionIn,
						 BDSBeamPipeInfo* beamPipeInfoIn):
  BDSGeometryComponent(NULL,NULL),
  name(nameIn),
  arcLength(arcLengthIn),
  type(typeIn),
  angle(angleIn),
  tiltOffset(tiltOffsetIn),
  precisionRegion(precisionRegionIn),
  beamPipeInfo(beamPipeInfoIn),
  lengthSafety(BDSGlobalConstants::Instance()->GetLengthSafety())

{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  readOutLV = NULL;
  itsSPos   = 0.0;
  emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());
  
  // calculate the chord length if the angle is finite
  if (BDS::IsFinite(angleIn))
    {chordLength = 2.0 * arcLengthIn * sin(0.5*angleIn) / angleIn;}
  else
    {chordLength = arcLengthIn;}
}

BDSAcceleratorComponent::~BDSAcceleratorComponent()
{
  delete beamPipeInfo;
  delete readOutLV;
}

void BDSAcceleratorComponent::Initialise()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  Build();
  readOutLV = BDS::BuildReadOutVolume(name, chordLength, angle);
}

void BDSAcceleratorComponent::Build()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BuildContainerLogicalVolume(); // pure virtual provided by derived class

  // visual attributes
  if(containerLogicalVolume)
    {
    if (BDSExecOptions::Instance()->GetVisDebug())
      {containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());}
    else
      {containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());}
    }
}

void BDSAcceleratorComponent::PrepareField(G4VPhysicalVolume*)
{//do nothing by default
  return;
}
