/** BDSIM, v0.4   

Last modified 23.10.2007 by Steve Malton

**/
#include "BDSGlobalConstants.hh"
#include "../parser/getEnv.h"
#include "BDSDebug.hh"
#include "G4UniformMagField.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include <cstdlib>

#include <assert.h>
#include <string>
#include <stack>
#include <cmath>

BDSGlobalConstants* BDSGlobalConstants::_instance = 0;

BDSGlobalConstants* BDSGlobalConstants::Instance(){
  if(_instance==0) {
    _instance = new BDSGlobalConstants(options);
  }
  return _instance;
}

BDSGlobalConstants::BDSGlobalConstants(struct Options& opt):
  log(NULL), itsBeamParticleDefinition(NULL)
{
  //environment variables
  itsBDSIMHOME=(G4String)getEnv("BDSIMHOME");
  PI = 4.0 * atan(1.0);
  // defaults:
  if(opt.physicsList == "") 
    itsPhysListName = "standard";
  else
    itsPhysListName = opt.physicsList;
  if(opt.pipeMaterial == "") 
    itsPipeMaterial = "StainlessSteel";
  else
    itsPipeMaterial = opt.pipeMaterial;
  if(opt.vacMaterial == "") 
    itsVacMaterial = "Vacuum";
  else
    itsVacMaterial = opt.vacMaterial;

 if(opt.tunnelMaterial == "") 
    itsTunnelMaterialName = "concrete";
  else
    itsTunnelMaterialName = opt.tunnelMaterial;
 if(opt.tunnelCavityMaterial == "") 
    itsTunnelCavityMaterialName = "Air";
  else
    itsTunnelCavityMaterialName = opt.tunnelCavityMaterial;
 if(opt.soilMaterial == "") 
    itsSoilMaterialName = "soil";
  else
    itsSoilMaterialName = opt.soilMaterial;

 itsSampleDistRandomly = true;
 itsGeometryBias = opt.geometryBias;

 itsShowTunnel=opt.showTunnel;
 itsSensitiveComponents=opt.sensitiveBeamlineComponents;
 itsSensitiveBeamPipe=opt.sensitiveBeamPipe;
 itsSensitiveBLMs=opt.sensitiveBLMs;
 itsDefaultRangeCut=opt.defaultRangeCut;
 itsElossHistoBinWidth=opt.elossHistoBinWidth; //Longitudinal and transverse energy loss histogram bin widths
 itsElossHistoTransBinWidth=opt.elossHistoTransBinWidth;
  itsFFact=opt.ffact;
  itsParticleName=G4String(opt.particleName);
  itsBeamTotalEnergy = opt.beamEnergy * GeV;
  itsVacuumPressure = opt.vacuumPressure*bar;
  itsPlanckScatterFe = opt.planckScatterFe;
  //Fraction of events with leading particle biasing.
  itsBeampipeRadius = opt.beampipeRadius * m;
  if(itsBeampipeRadius == 0){
    G4cerr << __METHOD_NAME__ << "Error: option \"beampipeRadius\" must be greater than 0" <<  G4endl;
    exit(1);
  }
  itsBeampipeThickness = opt.beampipeThickness * m;
  itsComponentBoxSize = opt.componentBoxSize *m;
  if (itsComponentBoxSize < (itsBeampipeThickness + itsBeampipeRadius)){
    G4cerr << __METHOD_NAME__ << "Error: option \"boxSize\" must be greater than the sum of \"beampipeRadius\" and \"beamPipeThickness\" " << G4endl;
    exit(1);
  }
  itsBuildTunnel = opt.buildTunnel;
  itsBuildTunnelFloor = opt.buildTunnelFloor;  
  itsTunnelRadius = opt.tunnelRadius * m;
  if (itsTunnelRadius < itsComponentBoxSize/2){
    G4cerr << __METHOD_NAME__ << "> Error: option \"tunnelRadius\" must be grater than \"boxSize\"/2 " << G4endl;
    exit(1);
  }
  itsTunnelThickness = opt.tunnelThickness * m; //Tunnel geometry options read from file
  itsTunnelSoilThickness = opt.tunnelSoilThickness * m;
  itsTunnelFloorOffset = opt.tunnelFloorOffset * m;
  itsTunnelOffsetX = opt.tunnelOffsetX * m;
  itsTunnelOffsetY = opt.tunnelOffsetY * m;
  //Beam loss monitor (BLM) geometry
  itsBlmRad = opt.blmRad * m;
  itsBlmLength = opt.blmLength *m;
  //Sampler geometry - default diameter is the tunnel diameter
  if(opt.samplerDiameter==0){
    itsSamplerDiameter=2*itsTunnelRadius;
  } else {
    itsSamplerDiameter = opt.samplerDiameter * m;
  }
  itsSamplerLength = 1E-8 * m;
  itsThresholdCutCharged = opt.thresholdCutCharged * GeV;
  itsThresholdCutPhotons = opt.thresholdCutPhotons * GeV;
  itsProdCutPhotons = opt.prodCutPhotons * m;
  itsProdCutPhotonsP = opt.prodCutPhotonsP * m;
  itsProdCutElectrons = opt.prodCutElectrons * m;
  itsProdCutElectronsP = opt.prodCutElectronsP * m;
  itsProdCutPositrons = opt.prodCutPositrons * m;
  itsProdCutPositronsP = opt.prodCutPositronsP * m;
  itsDeltaChord = opt.deltaChord * m;
  itsChordStepMinimum = opt.chordStepMinimum * m;
  itsDeltaIntersection= opt.deltaIntersection * m;
  itsMinimumEpsilonStep = opt.minimumEpsilonStep;
  itsMaximumEpsilonStep = opt.maximumEpsilonStep;
  //  itsMaxTime = opt.maximumTrackingTime;
  itsMaxTime=2e-4*s;
  itsDeltaOneStep = opt.deltaOneStep * m;
  doTwiss = opt.doTwiss;
  itsDoPlanckScattering = opt.doPlanckScattering;
  itsCheckOverlaps = opt.checkOverlaps;
  itsTurnOnCerenkov = opt.turnOnCerenkov;
  itsSynchRadOn = opt.synchRadOn;
  G4cout << "BDSGlobalConstants::Instance() synchRadOn = " << itsSynchRadOn << G4endl;
  itsDecayOn = opt.decayOn;
  itsSynchRescale = opt.synchRescale; // rescale due to synchrotron
  itsSynchTrackPhotons= opt.synchTrackPhotons;
  G4cout << __METHOD_NAME__ << "synchTrackphotons = " << itsSynchTrackPhotons << G4endl;
  itsSynchLowX = opt.synchLowX;
  itsSynchLowGamE = opt.synchLowGamE * GeV;  // lowest gamma energy
  itsSynchPhotonMultiplicity = opt.synchPhotonMultiplicity;
  itsSynchMeanFreeFactor = opt.synchMeanFreeFactor;
  //Synchrotron primary generator
  itsSynchPrimaryGen = false; //XXX check what this is 19/4/11
  itsLengthSafety = opt.lengthSafety;
  itsNumberToGenerate = opt.numberToGenerate;
  itsNumberOfEventsPerNtuple = opt.numberOfEventsPerNtuple;
  itsEventNumberOffset = opt.eventNumberOffset;
  itsRandomSeed = opt.randomSeed;
  itsGammaToMuFe= opt.gammaToMuFe;
  itsAnnihiToMuFe= opt.annihiToMuFe;
  itsEeToHadronsFe=opt.eeToHadronsFe;
  itsUseEMLPB=opt.useEMLPB;
  itsUseHadLPB=opt.useHadLPB;
  itsDecayOn=opt.decayOn;
  SetLPBFraction(opt.LPBFraction);
  itsStoreMuonTrajectories = opt.storeMuonTrajectories;
  itsTrajCutGTZ = opt.trajCutGTZ;
  itsTrajCutLTR = opt.trajCutLTR;
  itsStoreNeutronTrajectories = opt.storeNeutronTrajectories;
  itsStoreTrajectory = opt.storeTrajectory;
  //G4cout<<"STOREA TRAJ = "<< itsStoreTrajectory<<G4endl;
  stopTracks = opt.stopTracks; 
  // defaults - parameters of the laserwire process
  itsLaserwireWavelength = 0.532 * micrometer;
  itsLaserwireDir = G4ThreeVector(1,0,0);
  itsLaserwireTrackPhotons = 1;
  itsLaserwireTrackElectrons = 1;
  isWaitingForDump = false;
  isDumping = false;
  isReading = false;
  isReadFromStack = false;
  itsFifo = opt.fifo;
  itsRefVolume = opt.refvolume;
  itsRefCopyNo = opt.refcopyno;
  isReference = 0;
  itsIncludeIronMagFields = opt.includeIronMagFields;
  zeroMagField = new G4UniformMagField(G4ThreeVector());
  itsZeroFieldManager=new G4FieldManager();
  itsZeroFieldManager->SetDetectorField(magField);
  itsZeroFieldManager->CreateChordFinder(magField);
   
}

