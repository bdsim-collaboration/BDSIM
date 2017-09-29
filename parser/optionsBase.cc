#include "optionsBase.h"

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
  outputFormat          = "rootevent";
  survey                = false;
  surveyFileName        = "survey.dat";
  batch                 = false;
  verbose               = false;
  verboseEvent          = false;
  verboseStep           = false;
  verboseEventNumber    = -1;
  verboseRunLevel       = 0;
  verboseEventLevel     = 0;
  verboseTrackingLevel  = 0;
  verboseSteppingLevel  = 0;
  circular              = false;
  seed                  = -1;
  nGenerate             = 1;
  recreate              = false;
  recreateFileName      = "";
  startFromEvent        = 0;
  writeSeedState        = false;
  useASCIISeedState     = false;
  seedStateFileName     = "";
  generatePrimariesOnly = false;
  exportGeometry        = false;
  exportType            = "gdml";
  exportFileName        = "geometry";
  bdsimPath             = "";
  
  // very important options
  physicsList           = ""; //default - only transportation

  // beam options
  particleName          = "";
  distribType           = "reference";
  xDistribType          = "";
  yDistribType          = "";
  zDistribType          = "";
  distribFile           = "";
  distribFileFormat     = "";
  matchDistribFileLength = true;
  
  nlinesIgnore          = 0;
  eventOffset           = 0;
  recreateSeedState     = true;

  elossHistoBinWidth      = 1.0; // m
  
  ffact                 = 1.0;
  beamEnergy            = 0.0;

  beamlineX         = 0;
  beamlineY         = 0;
  beamlineZ         = 0;
  beamlinePhi       = 0;
  beamlineTheta     = 0;
  beamlinePsi       = 0;
  beamlineAxisX     = 0;
  beamlineAxisY     = 0;
  beamlineAxisZ     = 0;
  beamlineAngle     = 0;
  beamlineAxisAngle = false;

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

  haloEmitX             =  0;
  haloEmitY             =  0;

  // TODO : check if we really need default arguments here as
  // yields incorrect behaviour when collimators are not used
#if 0
  haloEnvelopeEmitX     =  1e9;
  haloEnvelopeEmitY     =  1e9;
  haloEnvelopeCollMinX  =  1e9;
  haloEnvelopeCollMaxX  = -1e9;
  haloEnvelopeCollMinXp =  1e9;
  haloEnvelopeCollMaxXp = -1e9;
  haloEnvelopeCollMinY  =  1e9;
  haloEnvelopeCollMaxY  = -1e9;
  haloEnvelopeCollMinYp =  1e9;
  haloEnvelopeCollMaxYp = -1e9;
