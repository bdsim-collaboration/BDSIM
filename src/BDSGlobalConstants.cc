#include <cstdlib>

#include "BDSGlobalConstants.hh"

#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSOutputFormat.hh"
#include "BDSParser.hh"
#include "BDSTunnelInfo.hh"

#include "G4Colour.hh"
#include "G4FieldManager.hh"
#include "G4ThreeVector.hh"
#include "G4UniformMagField.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include "CLHEP/Units/PhysicalConstants.h"

BDSGlobalConstants* BDSGlobalConstants::instance = nullptr;

BDSGlobalConstants* BDSGlobalConstants::Instance()
{
  if(instance == nullptr)
    {instance = new BDSGlobalConstants(BDSParser::Instance()->GetOptions());}
  return instance;
}

BDSGlobalConstants::BDSGlobalConstants(const GMAD::Options& opt):
  options(GMAD::Options(opt)),
  beamParticleDefinition(nullptr),
  beamMomentum(0.0),
  beamKineticEnergy(0.0),
  particleMomentum(0.0),
  particleKineticEnergy(0.0),
  brho(0.0),
  sMax(0.0),
  turnsTaken(0)
{
  outputFormat = BDS::DetermineOutputFormat(options.outputFormat);

  particleName = G4String(options.particleName);
  
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
      G4cerr << __METHOD_NAME__ << "Error: option \"outerDiameter\" " << outerDi << " must be greater than 2x (\"aper1\" + \"beamPipeThickness\") (" << defaultBeamPipeModel->aper1 << " + " << defaultBeamPipeModel->beamPipeThickness << ")" << G4endl;
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

  CalculateHistogramParameters();
  
  // initialise the default vis attributes and user limits that
  // can be copied by various bits of geometry
  InitVisAttributes();
  InitDefaultUserLimits();

  integratorSet = BDS::DetermineIntegratorSetType(options.integratorSet);
}

void BDSGlobalConstants::CalculateHistogramParameters()
{
  // rounding up so last bin definitely covers smax
  // (max - min) / bin width -> min = 0 here.
  const G4double binWidth = ElossHistoBinWidth();
  nBins = (int) ceil(SMax() / binWidth); 
  sMaxHistograms = nBins * binWidth; // round up to integer # of bins
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
      defaultUserLimits->SetUserMaxTime(MaxTime());
    }
  defaultUserLimits->SetMaxAllowedStep(MaxStepLength());
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
