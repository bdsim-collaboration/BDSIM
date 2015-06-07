#include <list>
#include <sstream>
#include <cmath>

#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSUtilities.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSGeometryComponent.hh"
#include "BDSMaterials.hh"
#include "BDSReadOutGeometry.hh"
#include "BDSTiltOffset.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4AssemblyVolume.hh"
#include "G4Transform3D.hh"

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
  Build();
  readOutLV = BDS::BuildReadOutVolume(name, chordLength, angle);
}

void BDSAcceleratorComponent::Build()
{
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