#endif
  haloEnvelopeEmitX     =  0;
  haloEnvelopeEmitY     =  0;
  haloEnvelopeCollMinX  =  0;
  haloEnvelopeCollMaxX  =  0;
  haloEnvelopeCollMinXp =  0;
  haloEnvelopeCollMaxXp =  0;
  haloEnvelopeCollMinY  =  0;
  haloEnvelopeCollMaxY  =  0;
  haloEnvelopeCollMinYp =  0;
  haloEnvelopeCollMaxYp =  0;

  haloPSWeightParameter = 1.0;
  haloPSWeightFunction  = "";
  
  sigmaE = 0.0;

  eventNumberOffset       = 0;

  // general geometrical prameters
  checkOverlaps           = false;
  xsize=0.0, ysize=0.0;

  // magnet geometry
  magnetGeometryType   = "polessquare";
  outerMaterialName    = "iron";
  outerDiameter        = 0.6;
  thinElementLength    = 1e-7;

  // geometry debugging
  // always split sbends into smaller chunks by default
  dontSplitSBends      = false;
  includeFringeFields  = false;

  includeIronMagFields = false;
  sensitiveBeamlineComponents = true;

  // beam pipe / aperture
  beampipeThickness    = 0.0025;
  apertureType         = "circular";
  aper1                = 0.025; // also beampipeRadius
  aper2                = 0.0;
  aper3                = 0.0;
  aper4                = 0.0;
  beampipeMaterial     = "StainlessSteel";
  ignoreLocalAperture  = false;
  
  vacMaterial          = "Vacuum";
  emptyMaterial        = "G4_Galactic";
  vacuumPressure       = 1e-12;
  sensitiveBeamPipe    = true;

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

  removeTemporaryFiles = false;
  
  // samplers
  samplerDiameter     = 5; // m

  // beam loss monitors geometry
  blmRad                   = 0.05;
  blmLength                = 0.18;
  sensitiveBLMs            = true;

  // physics processes
  turnOnOpticalAbsorption  = true;
  turnOnMieScattering      = true;
  turnOnRayleighScattering = true;
  turnOnOpticalSurface     = true;
  scintYieldFactor         = 1.0;
  maximumPhotonsPerStep    = -1;  ///< -1 -> no action take (could want 0)
  maximumBetaChangePerStep = 10;
  maximumTracksPerEvent    = 0;   ///< 0 -> no action taken
  thresholdCutCharged      = 0.0;
  thresholdCutPhotons      = 0.0;
  defaultRangeCut          = 1e-3;
  prodCutPhotons           = 1e-3;
  prodCutElectrons         = 1e-3;
  prodCutPositrons         = 1e-3;
  prodCutProtons           = 1e-3;

  // biasing options
  defaultBiasVacuum        = "";
  defaultBiasMaterial      = "";

  // tracking options
  integratorSet            = "bdsimtwo";
  lengthSafety             = 1e-9;   // be very careful adjusting this as it affects all the geometry
  maximumTrackingTime      = -1;      // s, nonsensical - used for testing
  maximumStepLength        = 20;      // m, quite big
  maximumTrackLength       = 1e90;    // m, no limit but smaller than DBL_MAX for safe *CLHEP::m
  chordStepMinimum         = 1e-9;    // m // minimum step in a field for an integrator
  deltaIntersection        = 1e-8;    // m - should be greater than lengthSafety!

  // default value in Geant4, old value 0 - error must be greater than this
  minimumEpsilonStep       = 5e-25;
  maximumEpsilonStep       = 1e-7;    // default value in Geant4, old value 1e-7
  deltaOneStep             = 1e-6;    // maximum allowed spatial error in position (1um)
  stopTracks               = false;
  stopSecondaries          = false;
  killNeutrinos            = true;
  minimumRadiusOfCurvature = 0.05; // 5cm - typical aperture

  // output / analysis options
  numberOfEventsPerNtuple  = 0;
  trajCutGTZ               = 1e99;  // minimum z position, so large default value
  trajCutLTR               = 0.0;   // maximum radius in mm, so small default value
  trajConnect              = false; // connect disconnected trajectory trees
  trajNoTransportation     = false;
  storeElossLinks          = false;
  storeElossLocal          = false;
  storeElossGlobal         = false;
  storeTrajectory          = false;
  storeTrajectoryDepth     = 0;
  storeTrajectoryParticle  = "";
  storeTrajectoryEnergyThreshold = -1.0;
  writePrimaries           = true;

  // circular options
  nturns                   = 1;

  printModuloFraction      = 0.1;

  // visualisation
  nSegmentsPerCircle       = 50;

  // scoring map
  nbinsx = 1;
  nbinsy = 1;
  nbinsz = 1;
  xmin   = 0;
  xmax   = 0;
  ymin   = 0;
  ymax   = 0;
  zmin   = 0;
  zmax   = 0;
  useScoringMap = false;

  // tracker options
  trackingType             = "hybrid";
  trackingSteps            = 1;
  dontUseAperture          = false;
}


void OptionsBase::print() const
{
  std::cout<<"Options                 " << std::endl;
  std::cout<<"particle              : " << particleName             << std::endl;
  std::cout<<"nominal energy        : " << beamEnergy               << std::endl;
  std::cout<<"n particles           : " << nGenerate                << std::endl;
  std::cout<<"sigmaX                : " << sigmaX                   << std::endl;
  std::cout<<"BV sign               : " << ffact                    << std::endl;
  std::cout<<"Optical absorption on : " << turnOnOpticalAbsorption  << std::endl;
  std::cout<<"Mie scattering on     : " << turnOnMieScattering      << std::endl;
  std::cout<<"Rayleigh scatering on : " << turnOnRayleighScattering << std::endl;
  std::cout<<"Optical surface on    : " << turnOnOpticalSurface     << std::endl;
}

