#include "options.h"

#include <algorithm>
#include <iostream>

using namespace GMAD;

Options::Options():
  OptionsBase()
{
  PublishMembers();
}

Options::Options(const GMAD::OptionsBase& options):
  OptionsBase(options)
{
  PublishMembers();
}

double Options::get_value(std::string property_name)const{
  double value;
  try {
    value = get<double>(this,property_name);
  }
  catch (std::runtime_error) {
    try {
      // try int and convert
      value = (double)get<int>(this,property_name);
    }
    catch (std::runtime_error) {
      try {
	// try long and convert
	value = (double)get<long>(this,property_name);
      }
      catch (std::runtime_error) {
	std::cerr << "options.cc> Error: unknown property \"" << property_name << "\" (only works on numerical properties)" << std::endl;
	exit(1);
      }
    }
  }
  return value;
}

void Options::Amalgamate(const Options& optionsIn, bool override)
{
  if (override)
    {
      for (auto const key : optionsIn.setKeys)
	{
	  try
	    {
          set(this, &optionsIn, key);
          setKeys.push_back(key);
        }
	  catch (std::runtime_error)
	    {
	      std::cerr << "Error: Amalgate unknown option \"" << key << "\"" << std::endl;
	      exit(1);
	    }
	}
    }
  else
    {// don't override - ie give preference to ones set in this instance
      for (auto const key : optionsIn.setKeys)
	{
	  auto const& ok = setKeys; // shortcut
	  auto result = std::find(ok.begin(), ok.end(), key);
	  if (result == ok.end())
	    {//it wasn't found so ok to copy
	      try
		{
          set(this, &optionsIn, key);
          setKeys.push_back(key);
        }
	      catch (std::runtime_error)
		{
		  std::cerr << "Error: Amalgate unknown option \"" << key << "\"" << std::endl;
		  exit(1);
		}
	    }
	}
    }
}

bool Options::HasBeenSet(std::string name) const
{
  auto result = std::find(setKeys.begin(), setKeys.end(), name);
  if (result == setKeys.end())
    {return false;}
  else
    {return true;}
}

