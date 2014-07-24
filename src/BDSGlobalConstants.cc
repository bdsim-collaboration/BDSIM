/** BDSIM, v0.4   

Last modified 23.10.2007 by Steve Malton

**/
#include "BDSGlobalConstants.hh"
#include "parser/options.h"
#include "BDSDebug.hh"
#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include <cstdlib>
#include "G4ThreeVector.hh"

extern Options options;

BDSGlobalConstants* BDSGlobalConstants::_instance = 0;

BDSGlobalConstants* BDSGlobalConstants::Instance(){
  if(_instance==0) {
    _instance = new BDSGlobalConstants(options);
  }
  return _instance;
}

BDSGlobalConstants::BDSGlobalConstants(struct Options& opt):
  itsBeamParticleDefinition(NULL),itsBeamMomentum(0.0),itsBeamKineticEnergy(0.0)
{
  itsPhysListName = opt.physicsList;
  itsPipeMaterial = opt.pipeMaterial;
  itsVacMaterial = opt.vacMaterial;
  itsTunnelMaterialName = opt.tunnelMaterial;
  itsTunnelCavityMaterialName = opt.tunnelCavityMaterial;
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
  itsBeamTotalEnergy = opt.beamEnergy * CLHEP::GeV;
  if (itsBeamTotalEnergy == 0) {
    G4cerr << __METHOD_NAME__ << "Error: option \"beamenergy\" is not defined or must be greater than 0" <<  G4endl;
    exit(1);
  }
  itsVacuumPressure = opt.vacuumPressure*CLHEP::bar;
  itsPlanckScatterFe = opt.planckScatterFe;
  //Fraction of events with leading particle biasing.
  itsBeampipeRadius = opt.beampipeRadius * CLHEP::m;
  if(itsBeampipeRadius == 0){
    G4cerr << __METHOD_NAME__ << "Error: option \"beampipeRadius\" must be greater than 0" <<  G4endl;
    exit(1);
  }
  itsBeampipeThickness = opt.beampipeThickness * CLHEP::m;
  itsComponentBoxSize = opt.componentBoxSize * CLHEP::m;
  if (itsComponentBoxSize < (itsBeampipeThickness + itsBeampipeRadius)){
    G4cerr << __METHOD_NAME__ << "Error: option \"boxSize\" must be greater than the sum of \"beampipeRadius\" and \"beamPipeThickness\" " << G4endl;
    exit(1);
  }
  itsBuildTunnel = opt.buildTunnel;
  itsBuildTunnelFloor = opt.buildTunnelFloor;  
  itsTunnelRadius = opt.tunnelRadius * CLHEP::m;
  if (itsTunnelRadius < itsComponentBoxSize/2){
    G4cerr << __METHOD_NAME__ << "> Error: option \"tunnelRadius\" must be grater than \"boxSize\"/2 " << G4endl;
    exit(1);
  }
  itsTunnelThickness = opt.tunnelThickness * CLHEP::m; //Tunnel geometry options read from file
  itsTunnelSoilThickness = opt.tunnelSoilThickness * CLHEP::m;
  itsTunnelFloorOffset = opt.tunnelFloorOffset * CLHEP::m;
  itsTunnelOffsetX = opt.tunnelOffsetX * CLHEP::m;
  itsTunnelOffsetY = opt.tunnelOffsetY * CLHEP::m;
  //Beam loss monitor (BLM) geometry
  itsBlmRad = opt.blmRad * CLHEP::m;
  itsBlmLength = opt.blmLength * CLHEP::m;
  //Sampler geometry - default diameter is the tunnel diameter
  if(opt.samplerDiameter==0){
    itsSamplerDiameter=2*itsTunnelRadius;
  } else {
    itsSamplerDiameter = opt.samplerDiameter * CLHEP::m;
  }
  itsSamplerLength = 4E-8 * CLHEP::m;
  itsThresholdCutCharged = opt.thresholdCutCharged * CLHEP::GeV;
  itsThresholdCutPhotons = opt.thresholdCutPhotons * CLHEP::GeV;
  itsProdCutPhotons = opt.prodCutPhotons * CLHEP::m;
  itsProdCutPhotonsP = opt.prodCutPhotonsP * CLHEP::m;
  itsProdCutPhotonsA = opt.prodCutPhotonsA * CLHEP::m;
  itsProdCutElectrons = opt.prodCutElectrons * CLHEP::m;
  itsProdCutElectronsP = opt.prodCutElectronsP * CLHEP::m;
  itsProdCutElectronsA = opt.prodCutElectronsA * CLHEP::m;
  itsProdCutPositrons = opt.prodCutPositrons * CLHEP::m;
  itsProdCutPositronsP = opt.prodCutPositronsP * CLHEP::m;
  itsProdCutPositronsA = opt.prodCutPositronsA * CLHEP::m;
  itsDeltaChord = opt.deltaChord * CLHEP::m;
  itsChordStepMinimum = opt.chordStepMinimum * CLHEP::m;
  itsDeltaIntersection= opt.deltaIntersection * CLHEP::m;
  itsMinimumEpsilonStep = opt.minimumEpsilonStep;
  itsMaximumEpsilonStep = opt.maximumEpsilonStep;
  itsMaxTime = opt.maximumTrackingTime * CLHEP::s;
  
  itsDeltaOneStep = opt.deltaOneStep * CLHEP::m;
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
  itsSynchLowGamE = opt.synchLowGamE * CLHEP::GeV;  // lowest gamma energy
  itsSynchPhotonMultiplicity = opt.synchPhotonMultiplicity;
  itsSynchMeanFreeFactor = opt.synchMeanFreeFactor;
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
  itsLaserwireWavelength = 0.532 * CLHEP::micrometer;
  itsLaserwireDir = G4ThreeVector(1,0,0);
  itsLaserwireTrackPhotons = 1;
  itsLaserwireTrackElectrons = 1;
  isWaitingForDump = false;
  isDumping = false;
  isReading = false;
  isReadFromStack = false;
  itsFifo = opt.fifo;
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << "itsFifo = " << itsFifo << G4endl;
  G4cout << __METHOD_NAME__ << "GetFifo() = " << GetFifo() << G4endl;
#endif
  itsRefVolume = opt.refvolume;
  itsRefCopyNo = opt.refcopyno;
  isReference = false;
  itsIncludeIronMagFields = opt.includeIronMagFields;
  zeroMagField = new G4UniformMagField(G4ThreeVector());
  itsZeroFieldManager=new G4FieldManager();
  itsZeroFieldManager->SetDetectorField(zeroMagField);
  itsZeroFieldManager->CreateChordFinder(zeroMagField);
  itsTurnsTaken = 1; //counting from 1
  if(opt.nturns < 1)
    {SetTurnsToTake(1);}
  else
    {SetTurnsToTake(opt.nturns);}  
  InitRotationMatrices();
  
  // options that are never used (no set method):
  itsLWCalWidth       = 0.0;
  itsLWCalOffset      = 0.0;
  itsMagnetPoleRadius = 0.0;
  itsMagnetPoleSize   = 0.0;
  teleporteroffset    = G4ThreeVector(0.,0.,0.);
  teleporterlength    = 0.0;
  // //Synchrotron primary generator
  // itsSynchPrimaryGen = false; //XXX check what this is 19/4/11
  // itsSynchPrimaryAngle = 0.0;
  // itsSynchPrimaryLength = 0.0;
}

