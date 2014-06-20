#include "options.h"

#include "getEnv.h"

#include <cstdlib>
#include <iostream>
#include <string>

void Options::print() const {
  std::cout<<"Options : "<<std::endl;
  std::cout<<"particle : "<<particleName<<std::endl;
  std::cout<<"energy : "<<beamEnergy<<std::endl;
  std::cout<<"n macroparticles : "<<numberToGenerate<<std::endl;
  std::cout<<"sigmaX           : "<<sigmaX<<std::endl;
  std::cout<<"Cerenkov on               : "<<turnOnCerenkov<<std::endl;
}

void Options::set_value(std::string name, double value )
{
#ifdef DEBUG
  std::cout << "parser> Setting value " << name << ", " << value << std::endl; 
#endif
  //
  // numeric options for the "beam" command
  //
  if(name == "elossHistoBinWidth") {elossHistoBinWidth = value; return;}
  if(name == "elossHistotransBinWidth") {elossHistoTransBinWidth = value; return;}
  if(name == "defaultRangeCut") {defaultRangeCut = value; return;}
  if(name == "ffact") {ffact = value; return;}
  if(name == "energy" ) {beamEnergy = value; return;}
  if(name == "X0" ) { X0 = value; return; }
  if(name == "Y0" ) { Y0 = value; return; }
  if(name == "Z0" ) { Z0 = value; return; }
  if(name == "T0" ) { T0 = value; return; }
  if(name == "Xp0" ) { Xp0 = value; return; }
  if(name == "Yp0" ) { Yp0 = value; return; }
  if(name == "Zp0" ) { Zp0 = value; return; }

  if(name == "sigmaT" ) { sigmaT = value; return; }
  if(name == "sigmaE" ) { sigmaE = value; return; }

  // options for beam distrType="gauss"
  if(name == "sigmaX" ) { sigmaX = value; return; }
  if(name == "sigmaY" ) { sigmaY = value; return; }
  if(name == "sigmaXp" ) { sigmaXp = value; return; }
  if(name == "sigmaYp" ) { sigmaYp = value; return; }

  // options for beam distrType="square" or distrType="circle"
  if(name == "envelopeX" ) { envelopeX = value; return; }
  if(name == "envelopeY" ) { envelopeY = value; return; }
  if(name == "envelopeXp" ) { envelopeXp = value; return; }
  if(name == "envelopeYp" ) { envelopeYp = value; return; }
  if(name == "envelopeT" ) { envelopeT = value; return; }
  if(name == "envelopeE" ) { envelopeE = value; return; }

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
  if(name == "shellX" ) { shellX = value; return; }
  if(name == "shellY" ) { shellY = value; return; }
  if(name == "shellXp" ) { shellXp = value; return; }
  if(name == "shellYp" ) { shellYp = value; return; }

  // options for beam distrType="ring"
  if(name == "Rmin" ) { Rmin = value; return; }
  if(name == "Rmax" ) { Rmax = value; return; }

  //
  // numeric options for the"option" command
  //

  // options for beam loss monitor geometry
  if(name == "blmRad" ) { blmRad = value; return; }
  if(name == "blmLength" ) { blmLength = value; return; }

  // options which influence the geometry
  if(name == "boxSize" ) {componentBoxSize = value; return; }
  if(name == "tunnelRadius" ) { tunnelRadius = value; return; }
  if(name == "beampipeThickness" ) { beampipeThickness = value; return; }
  if(name == "beampipeRadius" ) { beampipeRadius = value; return; }

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

  if(name == "synchRadOn") { 
    synchRadOn=(int)value;
    return; 
  }

  if(name == "decayOn") { 
    decayOn = (int)value; return; 
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
  if(name == "turnOnCerenkov") {
      turnOnCerenkov = (int)value; return;
  }

  if(name == "srRescale") {
      synchRescale = (int)value; return;
  }

  if(name == "srTrackPhotons") {
    synchTrackPhotons = (int)value; return;
  }

  if(name == "useEMLPB") { useEMLPB = value; return; }
  if(name == "useHadLPB") { useHadLPB = value; return; }
  if(name == "sensitiveBeamlineComponents") { sensitiveBeamlineComponents = value; return; }
  if(name == "sensitiveBeamPipe") { sensitiveBeamPipe = value; return; }
  if(name == "sensitiveBLMs") { sensitiveBLMs = value; return; }
  if(name == "LPBFraction") { LPBFraction = value; return; }
  if(name == "annihiToMuFe") { annihiToMuFe = value; return; }
  if(name == "gammaToMuFe") { gammaToMuFe = value; return; }
  if(name == "eeToHadronsFe") { eeToHadronsFe = value; return; }
  if(name == "thresholdCutCharged" ) { thresholdCutCharged = (double)value; return; }
  if(name == "thresholdCutPhotons" ) { thresholdCutPhotons = (double)value; return; }
  if(name == "vacuumPressure") { vacuumPressure = (double)value; return; }
  if(name == "planckScatterFe") { planckScatterFe = (double)value; return; }
  if(name == "stopTracks") { stopTracks = (int) value; return; } 
  if(name == "srLowX") { synchLowX = value; return; }
  if(name == "srLowGamE") { synchLowGamE = value; return; }
  if(name == "srMultiplicity") { synchPhotonMultiplicity = (int) value; return; }
  if(name == "srMeamFreeFactor") { synchMeanFreeFactor = (int) value; return; }

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
  if(name == "doTwiss" ) { doTwiss = (int) value; return; }
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
  
  std::cerr << "Error: parser> unkown option \"" << name << "\"" << std::endl; 
  exit(1);
}

void Options::set_value(std::string name, std::string value )
{
#ifdef DEBUG
  std::cout << "parser> Setting value " << name << ", " << value << std::endl; 
#endif
  // 
  // string options for the "beam" command
  //
  if(name == "particle") { particleName = value; return; }
  if(name == "distrType" ) { distribType = value; return; }
  if(name == "distrFile" ) { distribFile = getEnv("BDSIMPATH")+value; return; }

  //
  // string options for the "option" command
  //

  // options which influence the geometry
  if(name == "beampipeMaterial" ) { pipeMaterial = value; return; }
  if(name == "vacMaterial" ) { vacMaterial = value; return; }
  if(name == "tunnelMaterial" ) { tunnelMaterial = value; return; }
  if(name == "tunnelCavityMaterial" ) { tunnelCavityMaterial = value; return; }
  if(name == "soilMaterial" ) { soilMaterial = value; return; }
  
  // options which influence the tracking
  if(name == "physicsList" ) { physicsList = value; return; } 

  // options for external code interfaces
  if(name == "fifo") { fifo = value; return; }
  if(name == "refvolume") { refvolume = value; return; }
  std::cerr << "Error: parser.h> unkown option \"" << name << "\"" << std::endl; 
  exit(1);
}
