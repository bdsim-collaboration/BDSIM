#include "options.h"

#include <iostream>

using namespace GMAD;

Options::Options() : OptionsBase()
{
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
  publish("dispx",&Options::dispx);
  publish("dispy",&Options::dispy);
  publish("dispxp",&Options::dispxp);
  publish("dispyp",&Options::dispyp);
  
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

  // bias options
  publish("defaultBiasVacuum",   &Options::defaultBiasVacuum);
  publish("defaultBiasMaterial", &Options::defaultBiasMaterial);

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