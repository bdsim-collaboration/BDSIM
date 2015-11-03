#include "options.h"

#include <iostream>

using namespace GMAD;

Options::Options()
{
  // Default Values for Options

  // very important options
  physicsList           = "standard"; //default - standard (only transportation)
  modularPhysicsListsOn = 0;
  numberToGenerate      = 1;
  randomSeed            = 0;

  // beam options
  particleName          = "";
  distribType           = "";
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
  outerDiameter        = 0.2;

  // geometry debugging
  dontSplitSBends      = false; // always split sbends into smaller chunks by default
  
  includeIronMagFields = 0;
  sensitiveBeamlineComponents = 1;
  
  // beam pipe / aperture
  beampipeThickness    = 0.005;
  apertureType         = "circular";
  aper1                = 0.05; // also beampipeRadius
  aper2                = 0.0;
  aper3                = 0.0;
  aper4                = 0.0;
  beampipeMaterial     = "StainlessSteel";
  vacMaterial          = "Vacuum";
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
  gammaToMuFe              = 1;
  annihiToMuFe             = 1;
  eeToHadronsFe            = 1;
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
  decayOn                  = 1;
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

  // synchrotron radiation
  synchRadOn               = 0;
  synchTrackPhotons        = 0;
  synchLowX                = 0.0;
  synchLowGamE             = 0.0;
  synchPhotonMultiplicity  = 1;
  synchMeanFreeFactor      = 1;
  
  // output / analysis options
  numberOfEventsPerNtuple  = 0;
  elossHistoBinWidth       = 1.0; // m
  elossHistoTransBinWidth  = 0.1;
  storeMuonTrajectories    = false;
  trajCutGTZ               = 0.0;  // minimum z position
  trajCutLTR               = 1e12; // maximum radius in mm, so large default value
  storeNeutronTrajectories = false;
  storeTrajectory          = false;
  
  // circular options
  nturns                   = 1;

  printModuloFraction      = 0.1;

  PublishMembers();
}

