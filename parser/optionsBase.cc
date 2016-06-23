#include "options.h"

// include git commit version.
#include "version.h"

#include <iostream>

using namespace GMAD;

OptionsBase::OptionsBase()
{
  // Default Values for Options

  gitVersion = std::string(GIT_VERSION);

  // executable options
  inputFileName         = "optics.mad";
  visMacroFileName      = "";
  visDebug              = false;
  outputFileName        = "output";
  outputFormat          = "ascii";
  survey                = false;
  surveyFileName        = "survey.dat";
  gflash                = false;
  gflashemax            = 10000;
  gflashemin            = 0.1;
  verbose               = false;
  verboseEvent          = false;
  verboseStep           = false;
  verboseEventNumber    = -1;
  batch                 = false; 
  verboseRunLevel       = 0;
  verboseEventLevel     = 0;
  verboseTrackingLevel  = 0;
  verboseSteppingLevel  = 0;
  circular              = false;
  seed                  = -1;
  seedStateFileName     = "";
  setSeedState          = false;
  nGenerate             = 1;
  generatePrimariesOnly = false;
  exportGeometry        = false;
  exportType            = "gdml";
  exportFileName        = "geometry";
  bdsimPath             = "";
  
  // very important options
  physicsList           = ""; //default - only transportation
  modularPhysicsListsOn = 1;
  randomSeed            = -1;
  
  // beam options
  particleName          = "";
  distribType           = "reference";
  xDistribType          = "";
  yDistribType          = "";
  zDistribType          = "";
  distribFile           = "";
  distribFileFormat     = "";
  haloPSWeightParameter = 1.0;
  haloPSWeightFunction  = "";
  nlinesIgnore          = 0;
  ffact                 = 1.0;
  beamEnergy            = 0.0;

  X0 = 0.0, Y0 = 0.0, Z0 = 0.0, S0 = 0.0;
  Xp0 = 0.0, Yp0 = 0.0, Zp0 = 0.0;
  T0 = 0.0;
  E0 = 0.0;
  sigmaT = 0.0;
  betx = 0.0, bety = 0.0, alfx = 0.0, alfy = 0.0, emitx = 0.0, emity = 0.0;
  dispx = 0.0, dispy = 0.0, dispxp = 0.0, dispyp = 0.0;
  sigmaX = 0.0, sigmaXp = 0.0, sigmaY = 0.0, sigmaYp = 0.0;
  envelopeX = 0.0, envelopeXp = 0.0, envelopeY = 0.0, envelopeYp = 0.0, envelopeT = 0.0, envelopeE = 0.0;
  envelopeR = 0.0, envelopeRp = 0.0;
  sigma11 = 0.0,sigma12 = 0.0,sigma13 = 0.0,sigma14 = 0.0,sigma15 = 0.0,sigma16 = 0.0;
  sigma22 = 0.0,sigma23 = 0.0,sigma24 = 0.0,sigma25 = 0.0,sigma26 = 0.0;
  sigma33 = 0.0,sigma34 = 0.0,sigma35 = 0.0,sigma36 = 0.0;
  sigma44 = 0.0,sigma45 = 0.0,sigma46 = 0.0;
  sigma55 = 0.0,sigma56 = 0.0;
  sigma66 = 0.0;
  shellX=0.0, shellXp=0.0, shellY=0.0, shellYp=0.0;
  shellXWidth=0.0, shellXpWidth=0.0, shellYWidth=0.0, shellYpWidth=0.0;
  Rmin=0.0, Rmax=0.0;
  sigmaE=0.0;

  eventNumberOffset       = 0;

  // general geometrical prameters
  checkOverlaps           = 0;
  xsize=0.0, ysize=0.0;

  // magnet geometry
  magnetGeometryType   = "cylindrical";
  outerMaterialName    = "iron";
  outerDiameter        = 0.6;

  // geometry debugging
  // always split sbends into smaller chunks by default
  dontSplitSBends      = false; 

  includeIronMagFields = false;
  sensitiveBeamlineComponents = 1;

  // beam pipe / aperture
  beampipeThickness    = 0.0025;
  apertureType         = "circular";
  aper1                = 0.025; // also beampipeRadius
  aper2                = 0.0;
  aper3                = 0.0;
  aper4                = 0.0;
  beampipeMaterial     = "StainlessSteel";
  vacMaterial          = "Vacuum";
  emptyMaterial        = "G4_Galactic";
  vacuumPressure       = 1e-12;
  sensitiveBeamPipe    = 1;

  // tunnel options
  buildTunnel         = false;
  buildTunnelStraight = false;
  tunnelType          = "circular";
  tunnelThickness     = 0.1;
  tunnelSoilThickness = 1.0;
  tunnelMaterial      = "concrete";
  soilMaterial        = "soil";
  buildTunnelFloor    = true;
  tunnelFloorOffset   = 1.0; // m
  tunnelAper1         = 2.0; // m
  tunnelAper2         = 2.0; // m
  tunnelSensitive     = true;
  tunnelVisible       = true;
  tunnelOffsetX       = 0;
  tunnelOffsetY       = 0;

  // samplers
  samplerDiameter     = 5; // m

  // beam loss monitors geometry
  blmRad                   = 0.05;
  blmLength                = 0.18;
  sensitiveBLMs            = 1;

  // physics processes
  useEMLPB                 = 0;
  useHadLPB                = 0;
  doPlanckScattering       = 0;
  planckScatterFe          = 1.0;
  turnOnCerenkov           = 1;
  turnOnOpticalAbsorption  = 1;
  turnOnMieScattering      = 1;
  turnOnRayleighScattering = 1;
  turnOnOpticalSurface     = 1;
  turnOnBirksSaturation    = 1;
  scintYieldFactor         = 1.0;
  LPBFraction              = 0.0;
  thresholdCutCharged      = 0.0;
  thresholdCutPhotons      = 0.0;
  defaultRangeCut          = 1e-3;
  prodCutPhotons           = 1e-3;
  prodCutPhotonsP          = 1e-3;
  prodCutPhotonsA          = 1e-3;
  prodCutElectrons         = 1e-3;
  prodCutElectronsP        = 1e-3;
  prodCutElectronsA        = 1e-3;
  prodCutPositrons         = 1e-3;
  prodCutPositronsP        = 1e-3;
  prodCutPositronsA        = 1e-3;
  prodCutProtons           = 1e-3;
  prodCutProtonsP          = 1e-3;
  prodCutProtonsA          = 1e-3;

  // biasing options
  defaultBiasVacuum        = "";
  defaultBiasMaterial      = "";

  // tracking options
  lengthSafety             = 1e-12; // be very careful adjusting this as it affects all the geometry
  maximumTrackingTime      = 0.1;
  deltaChord               = 0.00001; // m
  chordStepMinimum         = 0.000001;// m
  deltaIntersection        = 0.00001;
  minimumEpsilonStep       = 5e-5;    // default value in Geant4, old value 0
  maximumEpsilonStep       = 1e-3;    // default value in Geant4, old value 1e-7
  deltaOneStep             = 0.5e-5;  // default value in Geant4, old value 0.00001;
  stopTracks               = false;
  stopSecondaries          = false;

  // output / analysis options
  numberOfEventsPerNtuple  = 0;
  elossHistoBinWidth       = 1.0; // m
  elossHistoTransBinWidth  = 0.1;
  trajCutGTZ               = 0.0;  // minimum z position
  trajCutLTR               = 1e12; // maximum radius in mm, so large default value
  storeTrajectory          = false;
  storeTrajectoryDepth     = 1;
  storeTrajectoryParticle  = "";
  storeTrajectoryEnergyThreshold = -1.0;

  // circular options
  nturns                   = 1;

  printModuloFraction      = 0.1;
}


void OptionsBase::print() const
{
  std::cout<<"Options               : " << std::endl;
  std::cout<<"particle              : " << particleName             << std::endl;
  std::cout<<"nominal energy        : " << beamEnergy               << std::endl;
  std::cout<<"n particles           : " << nGenerate                << std::endl;
  std::cout<<"sigmaX                : " << sigmaX                   << std::endl;
  std::cout<<"BV sign               : " << ffact                    << std::endl;
  std::cout<<"Cerenkov on           : " << turnOnCerenkov           << std::endl;
  std::cout<<"Optical absorption on : " << turnOnOpticalAbsorption  << std::endl;
  std::cout<<"Mie scattering on     : " << turnOnMieScattering      << std::endl;
  std::cout<<"Rayleigh scatering on : " << turnOnRayleighScattering << std::endl;
  std::cout<<"Optical surface on    : " << turnOnOpticalSurface     << std::endl;
  std::cout<<"Birks saturation on   : " << turnOnBirksSaturation    << std::endl;
}

