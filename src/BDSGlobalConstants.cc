#include <cstdlib>

#include "BDSGlobalConstants.hh"

#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSOutputFormat.hh"
#include "BDSParser.hh"
#include "BDSTunnelInfo.hh"
#include "BDSUtilities.hh"

#include "G4Colour.hh"
#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include "G4ThreeVector.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

BDSGlobalConstants* BDSGlobalConstants::instance = nullptr;

BDSGlobalConstants* BDSGlobalConstants::Instance()
{
  if(instance == nullptr)
    {instance = new BDSGlobalConstants(BDSParser::Instance()->GetOptions());}
  return instance;
}

BDSGlobalConstants::BDSGlobalConstants(const GMAD::Options& opt):
  options(GMAD::Options(opt)),
  itsBeamParticleDefinition(nullptr),
  itsBeamMomentum(0.0),
  itsBeamKineticEnergy(0.0),
  itsParticleMomentum(0.0),
  itsParticleKineticEnergy(0.0),
  itsSMax(0.0),
  itsTurnsTaken(0.0),
  teleporterlength(0.0)
{
  outputFormat = BDS::DetermineOutputFormat(options.outputFormat);
  if (options.nGenerate < 0) // run at least 1 event!
    {options.set_value("nGenerate", 1);}

  if (options.beamEnergy == 0)
    {
      G4cerr << __METHOD_NAME__ << "Error: option \"beamenergy\" is not defined or must be greater than 0" <<  G4endl;
      exit(1);
    }

  if (!BDS::IsFinite(options.E0))
    {options.set_value("E0", options.beamEnergy);}

  //beampipe
  defaultBeamPipeModel = new BDSBeamPipeInfo(options.apertureType,
					     options.aper1 * CLHEP::m,
					     options.aper2 * CLHEP::m,
					     options.aper3 * CLHEP::m,
					     options.aper4 * CLHEP::m,
					     options.vacMaterial,
					     options.beampipeThickness * CLHEP::m,
					     options.beampipeMaterial);
  
  // magnet geometry
  G4double outerDi = options.outerDiameter * CLHEP::m;
  if (outerDi < 2*(defaultBeamPipeModel->beamPipeThickness + defaultBeamPipeModel->aper1))
    {
      G4cerr << __METHOD_NAME__ << "Error: option \"outerDiameter\" must be greater than 2x (\"aper1\" + \"beamPipeThickness\") " << G4endl;
      exit(1);
    }
  itsMagnetGeometryType = BDS::DetermineMagnetGeometryType(options.magnetGeometryType);

  // tunnel
  tunnelInfo             = new BDSTunnelInfo(options.tunnelType,
					     options.tunnelThickness     * CLHEP::m,
					     options.tunnelSoilThickness * CLHEP::m,
					     options.tunnelMaterial,
					     options.soilMaterial,
					     options.buildTunnelFloor,
					     options.tunnelFloorOffset   * CLHEP::m,
					     options.tunnelAper1         * CLHEP::m,
					     options.tunnelAper2         * CLHEP::m,
					     options.tunnelSensitive,
					     options.tunnelVisible);
  
  if (options.lengthSafety < 1e-15)
    { // protect against poor lengthSafety choices that would cause potential overlaps
      G4cerr << "Dangerously low \"lengthSafety\" value of: " << options.lengthSafety
	     << " m that will result in potential geometry overlaps!" << G4endl;
      G4cerr << "This affects all geometry construction and should be carefully chosen!!!" << G4endl;
      G4cerr << "The default value is 1 pm" << G4endl;
      exit(1);
    }
  else
    {itsLengthSafety = options.lengthSafety * CLHEP::m;}

  itsLPBFraction = options.LPBFraction;
  if(itsLPBFraction > 1.0) // safety checks
    {itsLPBFraction = 1.0;}
  if(itsLPBFraction < 0.0)
    {itsLPBFraction = 0.0;}
  
  // defaults - parameters of the laserwire process
  itsLaserwireWavelength = 0.532 * CLHEP::micrometer;
  itsLaserwireDir = G4ThreeVector(1,0,0);
  itsLaserwireTrackPhotons = 1;
  itsLaserwireTrackElectrons = 1;
  
  zeroMagField = new G4UniformMagField(G4ThreeVector());
  itsZeroFieldManager=new G4FieldManager();
  itsZeroFieldManager->SetDetectorField(zeroMagField);
  itsZeroFieldManager->CreateChordFinder(zeroMagField);
  
  itsTurnsToTake = options.nturns;
  if(itsTurnsToTake < 1)
    {itsTurnsToTake = 1;}
  
  teleporterdelta     = G4ThreeVector(0.,0.,0.);

  InitRotationMatrices();
  
  // options that are never used (no set method):
  // refactor out of classes that use this
  itsLWCalWidth       = 0.0;
  itsLWCalOffset      = 0.0;
  itsMagnetPoleRadius = 0.0;
  itsMagnetPoleSize   = 0.0;

  // initialise the default vis attributes and user limits that
  // can be copied by various bits of geometry
  InitVisAttributes();
  InitDefaultUserLimits();
}

void BDSGlobalConstants::InitVisAttributes()
{
  //for vacuum volumes
  invisibleVisAttr = new G4VisAttributes(G4Colour::Black());
  invisibleVisAttr->SetVisibility(false);
  invisibleVisAttr->SetForceLineSegmentsPerCircle(50);

  //for normally invisible volumes like marker / container volumes in debug mode
  visibleDebugVisAttr = new G4VisAttributes(); //green
  visibleDebugVisAttr->SetColour(0,0.6,0,0.1);
  visibleDebugVisAttr->SetVisibility(true);
  visibleDebugVisAttr->SetForceLineSegmentsPerCircle(50);
}

void BDSGlobalConstants::InitDefaultUserLimits()
{
  //these must be copied and not attached directly
  defaultUserLimits = new G4UserLimits("default_cuts");
  defaultUserLimits->SetUserMaxTime(GetMaxTime());
  //user must set step length manually
}

void BDSGlobalConstants::InitRotationMatrices()
{
  _RotY90       = new G4RotationMatrix();
  _RotYM90      = new G4RotationMatrix();
  _RotX90       = new G4RotationMatrix();
  _RotXM90      = new G4RotationMatrix();
  _RotYM90X90   = new G4RotationMatrix();
  _RotYM90XM90  = new G4RotationMatrix();
  G4double pi_ov_2 = asin(1.);
  _RotY90->rotateY(pi_ov_2);
  _RotYM90->rotateY(-pi_ov_2);
  _RotX90->rotateX(pi_ov_2);
  _RotXM90->rotateX(-pi_ov_2);
  _RotYM90X90->rotateY(-pi_ov_2);
  _RotYM90X90->rotateX( pi_ov_2);
  _RotYM90XM90->rotateY(-pi_ov_2);
  _RotYM90XM90->rotateX(-pi_ov_2);
}

BDSGlobalConstants::~BDSGlobalConstants()
{  
  delete defaultBeamPipeModel;
  delete itsZeroFieldManager;
  delete zeroMagField;
  delete tunnelInfo;
  delete defaultUserLimits;
  delete invisibleVisAttr;
  delete visibleDebugVisAttr;

  delete _RotY90;
  delete _RotYM90;
  delete _RotX90;
  delete _RotXM90;
  delete _RotYM90X90;
  delete _RotYM90XM90;

  instance = nullptr;
}