void Options::PublishMembers()
{
  // options which influence the tracking
  publish("physicsList",&Options::physicsList);
  publish("modularPhysicsListsOn",&Options::modularPhysicsListsOn);

  // options for the "beam" command
  publish("particle",&Options::particleName);

  publish("distrType",&Options::distribType);
  publish("xDistrType",&Options::xDistribType);
  publish("yDistrType",&Options::yDistribType);
  publish("zDistrType",&Options::zDistribType);
  publish("distrFile",&Options::distribFile);
  publish("distrFileFormat",&Options::distribFileFormat);
  publish("ngenerate",&Options::numberToGenerate);
  publish("nlinesIgnore",&Options::nlinesIgnore);

  publish("elossHistoBinWidth",&Options::elossHistoBinWidth);
  publish("elossHistoTransBinWidth",&Options::elossHistoTransBinWidth);
  publish("defaultRangeCut",&Options::defaultRangeCut);
  publish("ffact",&Options::ffact);
  publish("bv",   &Options::ffact); // MadX naming

  publish("energy",&Options::beamEnergy);

  publish("X0",&Options::X0);
  publish("Y0",&Options::Y0);
  publish("Z0",&Options::Z0);
  publish("S0",&Options::S0);
  publish("Xp0",&Options::Xp0);
  publish("Yp0",&Options::Yp0);
  publish("Zp0",&Options::Zp0);
  publish("T0",&Options::T0);
  publish("E0",&Options::E0);
  publish("sigmaT",&Options::sigmaT);
  publish("betx",&Options::betx);
  publish("bety",&Options::bety);
  publish("alfx",&Options::alfx);
  publish("alfy",&Options::alfy);
  publish("emitx",&Options::emitx);
  publish("emity",&Options::emity);
  
  // options for beam distrType="gauss"
  publish("sigmaX",&Options::sigmaX);
  publish("sigmaXp",&Options::sigmaXp);
  publish("sigmaY",&Options::sigmaY);
  publish("sigmaYp",&Options::sigmaYp);

  // options for beam distrType="square" or distrType="circle"
  publish("envelopeX",&Options::envelopeX);
  publish("envelopeXp",&Options::envelopeXp);
  publish("envelopeY",&Options::envelopeY);
  publish("envelopeYp",&Options::envelopeYp);
  publish("envelopeT",&Options::envelopeT);
  publish("envelopeE",&Options::envelopeE);
  publish("envelopeR",&Options::envelopeR);
  publish("envelopeRp",&Options::envelopeRp);

  // options for beam distrType="gaussmatrix"
  publish("sigma11",&Options::sigma11);
  publish("sigma12",&Options::sigma12);
  publish("sigma13",&Options::sigma13);
  publish("sigma14",&Options::sigma14);
  publish("sigma15",&Options::sigma15);
  publish("sigma16",&Options::sigma16);
  publish("sigma22",&Options::sigma22);
  publish("sigma23",&Options::sigma23);
  publish("sigma24",&Options::sigma24);
  publish("sigma25",&Options::sigma25);
  publish("sigma26",&Options::sigma26);
  publish("sigma33",&Options::sigma33);
  publish("sigma34",&Options::sigma34);
  publish("sigma35",&Options::sigma35);
  publish("sigma36",&Options::sigma36);
  publish("sigma44",&Options::sigma44);
  publish("sigma45",&Options::sigma45);
  publish("sigma46",&Options::sigma46);
  publish("sigma55",&Options::sigma55);
  publish("sigma56",&Options::sigma56);
  publish("sigma66",&Options::sigma66);

  // options for beam distrType="eshell"
  publish("shellX",&Options::shellX);
  publish("shellXp",&Options::shellXp);
  publish("shellY",&Options::shellY);
  publish("shellYp",&Options::shellYp);
  publish("shellXWidth",&Options::shellXWidth);
  publish("shellXpWidth",&Options::shellXpWidth);
  publish("shellYWidth",&Options::shellYWidth);
  publish("shellYpWidth",&Options::shellYpWidth);

  // options for beam distrType="ring"
  publish("Rmin",&Options::Rmin);
  publish("Rmax",&Options::Rmax);

  // options for beam distrType="halo"
  publish("haloPSWeightParameter",&Options::haloPSWeightParameter);
  publish("haloPSWeightFunction",&Options::haloPSWeightFunction);

  publish("sigmaE",&Options::sigmaE);

  publish("doPlanckScattering",&Options::doPlanckScattering);
  publish("checkOverlaps",&Options::checkOverlaps);
  publish("nperfile",&Options::numberOfEventsPerNtuple);
  publish("eventNumberOffset",&Options::eventNumberOffset);
  publish("vacuumPressure",&Options::vacuumPressure);
  publish("planckScatterFe",&Options::planckScatterFe);
  publish("xsize",&Options::xsize);
  publish("ysize",&Options::ysize);
  // options which influence the geometry
  publish("magnetGeometryType",&Options::magnetGeometryType);
  publish("outerMaterial",&Options::outerMaterialName);
  publish("outerDiameter",&Options::outerDiameter);
  publish("boxSize",      &Options::outerDiameter); // for backwards compatability
  publish("includeIronMagFields",&Options::includeIronMagFields);
  publish("beampipeRadius",&Options::aper1);
  publish("beampipeThickness",&Options::beampipeThickness);
  publish("apertureType",&Options::apertureType);
  publish("aper1",&Options::aper1);
  publish("aper2",&Options::aper2);
  publish("aper3",&Options::aper3);
  publish("aper4",&Options::aper4);
  publish("beampipeMaterial",&Options::beampipeMaterial);
  publish("vacuumMaterial",&Options::vacMaterial);
  publish("dontSplitSBends", &Options::dontSplitSBends);

  // tunnel options
  publish("buildTunnel",&Options::buildTunnel);
  publish("buildTunnelStraight",&Options::buildTunnelStraight);
  publish("tunnelType",&Options::tunnelType);
  publish("tunnelThickness",&Options::tunnelThickness);
  publish("tunnelSoilThickness",&Options::tunnelSoilThickness);
  publish("tunnelMaterial",&Options::tunnelMaterial);
  publish("soilMaterial",&Options::soilMaterial);
  publish("buildTunnelFloor",&Options::buildTunnelFloor);
  publish("tunnelFloorOffset",&Options::tunnelFloorOffset);
  publish("tunnelAper1", &Options::tunnelAper1);
  publish("tunnelAper2", &Options::tunnelAper2);
  publish("tunnelRadius",&Options::tunnelAper1); // for backwards compatability
  publish("tunnelSensitive",&Options::tunnelSensitive);
  publish("tunnelVisible",&Options::tunnelVisible);
  publish("showTunnel",&Options::tunnelVisible); // for backwards compatability
  
  publish("tunnelOffsetX",&Options::tunnelOffsetX);
  publish("tunnelOffsetY",&Options::tunnelOffsetY);

  publish("samplerDiameter",&Options::samplerDiameter);
  
  // options for beam loss monitor geometry
  publish("blmRad",&Options::blmRad);
  publish("blmLength",&Options::blmLength);

  publish("gammaToMuFe",&Options::gammaToMuFe);
  publish("annihiToMuFe",&Options::annihiToMuFe);
  publish("eeToHadronsFe",&Options::eeToHadronsFe);
  publish("scintYieldFactor",&Options::scintYieldFactor);
  publish("useEMLPB",&Options::useEMLPB);
  publish("useHadLPB",&Options::useHadLPB);

  publish("sensitiveBeamlineComponents",&Options::sensitiveBeamlineComponents);
  publish("sensitiveBeamPipe",&Options::sensitiveBeamPipe);
  publish("sensitiveBLMs",&Options::sensitiveBLMs);
  publish("LPBFraction",&Options::LPBFraction);

  publish("thresholdCutCharged",&Options::thresholdCutCharged);
  publish("thresholdCutPhotons",&Options::thresholdCutPhotons);

  publish("prodCutPhotons",&Options::prodCutPhotons);
  publish("prodCutPhotonsP",&Options::prodCutPhotonsP);
  publish("prodCutPhotonsA",&Options::prodCutPhotonsA);
  publish("prodCutElectrons",&Options::prodCutElectrons);
  publish("prodCutElectronsP",&Options::prodCutElectronsP);
  publish("prodCutElectronsA",&Options::prodCutElectronsA);
  publish("prodCutPositrons",&Options::prodCutPositrons);
  publish("prodCutPositronsP",&Options::prodCutPositronsP);
  publish("prodCutPositronsA",&Options::prodCutPositronsA);
  publish("prodCutProtons",&Options::prodCutProtons);
  publish("prodCutHadrons",&Options::prodCutProtons); // backwards compatability
  publish("prodCutProtonsP",&Options::prodCutProtonsP);
  publish("prodCutProtonsA",&Options::prodCutProtonsA);

  // options which influence tracking 
  publish("maximumTrackingTime",&Options::maximumTrackingTime);
  publish("deltaChord",&Options::deltaChord);
  publish("chordStepMinimum",&Options::chordStepMinimum);
  publish("deltaIntersection",&Options::deltaIntersection);
  publish("minimumEpsilonStep",&Options::minimumEpsilonStep);
  publish("maximumEpsilonStep",&Options::maximumEpsilonStep);
  publish("deltaOneStep",&Options::deltaOneStep);

  // physics processes
  publish("turnOnCerenkov",&Options::turnOnCerenkov);
  publish("turnOnOpticalAbsorption",&Options::turnOnOpticalAbsorption);
  publish("turnOnMieScattering",&Options::turnOnMieScattering);
  publish("turnOnRayleighScattering",&Options::turnOnRayleighScattering);
  publish("turnOnOpticalSurface",&Options::turnOnOpticalSurface);
  publish("turnOnBirksSaturation",&Options::turnOnBirksSaturation);

  publish("synchRadOn",&Options::synchRadOn);
  publish("decayOn",&Options::decayOn);
  publish("srTrackPhotons",&Options::synchTrackPhotons);
  publish("srLowX",&Options::synchLowX);
  publish("srLowGamE",&Options::synchLowGamE);
  publish("srMultiplicity",&Options::synchPhotonMultiplicity);
  publish("srMeanFreeFactor",&Options::synchMeanFreeFactor);
  publish("lengthSafety",&Options::lengthSafety);
  publish("randomSeed",&Options::randomSeed);
  publish("storeMuonTrajectory",&Options::storeMuonTrajectories);
  publish("storeMuonTrajectories",&Options::storeMuonTrajectories);
  publish("trajCutGTZ",&Options::trajCutGTZ);
  publish("trajCutLTR",&Options::trajCutLTR);
  publish("storeNeutronTrajectory",&Options::storeNeutronTrajectories);
  publish("storeNeutronTrajectories",&Options::storeNeutronTrajectories);
  publish("storeTrajectory",&Options::storeTrajectory);
  publish("storeTrajectories",&Options::storeTrajectory);
  publish("stopSecondaries",&Options::stopSecondaries);
  publish("stopTracks",&Options::stopTracks);
  publish("nturns",&Options::nturns);
  publish("printModuloFraction",&Options::printModuloFraction);
}

void Options::print() const
{
  std::cout<<"Options               : " <<std::endl;
  std::cout<<"particle              : " <<particleName<<std::endl;
  std::cout<<"nominal energy        : " <<beamEnergy<<std::endl;
  std::cout<<"n macroparticles      : " <<numberToGenerate<<std::endl;
  std::cout<<"sigmaX                : " <<sigmaX<<std::endl;
  std::cout<<"BV sign               : " <<ffact<<std::endl;
  std::cout<<"Cerenkov on           : " <<turnOnCerenkov<<std::endl;
  std::cout<<"Optical absorption on : " << turnOnOpticalAbsorption <<std::endl;
  std::cout<<"Mie scattering on     : " << turnOnMieScattering <<std::endl;
  std::cout<<"Rayleigh scatering on : " << turnOnRayleighScattering <<std::endl;
  std::cout<<"Optical surface on    : " << turnOnOpticalSurface <<std::endl;
  std::cout<<"Birks saturation on   : " << turnOnBirksSaturation <<std::endl;
}
