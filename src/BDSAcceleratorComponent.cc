#include "BDSAcceleratorComponent.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSReadOutGeometry.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4UserLimits.hh"
#include "G4VReadOutGeometry.hh"
#include "G4VSolid.hh"

#include <cmath>

G4Material* BDSAcceleratorComponent::emptyMaterial = nullptr;
G4double    BDSAcceleratorComponent::lengthSafety  = -1;
G4bool      BDSAcceleratorComponent::checkOverlaps = false;

class BDSBeamPipeInfo;

G4double const BDSAcceleratorComponent::lengthSafetyLarge = 1*CLHEP::um;

BDSAcceleratorComponent::BDSAcceleratorComponent(G4String         nameIn,
						 G4double         arcLengthIn,
						 G4double         angleIn,
						 G4String         typeIn,
						 G4bool           precisionRegionIn,
						 BDSBeamPipeInfo* beamPipeInfoIn):
  BDSGeometryComponent(nullptr,nullptr),
  name(nameIn),
  arcLength(arcLengthIn),
  type(typeIn),
  angle(angleIn),
  precisionRegion(precisionRegionIn),
  beamPipeInfo(beamPipeInfoIn),
  readOutLV(nullptr),
  acceleratorVacuumLV(nullptr),
  copyNumber(-1) // -1 initialisation since it will be incremented when placed 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "(" << name << ")" << G4endl;
#endif
  // initialise static members
  if (!emptyMaterial)
    {emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());}
  if (lengthSafety < 0)
    {lengthSafety = BDSGlobalConstants::Instance()->GetLengthSafety();}
  checkOverlaps = BDSGlobalConstants::Instance()->GetCheckOverlaps();
  
  // calculate the chord length if the angle is finite
  if (BDS::IsFinite(angleIn))
    {chordLength = 2.0 * arcLengthIn * sin(0.5*angleIn) / angleIn;}
  else
    {chordLength = arcLengthIn;}
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "angle:        " << angleIn     << G4endl;
  G4cout << __METHOD_NAME__ << "arc length:   " << arcLengthIn << G4endl;
  G4cout << __METHOD_NAME__ << "chord length: " << chordLength << G4endl; 
#endif

  initialised = false;
}

BDSAcceleratorComponent::~BDSAcceleratorComponent()
{
  delete beamPipeInfo;
  delete readOutLV;
}

void BDSAcceleratorComponent::Initialise()
{
  if (initialised)
    {return;} // protect against duplicated initialisation and memory leaks
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  Build();
  readOutLV = BuildReadOutVolume(name, chordLength, angle);
  initialised = true; // record that this component has been initialised
}

void BDSAcceleratorComponent::Build()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BuildContainerLogicalVolume(); // pure virtual provided by derived class

  // set user limits for container
#ifndef NOUSERLIMITS
  if(containerLogicalVolume)
    {
      G4double maxStepFactor=0.5;
      G4UserLimits* containerUserLimits =  new G4UserLimits();
      containerUserLimits->SetMaxAllowedStep(chordLength*maxStepFactor);
      containerLogicalVolume->SetUserLimits(containerUserLimits);
      RegisterUserLimits(containerUserLimits);
    }
#endif

  // visual attributes
  if(containerLogicalVolume)
    {
    if (BDSGlobalConstants::Instance()->VisDebug())
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
  if (!BDS::IsFinite(chordLength)) return nullptr;

  G4double roRadius = 0;
  G4double roRadiusFromSampler = BDSGlobalConstants::Instance()->GetSamplerDiameter()*0.5;
  
  G4VSolid* roSolid = nullptr;
  if (!BDS::IsFinite(angle))
    {
      //angle is zero - build a box
      roRadius = roRadiusFromSampler;
      roSolid = new G4Box(name + "_ro_solid", // name
			  roRadius,           // x half width
			  roRadius,           // y half width
			  chordLength*0.5);   // z half width
    }
  else
    {
      // angle is finite!
      // factor of 0.95 here is arbitrary tolerance as g4 cut tubs seems to fail
      // with cutting entranace / exit planes close to limit.  
      G4double roRadiusFromAngleLength =  std::abs(chordLength / angle) * 0.8; // s = r*theta -> r = s/theta
      roRadius = std::min(roRadiusFromSampler,roRadiusFromAngleLength);
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "taking smaller of: sampler radius: " << roRadiusFromSampler
	     << " mm, max possible radius: " << roRadiusFromAngleLength << " mm" << G4endl;
#endif
      std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(0.5*angle,0.5*angle);
      G4ThreeVector inputface = faces.first;
      G4ThreeVector outputface = faces.second;
      inputface[0] *= -1;
      outputface[0] *= -1;
      //x components have to be multiplied by -1 for some reason. 

      roSolid = new G4CutTubs(name + "_ro_solid", // name
			      0,                  // inner radius
			      roRadius,           // outer radius
			      chordLength*0.5,    // half length (z)
			      0,                  // rotation start angle
			      CLHEP::twopi,       // rotation sweep angle
			      inputface,          // input face normal vector
			      outputface);        // output face normal vector
    }

  // note material not strictly necessary in geant4 > v10, but required for
  // v9 even though not used and doesn't affect simulation - leave for compatability
  G4LogicalVolume* readOutLV =  new G4LogicalVolume(roSolid,          // solid
						    emptyMaterial,    // material
						    name + "_ro_lv"); // name

  return readOutLV;
}

