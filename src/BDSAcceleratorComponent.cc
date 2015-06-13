#include "BDSAcceleratorComponent.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSReadOutGeometry.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4VReadOutGeometry.hh"
#include "G4VSolid.hh"

#include <cmath>

G4Material* BDSAcceleratorComponent::emptyMaterial = NULL;
G4double    BDSAcceleratorComponent::lengthSafety  = -1;

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
  beamPipeInfo(beamPipeInfoIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  readOutLV = NULL;
  itsSPos   = 0.0;

  // initialise static members
  if (!emptyMaterial)
    {emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());}
  if (lengthSafety < 0)
    {lengthSafety = BDSGlobalConstants::Instance()->GetLengthSafety();}
  
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
  readOutLV = BuildReadOutVolume(name, chordLength, angle);
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

G4LogicalVolume* BDSAcceleratorComponent::BuildReadOutVolume(G4String name,
							     G4double chordLength,
							     G4double angle)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  if (!BDS::IsFinite(chordLength)) return NULL;

  G4double roRadius      = BDSGlobalConstants::Instance()->GetSamplerDiameter()*0.5;
  G4Material* roMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());
  G4VSolid* roSolid      = NULL;
  if (!BDS::IsFinite(angle))
    {
      //angle is zero - build a box
      roSolid = new G4Box(name + "_ro_solid", // name
			  roRadius,           // x half width
			  roRadius,           // y half width
			  chordLength*0.5);     // z half width
    }
  else
    {
      // angle is finite!
      G4int orientation = BDS::CalculateOrientation(angle);
      G4double in_z     = cos(0.5*fabs(angle*0.5)); 
      G4double in_x     = sin(0.5*fabs(angle*0.5));
      G4ThreeVector inputface  = G4ThreeVector(-orientation*in_x, 0.0, -1.0*in_z);
      //-1 as pointing down in z for normal
      G4ThreeVector outputface = G4ThreeVector(-orientation*in_x, 0.0, in_z);

      roSolid = new G4CutTubs(name + "_ro_solid", // name
			      0,                  // inner radius
			      roRadius,           // outer radius
			      chordLength*0.5,         // half length (z)
			      0,                  // rotation start angle
			      CLHEP::twopi,       // rotation sweep angle
			      inputface,          // input face normal vector
			      outputface);        // output face normal vector
    }

  // note material not strictly necessary in geant4 > v10, but required for
  // v9 even though not used and doesn't affect simulation - leave for compatability
  G4LogicalVolume* readOutLV =  new G4LogicalVolume(roSolid,          // solid
						    roMaterial,       // material
						    name + "_ro_lv"); // name

  return readOutLV;
}