void Options::PublishMembers()
{
  // executable options first
  publish("inputFileName",         &Options::inputFileName);
  publish("visMacroFileName",      &Options::visMacroFileName);
  publish("visDebug",              &Options::visDebug);
  publish("outputFileName",        &Options::outputFileName);
  publish("outputFormat",          &Options::outputFormat);
  publish("survey",                &Options::survey);
  publish("surveyFileName",        &Options::surveyFileName);
  publish("verbose",               &Options::verbose);
  publish("verboseEvent",          &Options::verboseEvent);
  publish("verboseStep",           &Options::verboseStep);
  publish("verboseEventNumber",    &Options::verboseEventNumber);
  publish("batch",                 &Options::batch);
  publish("verboseRunLevel",       &Options::verboseRunLevel);
  publish("verboseEventLevel",     &Options::verboseEventLevel);
  publish("verboseTrackingLevel",  &Options::verboseTrackingLevel);
  publish("verboseSteppingLevel",  &Options::verboseSteppingLevel);
  publish("circular",              &Options::circular);
  publish("seed",                  &Options::seed);
  publish("recreate",              &Options::recreate);
  publish("recreateFileName",      &Options::recreateFileName);
  publish("startFromEvent",        &Options::startFromEvent);
  publish("writeSeedState",        &Options::writeSeedState);
  publish("useASCIISeedState",     &Options::useASCIISeedState);
  publish("seedStateFileName",     &Options::seedStateFileName);
  publish("ngenerate",             &Options::nGenerate);
  publish("generatePrimariesOnly", &Options::generatePrimariesOnly);
  publish("exportGeometry",        &Options::exportGeometry);
  publish("exportType",            &Options::exportType);
  publish("exportFileName",        &Options::exportFileName);
  publish("bdsimPath",             &Options::bdsimPath);
  
  // options which influence the tracking
  publish("physicsList",&Options::physicsList);

  // options for the "beam" command
  publish("particle",&Options::particleName);

  publish("distrType", &Options::distribType);
  publish("xDistrType",&Options::xDistribType);
  publish("yDistrType",&Options::yDistribType);
  publish("zDistrType",&Options::zDistribType);
  publish("distrFile", &Options::distribFile);
  publish("distrFileFormat",   &Options::distribFileFormat);
  publish("matchDistribFileLength", &Options::matchDistribFileLength);
  publish("nlinesIgnore",      &Options::nlinesIgnore);
  publish("eventOffset",       &Options::eventOffset);
  publish("recreateSeedState", &Options::recreateSeedState);

  publish("elossHistoBinWidth",&Options::elossHistoBinWidth);
  publish("defaultRangeCut",&Options::defaultRangeCut);
  publish("ffact",&Options::ffact);
  publish("bv",   &Options::ffact); // MadX naming

  publish("energy",&Options::beamEnergy);

  publish("beamlineX",         &Options::beamlineX);
  publish("beamlineY",         &Options::beamlineY);
  publish("beamlineZ",         &Options::beamlineZ);
  publish("beamlinePhi",       &Options::beamlinePhi);
  publish("beamlineTheta",     &Options::beamlineTheta);
  publish("beamlinePsi",       &Options::beamlinePsi);
  publish("beamlineAxisX",     &Options::beamlineAxisX);
  publish("beamlineAxisY",     &Options::beamlineAxisY);
  publish("beamlineAxisZ",     &Options::beamlineAxisZ);
  publish("beamlineAngle",     &Options::beamlineAngle);
  publish("beamlineAxisAngle", &Options::beamlineAxisAngle);
  
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
  publish("haloEmitX",             &Options::haloEmitX);
  publish("haloEmitY",             &Options::haloEmitY);
  publish("haloEnvelopeEmitX",     &Options::haloEnvelopeEmitX);
  publish("haloEnvelopeEmitY",     &Options::haloEnvelopeEmitY);
  publish("haloEnvelopeCollMinX",  &Options::haloEnvelopeCollMinX);
  publish("haloEnvelopeCollMaxX",  &Options::haloEnvelopeCollMaxX);
  publish("haloEnvelopeCollMinXp", &Options::haloEnvelopeCollMinXp);
  publish("haloEnvelopeCollMaxXp", &Options::haloEnvelopeCollMaxXp);
  publish("haloEnvelopeCollMinY",  &Options::haloEnvelopeCollMinY);
  publish("haloEnvelopeCollMaxY",  &Options::haloEnvelopeCollMaxY);
  publish("haloEnvelopeCollMinYp", &Options::haloEnvelopeCollMinYp);
  publish("haloEnvelopeCollMaxYp", &Options::haloEnvelopeCollMaxYp);
  publish("haloPSWeightParameter", &Options::haloPSWeightParameter);
  publish("haloPSWeightFunction",  &Options::haloPSWeightFunction);

  publish("sigmaE",&Options::sigmaE);

  publish("checkOverlaps",&Options::checkOverlaps);
  publish("nperfile",&Options::numberOfEventsPerNtuple);
  publish("eventNumberOffset",&Options::eventNumberOffset);
  publish("vacuumPressure",&Options::vacuumPressure);
  publish("xsize",&Options::xsize);
  publish("ysize",&Options::ysize);
  // options which influence the geometry
  publish("magnetGeometryType",&Options::magnetGeometryType);
  publish("outerMaterial",&Options::outerMaterialName);
  publish("outerDiameter",&Options::outerDiameter);
  publish("boxSize",      &Options::outerDiameter); // for backwards compatability
  publish("includeIronMagFields",&Options::includeIronMagFields);
  publish("includeFringeFields",&Options::includeFringeFields);
  publish("beampipeRadius",&Options::aper1);
  publish("beampipeThickness",&Options::beampipeThickness);
  publish("apertureType",&Options::apertureType);
  publish("aper1",&Options::aper1);
  publish("aper2",&Options::aper2);
  publish("aper3",&Options::aper3);
  publish("aper4",&Options::aper4);
  publish("beampipeMaterial",&Options::beampipeMaterial);
  publish("ignoreLocalAperture", &Options::ignoreLocalAperture);
  publish("vacuumMaterial",&Options::vacMaterial);
  publish("emptyMaterial",&Options::emptyMaterial);
  publish("dontSplitSBends", &Options::dontSplitSBends);
  publish("thinElementLength", &Options::thinElementLength);

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

  publish("removeTemporaryFiles", &Options::removeTemporaryFiles);

  publish("samplerDiameter",&Options::samplerDiameter);
  
  // options for beam loss monitor geometry
  publish("blmRad",&Options::blmRad);
  publish("blmLength",&Options::blmLength);
  
  publish("scintYieldFactor",&Options::scintYieldFactor);
  publish("maximumPhotonsPerStep", &Options::maximumPhotonsPerStep);
  publish("maximumBetaChangePerStep", &Options::maximumBetaChangePerStep);
  publish("maximumTracksPerEvent", &Options::maximumTracksPerEvent);
  publish("sensitiveBeamlineComponents",&Options::sensitiveBeamlineComponents);
  publish("sensitiveBeamPipe",&Options::sensitiveBeamPipe);
  publish("sensitiveBLMs",&Options::sensitiveBLMs);

  publish("thresholdCutCharged",&Options::thresholdCutCharged);
  publish("thresholdCutPhotons",&Options::thresholdCutPhotons);

  publish("prodCutPhotons",  &Options::prodCutPhotons);
  publish("prodCutElectrons",&Options::prodCutElectrons);
  publish("prodCutPositrons",&Options::prodCutPositrons);
  publish("prodCutProtons",  &Options::prodCutProtons);
  publish("prodCutHadrons",  &Options::prodCutProtons); // backwards compatability

  // bias options
  publish("defaultBiasVacuum",   &Options::defaultBiasVacuum);
  publish("defaultBiasMaterial", &Options::defaultBiasMaterial);

  // options which influence tracking
  publish("integratorSet",      &Options::integratorSet);
  publish("maximumTrackingTime",&Options::maximumTrackingTime);
  publish("maximumStepLength",  &Options::maximumStepLength);
  publish("maximumTrackLength", &Options::maximumTrackLength);
  publish("chordStepMinimum",   &Options::chordStepMinimum);
  publish("deltaIntersection",  &Options::deltaIntersection);
  publish("minimumEpsilonStep", &Options::minimumEpsilonStep);
  publish("maximumEpsilonStep", &Options::maximumEpsilonStep);
  publish("deltaOneStep",       &Options::deltaOneStep);

  // physics processes
  publish("turnOnOpticalAbsorption",&Options::turnOnOpticalAbsorption);
  publish("turnOnMieScattering",&Options::turnOnMieScattering);
  publish("turnOnRayleighScattering",&Options::turnOnRayleighScattering);
  publish("turnOnOpticalSurface",&Options::turnOnOpticalSurface);

  publish("lengthSafety",&Options::lengthSafety);

  publish("storeElossLinks", &Options::storeElossLinks);
  publish("storeElossLocal",  &Options::storeElossLocal);
  publish("storeElossGlobal", &Options::storeElossGlobal);

  // trajectory storage
  publish("storeTrajectory",&Options::storeTrajectory);
  publish("storeTrajectories",&Options::storeTrajectory);
  publish("storeTrajectoryDepth",&Options::storeTrajectoryDepth);
  publish("storeTrajectoryParticle",&Options::storeTrajectoryParticle);
  publish("storeTrajectoryEnergyThreshold",&Options::storeTrajectoryEnergyThreshold);
  publish("trajConnect",&Options::trajConnect);
  publish("trajCutGTZ",&Options::trajCutGTZ);
  publish("trajCutLTR",&Options::trajCutLTR);
  publish("trajNoTransportation", &Options::trajNoTransportation);

  publish("stopSecondaries",&Options::stopSecondaries);
  publish("stopTracks",     &Options::stopTracks);
  publish("killNeutrinos",  &Options::killNeutrinos);
  publish("minimumRadiusOfCurvature", &Options::minimumRadiusOfCurvature);
  publish("nturns",         &Options::nturns);
  publish("printModuloFraction",&Options::printModuloFraction);
  publish("nSegmentsPerCircle", &Options::nSegmentsPerCircle);
  publish("writePrimaries", &Options::writePrimaries);

  // scoring map
  publish("nbinsx", &Options::nbinsx);
  publish("nbinsy", &Options::nbinsy);
  publish("nbinsz", &Options::nbinsz);
  publish("xmin",   &Options::xmin);
  publish("xmax",   &Options::xmax);
  publish("ymin",   &Options::ymin);
  publish("ymax",   &Options::ymax);
  publish("zmin",   &Options::zmin);
  publish("zmax",   &Options::zmax);
  publish("useScoringMap", &Options::useScoringMap);

  // tracker
  publish("trackingType",    &Options::trackingType);
  publish("trackingSteps",   &Options::trackingSteps);
  publish("dontUseAperture", &Options::dontUseAperture);
}
