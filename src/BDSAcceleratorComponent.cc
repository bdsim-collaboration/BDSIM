#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSFieldInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSTunnelInfo.hh"
#include "BDSUtilities.hh"

#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4UserLimits.hh"

#include <cmath>

G4Material* BDSAcceleratorComponent::emptyMaterial = nullptr;
G4double    BDSAcceleratorComponent::lengthSafety  = -1;
G4bool      BDSAcceleratorComponent::checkOverlaps = false;

G4double const BDSAcceleratorComponent::lengthSafetyLarge = 1*CLHEP::um;

BDSAcceleratorComponent::BDSAcceleratorComponent(G4String         nameIn,
						 G4double         arcLengthIn,
						 G4double         angleIn,
						 G4String         typeIn,
						 BDSBeamPipeInfo* beamPipeInfoIn,
						 G4ThreeVector    inputFaceNormalIn,
						 G4ThreeVector    outputFaceNormalIn,
						 BDSFieldInfo*    fieldInfoIn):
  BDSGeometryComponent(nullptr,nullptr),
  name(nameIn),
  arcLength(arcLengthIn),
  type(typeIn),
  angle(angleIn),
  beamPipeInfo(beamPipeInfoIn),
  acceleratorVacuumLV(nullptr),
  endPieceBefore(nullptr),
  endPieceAfter(nullptr),
  copyNumber(-1), // -1 initialisation since it will be incremented when placed
  inputFaceNormal(inputFaceNormalIn),
  outputFaceNormal(outputFaceNormalIn),
  fieldInfo(fieldInfoIn),
  readOutRadius(0)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "(" << name << ")" << G4endl;
#endif
  // initialise static members
  const auto globals = BDSGlobalConstants::Instance(); // shortcut
  if (!emptyMaterial)
    {emptyMaterial = BDSMaterials::Instance()->GetMaterial(globals->EmptyMaterial());}
  if (lengthSafety < 0)
    {lengthSafety = globals->LengthSafety();}
  checkOverlaps = globals->CheckOverlaps();

  // Prevent negative length components.
  if (arcLength < 0)
    {
      G4cerr << __METHOD_NAME__ << "Negative length for component named \""
	     << name << "\" with length " << arcLength << G4endl;
      exit(1);
    }
  
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
}

void BDSAcceleratorComponent::Initialise()
{
  if (initialised)
    {return;} // protect against duplicated initialisation and memory leaks
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  Build();
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
    {containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());}
}

void BDSAcceleratorComponent::SetField(BDSFieldInfo* fieldInfoIn)
{
  delete fieldInfo; // clear up existing definition if there is one
  fieldInfo = fieldInfoIn;
}
