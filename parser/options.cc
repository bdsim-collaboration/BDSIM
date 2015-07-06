#include "options.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

Options::Options(){
  // Default Values for Options

  physicsList = "standard"; //default - standard (only transportation)
  modularPhysicsListsOn = 0;
  particleName = "";
  distribType = "";
  xDistribType = ""; 
  yDistribType = "";
  zDistribType = "";
  distribFile = "";
  distribFileFormat = "";
  haloPSWeightParameter = 1.0;
  haloPSWeightFunction = "";

  numberToGenerate = 1;
  nlinesIgnore = 0;
  elossHistoBinWidth = 1.0;
  elossHistoTransBinWidth = 0.1;
  defaultRangeCut = 7e-4;
  ffact = 1.0;
  beamEnergy = 0.0;
  
  X0 = 0.0, Y0 = 0.0, Z0 = 0.0;
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

  doPlanckScattering=0;
  checkOverlaps=0;
  numberOfEventsPerNtuple=0;
  eventNumberOffset=0;

  vacuumPressure = 1e-12;
  planckScatterFe = 1.0;

  xsize=0.0, ysize=0.0;

  magnetGeometryType = "cylindrical";
  outerMaterialName  = "iron";
  outerDiameter      = 0.2;
  tunnelRadius       = 2.0;
  beampipeRadius     = 0.05;
  beampipeThickness  = 0.005;

  apertureType      = "circular";
  aper1             = 0.0;
  aper2             = 0.0;
  aper3             = 0.0;
  aper4             = 0.0;
    
  beampipeMaterial     = "StainlessSteel";
  vacMaterial          = "Vacuum";
  tunnelMaterial       = "concrete";
  tunnelCavityMaterial = "Air";
  soilMaterial         = "soil";

  includeIronMagFields = 0;

  buildTunnel = 0;
  buildTunnelFloor = 0;
  showTunnel = 0;
  tunnelOffsetX = 0;
  tunnelOffsetY = 0;
  samplerDiameter = 5; // m
  tunnelThickness = 0.0;
  tunnelSoilThickness = 0.0;
  tunnelFloorOffset = 0.0;

  geometryBias = 0;
  //Beam loss monitors geometry
  blmRad = 0.05;
  blmLength = 0.18;

  gammaToMuFe = 1;
  annihiToMuFe = 1;
  eeToHadronsFe = 1;
  useEMLPB = 0;
  useHadLPB = 0;

  sensitiveBeamlineComponents = 1;
  sensitiveBeamPipe = 1;
  sensitiveBLMs = 1;

  turnOnCerenkov = 1;
  turnOnOpticalAbsorption = 1;
  turnOnMieScattering = 1;
  turnOnRayleighScattering = 1;
  turnOnOpticalSurface = 1;
  turnOnBirksSaturation = 1;
  scintYieldFactor = 1.0;
  decayOn = 1;
  //  synchRadOn = 0;


  LPBFraction = 0.0;

  thresholdCutCharged = 0.0;
  thresholdCutPhotons = 0.0;

  prodCutPhotons=7e-4;
  prodCutPhotonsP=7e-4;
  prodCutPhotonsA=1;
  prodCutElectrons=7e-4;
  prodCutElectronsP=7e-4;
  prodCutElectronsA=1;
  prodCutPositrons=7e-4;
  prodCutPositronsP=7e-4;
  prodCutPositronsA=1;


  //tracking options
  maximumTrackingTime = 0.1;
  deltaChord = 0.00001; // m
  chordStepMinimum = 0.000001; // m
  deltaIntersection = 0.00001;
  minimumEpsilonStep=5e-5; // default value in Geant4, old value 0
  maximumEpsilonStep=1e-3; // default value in Geant4, old value 1e-7
  deltaOneStep = 0.5e-5; // default value in Geant4, old value 0.00001;
  turnOnCerenkov = 1;
  synchRadOn = 0;
  decayOn = 1;
  synchTrackPhotons = 0;
  synchLowX = 0.0;
  synchLowGamE = 0.0;
  synchPhotonMultiplicity = 1;
  synchMeanFreeFactor = 1;
  lengthSafety = 0.000000001;
  randomSeed = 0;
  
  useTimer = 0;
  storeMuonTrajectories = 0;
  trajCutGTZ = 0.0;
  trajCutLTR = 0.0;
  storeNeutronTrajectories = 0;
  storeTrajectory = 0;
  stopTracks = 0;

  fifo = "";
  refvolume = "";
  refcopyno = 0;

  // ring options
  nturns = 1;
}