// a robust compiler-invariant method to convert from integer to G4String
G4String BDSGlobalConstants::StringFromInt(G4int N) 
{
  if (N==0) return "0";
  G4int nLocal=N, nDigit=0, nMax=1;
  do { nDigit++;
      nMax*=10;} while(N > nMax-1);
  nMax/=10;
  G4String Cnum;
  do {Cnum+=StringFromDigit(nLocal/nMax);
      nLocal-= nLocal/nMax * nMax;
      nMax/=10;}   while(nMax>1);
  if(nMax!=0)Cnum+=StringFromDigit(nLocal/nMax);
  return Cnum;
}

// a robust compiler-invariant method to convert from digit to G4String
G4String BDSGlobalConstants::StringFromDigit(G4int N) 
{
  if(N<0 || N>9)
    G4Exception("Invalid Digit in BDSGlobalConstants::StringFromDigit", "-1", FatalException, "");
  G4String Cnum;
  if(N==0)Cnum="0";
  else if(N==1)Cnum="1";
  else if(N==2)Cnum="2";
  else if(N==3)Cnum="3";
  else if(N==4)Cnum="4";
  else if(N==5)Cnum="5";
  else if(N==6)Cnum="6";
  else if(N==7)Cnum="7";
  else if(N==8)Cnum="8";
  else if(N==9)Cnum="9"; 
  return Cnum;
}

BDSGlobalConstants::~BDSGlobalConstants()
{  
  delete itsZeroFieldManager;
  delete zeroMagField;
}