void BDSGlobalConstants::InitRotationMatrices(){
  _RotY90       = new G4RotationMatrix();
  _RotYM90      = new G4RotationMatrix();
  _RotX90       = new G4RotationMatrix();
  _RotXM90      = new G4RotationMatrix();
  _RotYM90X90   = new G4RotationMatrix();
  _RotYM90XM90  = new G4RotationMatrix();
  G4double pi_ov_2 = asin(1.);
  _RotY90->rotateY(pi_ov_2);
  _RotYM90->rotateY(-pi_ov_2);
  _RotX90->rotateX(pi_ov_2);
  _RotXM90->rotateX(-pi_ov_2);
  _RotYM90X90->rotateY(-pi_ov_2);
  _RotYM90X90->rotateX( pi_ov_2);
  _RotYM90XM90->rotateY(-pi_ov_2);
  _RotYM90XM90->rotateX(-pi_ov_2);
}

//Methods to get the rotation matrices
G4RotationMatrix* BDSGlobalConstants::RotY90() const{
  return _RotY90;
}

G4RotationMatrix* BDSGlobalConstants::RotYM90() const{
  return _RotYM90;
}

G4RotationMatrix* BDSGlobalConstants::RotX90() const{
  return _RotX90;
}

G4RotationMatrix* BDSGlobalConstants::RotXM90() const{
  return _RotXM90;
}

G4RotationMatrix* BDSGlobalConstants::RotYM90X90() const{
  return _RotYM90X90;
}

G4RotationMatrix* BDSGlobalConstants::RotYM90XM90() const{
  return _RotYM90XM90;
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
  _instance = 0;
}