void Options::print() const {
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

void Options::set_value(std::string name, double value )
{
#ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << name << value << std::endl;
#endif
  //
  // numeric options for the "beam" command
  //
  if(name == "elossHistoBinWidth") {elossHistoBinWidth = value; return;}
  if(name == "elossHistotransBinWidth") {elossHistoTransBinWidth = value; return;}
  if(name == "defaultRangeCut") {defaultRangeCut = value; return;}
  if(name == "ffact") {ffact = value; return;}
  if(name == "bv")  { ffact = value; return; } // MadX naming
  if(name == "energy") {beamEnergy = value; return;}
  if(name == "X0" ) { X0 = value; return; }
  if(name == "Y0" ) { Y0 = value; return; }
  if(name == "Z0" ) { Z0 = value; return; }
  if(name == "T0" ) { T0 = value; return; }
  if(name == "Xp0" ) { Xp0 = value; return; }
  if(name == "Yp0" ) { Yp0 = value; return; }
  if(name == "Zp0" ) { Zp0 = value; return; }
  if(name == "E0") {E0 = value; return;}

  if(name == "sigmaT" ) { sigmaT = value; return; }
  if(name == "sigmaE" ) { sigmaE = value; return; }

  // options for beam distrType="gauss"
  if(name == "sigmaX" ) { sigmaX = value; return; }
  if(name == "sigmaY" ) { sigmaY = value; return; }
  if(name == "sigmaXp" ) { sigmaXp = value; return; }
  if(name == "sigmaYp" ) { sigmaYp = value; return; }

  // options for beam distrType="square" or distrType="circle"
  if(name == "envelopeX"  ) { envelopeX  = value; return; }
  if(name == "envelopeY"  ) { envelopeY  = value; return; }
  if(name == "envelopeXp" ) { envelopeXp = value; return; }
  if(name == "envelopeYp" ) { envelopeYp = value; return; }
  if(name == "envelopeT"  ) { envelopeT  = value; return; }
  if(name == "envelopeE"  ) { envelopeE  = value; return; }
  if(name == "envelopeR"  ) { envelopeR  = value; return; }
  if(name == "envelopeRp" ) { envelopeRp = value; return; }

  // options for beam distrType="gaussmatrix"
  if(name == "sigma11" ) { sigma11 = value; return; }
  if(name == "sigma12" ) { sigma12 = value; return; }
  if(name == "sigma13" ) { sigma13 = value; return; }
  if(name == "sigma14" ) { sigma14 = value; return; }
  if(name == "sigma15" ) { sigma15 = value; return; }
  if(name == "sigma16" ) { sigma16 = value; return; }

  if(name == "sigma22" ) { sigma22 = value; return; }
  if(name == "sigma23" ) { sigma23 = value; return; }
  if(name == "sigma24" ) { sigma24 = value; return; }
  if(name == "sigma25" ) { sigma25 = value; return; }
  if(name == "sigma26" ) { sigma26 = value; return; }

  if(name == "sigma33" ) { sigma33 = value; return; }
  if(name == "sigma34" ) { sigma34 = value; return; }
  if(name == "sigma35" ) { sigma35 = value; return; }
  if(name == "sigma36" ) { sigma36 = value; return; }

  if(name == "sigma44" ) { sigma44 = value; return; }
  if(name == "sigma45" ) { sigma45 = value; return; }
  if(name == "sigma46" ) { sigma46 = value; return; }

  if(name == "sigma55" ) { sigma55 = value; return; }
  if(name == "sigma56" ) { sigma56 = value; return; }

  if(name == "sigma66" ) { sigma66 = value; return; }
    
  // options for beam distrType="eshell"
  if(name == "shellX"  ) { shellX  = value; return; }
  if(name == "shellY"  ) { shellY  = value; return; }
  if(name == "shellXp" ) { shellXp = value; return; }
  if(name == "shellYp" ) { shellYp = value; return; }
  if(name == "shellXWidth" ) { shellXWidth  = value; return;}
  if(name == "shellXpWidth") { shellXpWidth = value; return;}
  if(name == "shellYWidth" ) { shellYWidth  = value; return;}
  if(name == "shellYpWidth") { shellYpWidth = value; return;}

  // options for beam distrType="ring"
  if(name == "Rmin" ) { Rmin = value; return; }
  if(name == "Rmax" ) { Rmax = value; return; }

  // options for beam distrType="halo"
  if(name == "haloPSWeightParameter") {haloPSWeightParameter= value; return;}

  //
  // numeric options for the"option" command
  //

  // options for beam loss monitor geometry
  if(name == "blmRad" ) { blmRad = value; return; }
  if(name == "blmLength" ) { blmLength = value; return; }

  // options which influence the geometry
  if(name == "outerDiameter" ) {outerDiameter = value; return; }
  if(name == "boxSize")        {outerDiameter = value; return; } // for backwards compatability
  if(name == "tunnelRadius" ) { tunnelRadius = value; return; }
  if(name == "beampipeThickness" ) { beampipeThickness = value; return; }
  if(name == "beampipeRadius" ) { beampipeRadius = value; return; }
  if(name == "aper1" ) { aper1 = value; return; }
  if(name == "aper2" ) { aper2 = value; return; }
  if(name == "aper3" ) { aper3 = value; return; }
  if(name == "aper4" ) { aper4 = value; return; }

  if(name == "includeIronMagFields") {
    includeIronMagFields = (int)value; return;
  } 

  if(name == "buildTunnel") {
      buildTunnel = (int)value; return;
  }

  if(name == "buildTunnelFloor") {
    buildTunnelFloor = (int)value; return;
  }
  
  if(name == "showTunnel") {
    showTunnel = (int)value; return;
  }  
  if(name == "tunnelOffsetX" ) { tunnelOffsetX = value; return; }
  if(name == "tunnelOffsetY" ) { tunnelOffsetY = value; return; }
  if(name == "samplerDiameter" ) { samplerDiameter = value; return; }
  if(name == "tunnelThickness" ) { tunnelThickness = value; return; }
  if(name == "tunnelSoilThickness" ) { tunnelSoilThickness = value; return; }
  if(name == "tunnelFloorOffset" ) { tunnelFloorOffset = value; return; }

  //geometry biasing
  if(name == "geometryBias") {
    geometryBias = (int)value; return;
  }


  // options which influence tracking 
  if(name == "maximumTrackingTime") {maximumTrackingTime = value; return;}
  if(name == "deltaChord") { deltaChord = value; return; }
  if(name == "deltaIntersection") { deltaIntersection = value; return; }
  if(name == "chordStepMinimum") { chordStepMinimum = value; return; }
  if(name == "lengthSafety") { lengthSafety = value; return; }
  if(name == "minimumEpsilonStep" ) { minimumEpsilonStep = value; return; }
  if(name == "maximumEpsilonStep" ) { maximumEpsilonStep = value; return; }
  if(name == "deltaOneStep" ) { deltaOneStep = value; return; }

  // physics processes
  if(name == "modularPhysicsListsOn") {
    modularPhysicsListsOn = (int)value; return;
  }
  if(name == "synchRadOn") { 
    synchRadOn=(int)value; return; 
  }
  if(name == "decayOn") { 
    decayOn = (int)value; return; 
  }
  if(name == "turnOnCerenkov") {
      turnOnCerenkov = (int)value; return;
  }
  if(name == "turnOnOpticalAbsorption") {
      turnOnOpticalAbsorption = (int)value; return;
  }
  if(name == "turnOnMieScattering") {
      turnOnMieScattering = (int)value; return;
  }
  if(name == "turnOnRayleighScattering") {
      turnOnRayleighScattering = (int)value; return;
  }
  if(name == "turnOnOpticalSurface") {
      turnOnOpticalSurface = (int)value; return;
  }
  if(name == "turnOnBirksSaturation") {
      turnOnBirksSaturation = (int)value; return;
  }
  if(name == "srTrackPhotons") {
    synchTrackPhotons = (int)value; return;
  }
  if(name == "useEMLPB") { useEMLPB = (int)value; return; }
  if(name == "useHadLPB") { useHadLPB = (int)value; return; }
  if(name == "sensitiveBeamlineComponents") { sensitiveBeamlineComponents = (int)value; return; }
  if(name == "sensitiveBeamPipe") { sensitiveBeamPipe = (int)value; return; }
  if(name == "sensitiveBLMs") { sensitiveBLMs = (int)value; return; }
  if(name == "LPBFraction") { LPBFraction = value; return; }
  if(name == "annihiToMuFe") { annihiToMuFe = value; return; }
  if(name == "gammaToMuFe") { gammaToMuFe = value; return; }
  if(name == "scintYieldFactor") { scintYieldFactor = value; return; }
  if(name == "eeToHadronsFe") { eeToHadronsFe = value; return; }
  if(name == "thresholdCutCharged" ) { thresholdCutCharged = (double)value; return; }
  if(name == "thresholdCutPhotons" ) { thresholdCutPhotons = (double)value; return; }
  if(name == "vacuumPressure") { vacuumPressure = (double)value; return; }
  if(name == "planckScatterFe") { planckScatterFe = (double)value; return; }
  if(name == "stopTracks") { stopTracks = (int) value; return; } 
  if(name == "srLowX") { synchLowX = value; return; }
  if(name == "srLowGamE") { synchLowGamE = value; return; }
  if(name == "srMultiplicity") { synchPhotonMultiplicity = (int) value; return; }
  if(name == "srMeanFreeFactor") { synchMeanFreeFactor = (int) value; return; }

  if(name == "prodCutPhotons" ) { prodCutPhotons = value; return; }
  if(name == "prodCutPhotonsP" ) { prodCutPhotonsP = value; return; }
  if(name == "prodCutPhotonsA" ) { prodCutPhotonsA = value; return; }
  if(name == "prodCutElectrons" ) { prodCutElectrons = value; return; }
  if(name == "prodCutElectronsP" ) { prodCutElectronsP = value; return; }
  if(name == "prodCutElectronsA" ) { prodCutElectronsA = value; return; }
  if(name == "prodCutPositrons" ) { prodCutPositrons = value; return; }
  if(name == "prodCutPositronsP" ) { prodCutPositronsP = value; return; }
  if(name == "prodCutPositronsA" ) { prodCutPositronsA = value; return; }

  // twiss parameters
  if(name == "betx" ) { betx = value; return; }
  if(name == "bety" ) { bety = value; return; }
  if(name == "alfx" ) { alfx = value; return; }
  if(name == "alfy" ) { alfy = value; return; }
  if(name == "emitx" ) { emitx = value; return; }
  if(name == "emity" ) { emity = value; return; }
  if(name == "doPlanckScattering" ) { doPlanckScattering = (int) value; return; }
  if(name == "checkOverlaps" ) { checkOverlaps = (int) value; return; }

  if(name == "storeTrajectory") { storeTrajectory = (int) value; return; } 
  if(name == "storeTrajectories") { storeTrajectory = (int) value; return; } 
  if(name == "storeMuonTrajectory") { storeMuonTrajectories = (int) value; return; } 
  if(name == "storeMuonTrajectories") { storeMuonTrajectories = (int) value; return; } 
  if(name == "trajCutGTZ") { trajCutGTZ = (double) value; return; } 
  if(name == "trajCutLTR") { trajCutLTR = (double) value; return; } 

  if(name == "storeNeutronTrajectory") { storeNeutronTrajectories = (int) value; return; } 
  if(name == "storeNeutronTrajectories") { storeNeutronTrajectories = (int) value; return; } 


  // options for generation and storage
  if(name == "randomSeed") { randomSeed = (int) value; return; }
  if(name == "ngenerate" ) { numberToGenerate = (int)value; return; }
  if(name == "nperfile" ) { numberOfEventsPerNtuple = (int)value; return; }
  if(name == "eventNumberOffset" ) { eventNumberOffset = (int)value; return; }
  if(name == "nlinesIgnore") { nlinesIgnore = (int) value; return; }

  // options for neutrons
  if(name=="refcopyno") { refcopyno = (int) value; return; }
  
  // option for rings
  if(name=="nturns") {nturns = (int) value; return; }
  
  std::cerr << "parser> Error: unknown option \"" << name << "\"" << std::endl; 
  exit(1);
}

void Options::set_value(std::string name, std::string value )
{
#ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << name << value << std::endl;
#endif
  // 
  // string options for the "beam" command
  //
  if(name == "particle") { particleName = value; return; }
  if(name == "distrType" ) { distribType = value; return; }
  if(name == "xDistrType" ) { xDistribType = value; return; }
  if(name == "yDistrType" ) { yDistribType = value; return; }
  if(name == "zDistrType" ) { zDistribType = value; return; }
  if(name == "distrFile" ) { distribFile = value; return; }
  if(name == "distrFileFormat" ) { distribFileFormat = value; return; }
  if(name == "haloPSWeightFunction")  {haloPSWeightFunction = value; return;}

  //
  // string options for the "option" command
  //

  // options which influence the geometry
  if(name == "magnetGeometryType" ){ magnetGeometryType = value; return; }
  if(name == "outerMaterial" )     { outerMaterialName  = value; return; }
  if(name == "apertureType" )      { apertureType       = value; return; }
  if(name == "beampipeMaterial" )  { beampipeMaterial   = value; return; }
  if(name == "vacuumMaterial" )    { vacMaterial        = value; return; }
  if(name == "tunnelMaterial" )    { tunnelMaterial     = value; return; }
  if(name == "tunnelCavityMaterial" ) { tunnelCavityMaterial = value; return; }
  if(name == "soilMaterial" )      { soilMaterial       = value; return; }
  
  // options which influence the tracking
  if(name == "physicsList" ) { physicsList = value; return; } 
  

  // options for external code interfaces
  if(name == "fifo") { fifo = value; return; }
  if(name == "refvolume") { refvolume = value; return; }
  std::cerr << "Error: parser.h> unknown option \"" << name << "\"" << std::endl; 
  exit(1);
}
