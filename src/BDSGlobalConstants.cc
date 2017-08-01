#include <cstdlib>

#include "BDSGlobalConstants.hh"

#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSOutputType.hh"
#include "BDSParser.hh"
#include "BDSTunnelInfo.hh"

#include "G4Colour.hh"
#include "G4FieldManager.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4UniformMagField.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/EulerAngles.h"

BDSGlobalConstants* BDSGlobalConstants::instance = nullptr;

BDSGlobalConstants* BDSGlobalConstants::Instance()
{
  if(instance == nullptr)
    {instance = new BDSGlobalConstants(BDSParser::Instance()->GetOptions());}
  return instance;
}

BDSGlobalConstants::BDSGlobalConstants(const GMAD::Options& opt):
  options(opt),
  beamParticleDefinition(nullptr),
  beamMomentum(0.0),
  beamKineticEnergy(0.0),
  particleMomentum(0.0),
  particleKineticEnergy(0.0),
  brho(0.0),
  turnsTaken(0)
{
  outputType = BDS::DetermineOutputType(options.outputFormat);

  particleName = G4String(options.particleName);

  numberToGenerate = G4int(options.nGenerate);

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
      G4cerr << __METHOD_NAME__ << "Error: option \"outerDiameter\" " << outerDi
	     << " must be greater than 2x (\"aper1\" + \"beamPipeThickness\") ("
	     << defaultBeamPipeModel->aper1 << " + " << defaultBeamPipeModel->beamPipeThickness << ")" << G4endl;
      exit(1);
    }
  magnetGeometryType = BDS::DetermineMagnetGeometryType(options.magnetGeometryType);

  // tunnel
  tunnelInfo = new BDSTunnelInfo(options.tunnelType,
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
  
  // defaults - parameters of the laserwire process
  itsLaserwireWavelength = 0.532 * CLHEP::micrometer;
  itsLaserwireDir = G4ThreeVector(1,0,0);
  itsLaserwireTrackPhotons = true;
  itsLaserwireTrackElectrons = true;
  
  zeroMagField = new G4UniformMagField(G4ThreeVector());
  zeroFieldManager=new G4FieldManager();
  zeroFieldManager->SetDetectorField(zeroMagField);
  zeroFieldManager->CreateChordFinder(zeroMagField);

  cOverGeV = CLHEP::c_light /CLHEP::GeV;

  // initialise the default vis attributes and user limits that
  // can be copied by various bits of geometry
  InitVisAttributes();
  InitDefaultUserLimits();

  integratorSet = BDS::DetermineIntegratorSetType(options.integratorSet);

  InitialiseBeamlineTransform();
}

void BDSGlobalConstants::InitialiseBeamlineTransform()
{  
  G4ThreeVector offset = G4ThreeVector(options.beamlineX * CLHEP::m,
				       options.beamlineY * CLHEP::m,
				       options.beamlineZ * CLHEP::m);
  
  G4RotationMatrix rm;
  if (options.beamlineAxisAngle)
    {
      G4ThreeVector axis = G4ThreeVector(options.beamlineAxisX,
					 options.beamlineAxisY,
					 options.beamlineAxisZ);

      rm = G4RotationMatrix(axis, options.beamlineAngle * CLHEP::rad);
    }
  else
    {
      G4double phi   = options.beamlinePhi   * CLHEP::rad;
      G4double theta = options.beamlineTheta * CLHEP::rad;
      G4double psi   = options.beamlinePsi   * CLHEP::rad;
      CLHEP::HepEulerAngles ang = CLHEP::HepEulerAngles(phi, theta, psi);
      rm = G4RotationMatrix(ang);
    }

  beamlineTransform = G4Transform3D(rm, offset);
}

void BDSGlobalConstants::InitVisAttributes()
{
  //for vacuum volumes
  invisibleVisAttr = new G4VisAttributes(G4Colour::Black());
  invisibleVisAttr->SetVisibility(false);
  invisibleVisAttr->SetForceLineSegmentsPerCircle(options.nSegmentsPerCircle);

  //for normally invisible volumes like marker / container volumes in debug mode
  visibleDebugVisAttr = new G4VisAttributes(); //green
  visibleDebugVisAttr->SetColour(0,0.6,0,0.1);
  visibleDebugVisAttr->SetVisibility(true);
  visibleDebugVisAttr->SetForceLineSegmentsPerCircle(options.nSegmentsPerCircle);
}

void BDSGlobalConstants::InitDefaultUserLimits()
{
  defaultUserLimits = new G4UserLimits("default_cuts");
  const G4double maxTime = MaxTime();
  if (maxTime > 0)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Setting maximum tracking time to " << maxTime << " ns" << G4endl;
#endif
      defaultUserLimits->SetUserMaxTime(maxTime);
    }
  defaultUserLimits->SetMaxAllowedStep(MaxStepLength());
  defaultUserLimits->SetUserMaxTrackLength(MaxTrackLength());
}

G4int BDSGlobalConstants::PrintModulo()const
{
  G4int nGenerate = NGenerate();
  G4double fraction = PrintModuloFraction();
  G4int printModulo = (G4int)ceil(nGenerate * fraction);
  if (printModulo < 0)
    {printModulo = 1;}
  return printModulo;
}

BDSGlobalConstants::~BDSGlobalConstants()
{  
  delete defaultBeamPipeModel;
  delete zeroFieldManager;
  delete zeroMagField;
  delete tunnelInfo;
  delete defaultUserLimits;
  delete invisibleVisAttr;
  delete visibleDebugVisAttr;

  instance = nullptr;
}
