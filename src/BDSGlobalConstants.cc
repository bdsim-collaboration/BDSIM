#include <cstdlib>

#include "BDSGlobalConstants.hh"

#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSParser.hh"
#include "BDSTunnelInfo.hh"

#include "G4Colour.hh"
#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include "G4ThreeVector.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

BDSGlobalConstants* BDSGlobalConstants::_instance = nullptr;

BDSGlobalConstants* BDSGlobalConstants::Instance()
{
  if(_instance==nullptr)
    {_instance = new BDSGlobalConstants(BDSParser::Instance()->GetOptions());}
  return _instance;
}

BDSGlobalConstants::BDSGlobalConstants(const GMAD::Options& opt):
  itsBeamParticleDefinition(nullptr),
  itsBeamMomentum(0.0),
  itsBeamKineticEnergy(0.0),
  itsParticleMomentum(0.0),
  itsParticleKineticEnergy(0.0),
  itsSMax(0.0)
{
  printModuloFraction   = opt.printModuloFraction;
  itsPhysListName       = opt.physicsList;
  itsVacuumMaterial     = opt.vacMaterial;
  itsEmptyMaterial      = "G4_Galactic"; // space vacuum

  itsSampleDistRandomly = true;
  
  itsSensitiveComponents=opt.sensitiveBeamlineComponents;
  itsSensitiveBeamPipe=opt.sensitiveBeamPipe;
  itsSensitiveBLMs=opt.sensitiveBLMs;
  itsDefaultRangeCut=opt.defaultRangeCut * CLHEP::m;
  itsElossHistoBinWidth=opt.elossHistoBinWidth; //Longitudinal and transverse energy loss histogram bin widths
  itsElossHistoTransBinWidth=opt.elossHistoTransBinWidth;
  itsFFact=opt.ffact;
  itsParticleName = G4String(opt.particleName);
  itsBeamTotalEnergy = opt.beamEnergy * CLHEP::GeV;
  if (itsBeamTotalEnergy == 0)
    {
      G4cerr << __METHOD_NAME__ << "Error: option \"beamenergy\" is not defined or must be greater than 0" <<  G4endl;
      exit(1);
    }
  itsParticleTotalEnergy = opt.E0 * CLHEP::GeV; 
  if (itsParticleTotalEnergy == 0)
    {itsParticleTotalEnergy = itsBeamTotalEnergy;}

  itsPlanckScatterFe = opt.planckScatterFe;
  //Fraction of events with leading particle biasing.

  //beampipe
  defaultBeamPipeModel = new BDSBeamPipeInfo(opt.apertureType,
					     opt.aper1 * CLHEP::m,
					     opt.aper2 * CLHEP::m,
					     opt.aper3 * CLHEP::m,
					     opt.aper4 * CLHEP::m,
					     opt.vacMaterial,
					     opt.beampipeThickness * CLHEP::m,
					     opt.beampipeMaterial);
  
  // magnet geometry
  itsOuterDiameter = opt.outerDiameter * CLHEP::m;
  if (itsOuterDiameter < 2*(defaultBeamPipeModel->beamPipeThickness + defaultBeamPipeModel->aper1))
    {
      G4cerr << __METHOD_NAME__ << "Error: option \"outerDiameter\" must be greater than 2x (\"aper1\" + \"beamPipeThickness\") " << G4endl;
      exit(1);
    }
  itsMagnetGeometryType = BDS::DetermineMagnetGeometryType(opt.magnetGeometryType);
  itsOuterMaterialName  = opt.outerMaterialName;

  dontSplitSBends       = opt.dontSplitSBends;

  // tunnel
  buildTunnel            = opt.buildTunnel;
  buildTunnelStraight    = opt.buildTunnelStraight;
  tunnelInfo             = new BDSTunnelInfo(opt.tunnelType,
					     opt.tunnelThickness     * CLHEP::m,
					     opt.tunnelSoilThickness * CLHEP::m,
					     opt.tunnelMaterial,
					     opt.soilMaterial,
					     opt.buildTunnelFloor,
					     opt.tunnelFloorOffset   * CLHEP::m,
					     opt.tunnelAper1         * CLHEP::m,
					     opt.tunnelAper2         * CLHEP::m,
					     opt.tunnelSensitive,
					     opt.tunnelVisible);
  tunnelOffsetX          = opt.tunnelOffsetX * CLHEP::m;
  tunnelOffsetY          = opt.tunnelOffsetY * CLHEP::m;

  // beam loss monitor (BLM) geometry
  itsBlmRad              = opt.blmRad              * CLHEP::m;
  itsBlmLength           = opt.blmLength           * CLHEP::m;

  // samplers
  itsSamplerDiameter     = opt.samplerDiameter     * CLHEP::m;
  itsSamplerLength       = 4E-8                    * CLHEP::m;

  // production thresholds
  itsThresholdCutCharged = opt.thresholdCutCharged * CLHEP::GeV;
  itsThresholdCutPhotons = opt.thresholdCutPhotons * CLHEP::GeV;
  itsProdCutPhotons      = opt.prodCutPhotons      * CLHEP::m;
  itsProdCutPhotonsP     = opt.prodCutPhotonsP     * CLHEP::m;
  itsProdCutPhotonsA     = opt.prodCutPhotonsA     * CLHEP::m;
  itsProdCutElectrons    = opt.prodCutElectrons    * CLHEP::m;
  itsProdCutElectronsP   = opt.prodCutElectronsP   * CLHEP::m;
  itsProdCutElectronsA   = opt.prodCutElectronsA   * CLHEP::m;
  itsProdCutPositrons    = opt.prodCutPositrons    * CLHEP::m;
  itsProdCutPositronsP   = opt.prodCutPositronsP   * CLHEP::m;
  itsProdCutPositronsA   = opt.prodCutPositronsA   * CLHEP::m;
  itsProdCutProtons      = opt.prodCutProtons      * CLHEP::m;
  itsProdCutProtonsP     = opt.prodCutProtonsP     * CLHEP::m;
  itsProdCutProtonsA     = opt.prodCutProtonsA     * CLHEP::m;
  
  // tracking accuracy
  itsDeltaChord          = opt.deltaChord          * CLHEP::m;
  itsChordStepMinimum    = opt.chordStepMinimum    * CLHEP::m;
  itsDeltaIntersection   = opt.deltaIntersection   * CLHEP::m;
  itsMinimumEpsilonStep  = opt.minimumEpsilonStep;
  itsMaximumEpsilonStep  = opt.maximumEpsilonStep;
  itsMaxTime             = opt.maximumTrackingTime * CLHEP::s;
  itsDeltaOneStep        = opt.deltaOneStep        * CLHEP::m;
  
  itsDoPlanckScattering = opt.doPlanckScattering;
  itsCheckOverlaps = opt.checkOverlaps;
  itsTurnOnCerenkov = opt.turnOnCerenkov;
  itsTurnOnOpticalAbsorption = opt.turnOnOpticalAbsorption;
  itsTurnOnRayleighScattering = opt.turnOnRayleighScattering;
  itsTurnOnMieScattering = opt.turnOnMieScattering;
  itsTurnOnOpticalSurface = opt.turnOnOpticalSurface;
  itsTurnOnBirksSaturation = opt.turnOnBirksSaturation;
  itsScintYieldFactor=opt.scintYieldFactor;
  if (opt.lengthSafety < 1e-15)
    { // protect against poor lengthSafety choices that would cause potential overlaps
      G4cerr << "Dangerously low \"lengthSafety\" value of: " << opt.lengthSafety
	     << " m that will result in potential geometry overlaps!" << G4endl;
      G4cerr << "This affects all geometry construction and should be carefully chosen!!!" << G4endl;
      G4cerr << "The default value is 1 pm" << G4endl;
      exit(1);
    }
  else
    {itsLengthSafety = opt.lengthSafety * CLHEP::m;}

  // set the number of primaries to generate - exec options overrides whatever's in gmad
  G4int nToGenerate = BDSExecOptions::Instance()->GetNGenerate();
  if (nToGenerate < 0)
    {itsNumberToGenerate = opt.numberToGenerate;}
  else
    {itsNumberToGenerate = nToGenerate;}
  
  itsNumberOfEventsPerNtuple = opt.numberOfEventsPerNtuple;
  itsEventNumberOffset = opt.eventNumberOffset;
  itsRandomSeed = opt.randomSeed;
  itsUseEMLPB=opt.useEMLPB;
  itsUseHadLPB=opt.useHadLPB;
  SetLPBFraction(opt.LPBFraction);
  itsStoreMuonTrajectories = opt.storeMuonTrajectories;
  itsTrajCutGTZ = opt.trajCutGTZ;
  itsTrajCutLTR = opt.trajCutLTR;
  itsStoreNeutronTrajectories = opt.storeNeutronTrajectories;
  itsStoreTrajectory = opt.storeTrajectory;
  //G4cout<<"STOREA TRAJ = "<< itsStoreTrajectory<<G4endl;
  stopSecondaries = opt.stopSecondaries;
  stopTracks = opt.stopTracks; 
  // defaults - parameters of the laserwire process
  itsLaserwireWavelength = 0.532 * CLHEP::micrometer;
  itsLaserwireDir = G4ThreeVector(1,0,0);
  itsLaserwireTrackPhotons = 1;
  itsLaserwireTrackElectrons = 1;
  //itsIncludeIronMagFields = opt.includeIronMagFields;
  itsIncludeIronMagFields = false;
  zeroMagField = new G4UniformMagField(G4ThreeVector());
  itsZeroFieldManager=new G4FieldManager();
  itsZeroFieldManager->SetDetectorField(zeroMagField);
  itsZeroFieldManager->CreateChordFinder(zeroMagField);
  itsTurnsTaken = 0;
  if(opt.nturns < 1)
    itsTurnsToTake = 1;
  else
    itsTurnsToTake = opt.nturns;
  teleporterdelta     = G4ThreeVector(0.,0.,0.);
  teleporterlength    = 0.0;
  
  // options that are never used (no set method):
  itsLWCalWidth       = 0.0;
  itsLWCalOffset      = 0.0;
  itsMagnetPoleRadius = 0.0;
  itsMagnetPoleSize   = 0.0;

  // initialise the default vis attributes and user limits that
  // can be copied by various bits of geometry
  InitVisAttributes();
  InitDefaultUserLimits(); // should be done after itsMaxTime set
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
  defaultUserLimits->SetUserMaxTime(itsMaxTime);
  //user must set step length manually
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

  _instance = nullptr;
}
