/** BDSIM, v0.4

Last modified 30.10.2007 by Steve Malton

**/

//==============================================================

#ifndef BDSGlobalConstants_h
#define BDSGlobalConstants_h 

//#include <fstream>
#include <deque>
#include <map>

#include "G4ThreeVector.hh"
#include "G4String.hh"
#include "G4AffineTransform.hh"

#include "BDSParticle.hh"
#include "G4LogicalVolume.hh"
#include "BDSLogicalVolumeInfo.hh"

class G4FieldManager;
class G4ParticleDefinition;
class G4UniformMagField;

struct Options;

struct strCmp {
  G4bool operator()( const G4String s1, const G4String s2 ) const {
    return strcmp(s1,s2) < 0;}
};

/*
struct trackSort : public binary_function<G4Track*, G4Track*, G4bool>{
  G4bool operator()(G4Track* a, G4Track* b){
  return a->GetPosition().z() < b->GetPosition().z();}
};
*/

class BDSGlobalConstants 
{
public:
 
  static BDSGlobalConstants* Instance();
  ~BDSGlobalConstants();
  
  G4bool   GetDoPlanckScattering();
  G4bool   GetCheckOverlaps();
  G4double GetMinimumEpsilonStep();
  G4double GetMaximumEpsilonStep();
  G4double GetMaxTime();
  G4double GetDeltaOneStep();

  G4String StringFromInt(G4int anInt);
  G4String StringFromDigit(G4int anInt);

  G4ParticleDefinition* GetParticleDefinition();
  void     SetParticleDefinition(G4ParticleDefinition* aBeamParticleDefinition);
  G4String GetParticleName();
  void     SetParticleName(G4String aParticleName);

  G4double GetLPBFraction();
  void     SetLPBFraction(G4double val);

  G4double GetElossHistoBinWidth();
  G4double GetElossHistoTransBinWidth(); //The transverse (x,y) bin width
  G4double GetDefaultRangeCut();

  G4double GetFFact();

  G4double GetBeamKineticEnergy();
  void     SetBeamKineticEnergy(G4double val);
  G4double GetBeamTotalEnergy();
  void     SetBeamTotalEnergy(G4double val);
  G4double GetBeamMomentum();
  void     SetBeamMomentum(G4double val);
  G4double GetVacuumPressure();
  G4double GetPlanckScatterFe();

  G4double GetGammaToMuFe();
  G4double GetAnnihiToMuFe();
  G4double GetEeToHadronsFe();
  G4bool   GetSampleDistRandomly();
  G4bool   GetGeometryBias();
  G4bool   GetUseEMLPB();
  G4bool   GetUseHadLPB();
  // Booleans determining which types of components are sensitive
  G4bool   GetSensitiveComponents();
  G4bool   GetSensitiveBeamPipe();
  G4bool   GetSensitiveBLMs();
  G4double GetComponentBoxSize();
  G4double GetMagnetPoleSize();
  G4double GetMagnetPoleRadius();

  G4bool   GetBuildTunnel(); 
  G4bool   GetBuildTunnelFloor(); 
  G4bool   GetShowTunnel(); 
  G4double GetTunnelRadius(); 
  void     SetTunnelRadius(G4double radius); 
  G4double GetTunnelThickness(); 
  G4double GetTunnelSoilThickness(); 
  G4double GetTunnelFloorOffset(); 
  G4double GetTunnelOffsetX(); 
  G4double GetTunnelOffsetY(); 

  // Beam loss monitors
  G4double GetBlmRad();
  G4double GetBlmLength();

  G4double GetBeampipeRadius(); 
  G4double GetBeampipeThickness(); 

  G4double GetSamplerDiameter();
  G4double GetSamplerLength();

  G4double GetDeltaIntersection();
  G4double GetDeltaChord();
  G4double GetChordStepMinimum();

  G4double GetThresholdCutCharged();
  G4double GetThresholdCutPhotons();

  G4double GetProdCutPhotons();
  G4double GetProdCutPhotonsP();
  G4double GetProdCutPhotonsA();
  G4double GetProdCutElectrons();
  G4double GetProdCutElectronsP();
  G4double GetProdCutElectronsA();
  G4double GetProdCutPositrons();
  G4double GetProdCutPositronsP();
  G4double GetProdCutPositronsA();

  // Environment variables
  G4String GetBDSIMPATH();

  // Physical processes etc.

  G4String GetPhysListName();
  void     SetPhysListName(G4String val);
  G4bool   GetSynchRadOn();
  G4bool   GetDecayOn();
  G4bool   GetSynchTrackPhotons();
  void     SetSynchTrackPhotons(G4bool srTrackPhotons);
  G4double GetSynchLowX();
  G4double GetSynchLowGamE();
  G4int    GetSynchPhotonMultiplicity();
  G4int    GetSynchMeanFreeFactor();
  G4double GetLaserwireWavelength();
  G4ThreeVector GetLaserwireDir();

  // Use map to generate multiple laserwires with independent wavelength 
  // and direction
  G4double GetLaserwireWavelength(G4String aName); 
  G4ThreeVector GetLaserwireDir(G4String aName); 
  void     SetLaserwireWavelength(G4String aName, G4double aWavelength);
  void     SetLaserwireDir(G4String aName, G4ThreeVector aDirection);
  G4bool   GetLaserwireTrackPhotons();
  G4bool   GetLaserwireTrackElectrons();
  G4bool   GetTurnOnCerenkov();
  G4bool GetTurnOnOpticalAbsorption();
  G4bool GetTurnOnRayleighScattering();
  G4bool GetTurnOnMieScattering();
  G4bool GetTurnOnOpticalSurface();
  G4bool GetTurnOnBirksSaturation();
  G4double GetScintYieldFactor();

  G4bool   GetStoreMuonTrajectories();
  G4double GetTrajCutGTZ();
  G4double GetTrajCutLTR();
  G4bool   GetStoreNeutronTrajectories();
  G4bool   GetStoreTrajectory();
  G4bool   GetIncludeIronMagFields();
  G4bool   GetStopTracks();
  void     SetStopTracks(G4bool);
  G4bool stopTracks; // kill tracks after interactions

  G4double GetLengthSafety();
  G4long   GetRandomSeed();
  G4int    GetNumberToGenerate();
  G4int    GetNumberOfEventsPerNtuple();
  G4int    GetEventNumberOffset();
  G4FieldManager* GetZeroFieldManager();

  // G4bool   GetUseSynchPrimaryGen();
  // G4double GetSynchPrimaryAngle();
  // G4double GetSynchPrimaryLength();

  // AI : for placet synchronization
  void     setWaitingForDump(G4bool flag);
  G4bool   getWaitingForDump();
  void     setDumping(G4bool flag);
  G4bool   getDumping();
  void     setReading(G4bool flag);
  G4bool   getReading();
  void     setReadFromStack(G4bool flag);
  G4bool   getReadFromStack();
  G4String GetFifo();
  G4int    GetTurnsTaken();
  void     IncrementTurnNumber();
  void     ResetTurnNumber();
  G4int    GetTurnsToTake();
  void     SetTurnsToTake(G4int TurnsToTake);

  G4AffineTransform GetDumpTransform();
  void              SetDumpTransform(G4AffineTransform tf);

  G4String GetRefVolume();
  G4int    GetRefCopyNo();

  const G4AffineTransform* GetRefTransform();
  void                     SetRefTransform(G4AffineTransform& aTransform);

  G4double GetSMax();
  void     SetSMax(G4double);
  G4ThreeVector GetTeleporterDelta();
  void          SetTeleporterDelta(G4ThreeVector newteleporterdelta);
  void          SetTeleporterLength(G4double newteleporterlength);
  G4double      GetTeleporterLength(); 

  // for general info about a logical volume - extendable data class
  // nominally used to get s position for energy loss
  // get the info for a given logical volume pointer
  BDSLogicalVolumeInfo* GetLogicalVolumeInfo(G4LogicalVolume* logvolpointer);
  // get a pointer to the map of log vol infos
  std::map<G4LogicalVolume*,BDSLogicalVolumeInfo*>* LogicalVolumeInfo(); 
  // add a new set of info to the map
  void AddLogicalVolumeInfo(G4LogicalVolume* logvolpointer, BDSLogicalVolumeInfo* bdslogvolinfo);
  
  // SPM : temp filestream for placet to read and write
  //  std::ofstream fileDump;
  // ifstream fileRead; replaced with FILE* fifo in code for consistency with Placet. SPM

  std::deque<BDSParticle> holdingQueue;
  std::deque<BDSParticle> outputQueue;
  std::deque<BDSParticle> transformedQueue;

protected:
  BDSGlobalConstants(struct Options&);


private:
  static BDSGlobalConstants* _instance;

  G4UniformMagField* zeroMagField;

  G4double itsElossHistoBinWidth;
  G4double itsElossHistoTransBinWidth;
  G4double itsDefaultRangeCut;
  G4double itsFFact;

  // initial bunch parameters
  G4String itsParticleName;
  G4ParticleDefinition* itsBeamParticleDefinition;
  G4double itsBeamTotalEnergy;
  G4double itsBeamMomentum, itsBeamKineticEnergy;
  G4double itsLPBFraction;
  G4double itsVacuumPressure;
  G4double itsPlanckScatterFe;
  G4double itsGammaToMuFe;
  G4double itsAnnihiToMuFe;
  G4double itsEeToHadronsFe;
  G4bool   itsSampleDistRandomly;
  G4bool   itsGeometryBias;
  G4bool   itsUseEMLPB;
  G4bool   itsUseHadLPB;
  G4double itsMinimumEpsilonStep;
  G4double itsMaximumEpsilonStep;
  G4double itsMaxTime;
  G4double itsDeltaOneStep;
  G4double itsComponentBoxSize;
  G4double itsMagnetPoleSize;
  G4double itsMagnetPoleRadius;
  G4bool   itsBuildTunnel;
  G4bool   itsBuildTunnelFloor;
  G4double itsTunnelRadius;
  G4double itsTunnelThickness;
  G4double itsTunnelSoilThickness;
  G4double itsTunnelFloorOffset;
  G4double itsTunnelOffsetX;
  G4double itsTunnelOffsetY;
  G4bool   itsShowTunnel;
  //Booleans determining which types of components are sensitive
  G4bool   itsSensitiveComponents;
  G4bool   itsSensitiveBeamPipe;
  G4bool   itsSensitiveBLMs;
  //Beam loss monitor geometry
  G4double itsBlmRad;
  G4double itsBlmLength;
  G4double itsBeampipeRadius; 
  G4double itsBeampipeThickness; 
  G4double itsSamplerDiameter;
  G4double itsSamplerLength;
  G4double itsDeltaIntersection;
  G4double itsDeltaChord;
  G4double itsChordStepMinimum;
  G4double itsThresholdCutCharged;
  G4double itsThresholdCutPhotons;
  G4double itsProdCutPhotons;
  G4double itsProdCutPhotonsP;
  G4double itsProdCutPhotonsA;
  G4double itsProdCutElectrons;
  G4double itsProdCutElectronsP;
  G4double itsProdCutElectronsA;
  G4double itsProdCutPositrons;
  G4double itsProdCutPositronsP;
  G4double itsProdCutPositronsA;
  G4String itsBDSIMPATH;
  G4String itsPhysListName;
  G4bool   itsSynchRadOn;
  G4bool   itsDecayOn;
  G4bool   itsSynchTrackPhotons;
  G4double itsSynchLowX;
  G4double itsSynchLowGamE;
  G4int    itsSynchMeanFreeFactor;
  G4int    itsSynchPhotonMultiplicity;
  // test map container for laserwire parameters - Steve
  std::map<const G4String, G4double, strCmp> lwWavelength;
  std::map<const G4String, G4ThreeVector, strCmp> lwDirection;
  G4double itsLaserwireWavelength;
  G4ThreeVector itsLaserwireDir;
  G4bool   itsLaserwireTrackPhotons;
  G4bool   itsLaserwireTrackElectrons;
  G4bool   itsTurnOnCerenkov;
  G4bool   itsTurnOnOpticalAbsorption;
  G4bool   itsTurnOnRayleighScattering;
  G4bool   itsTurnOnMieScattering;
  G4bool   itsTurnOnOpticalSurface;
  G4bool   itsTurnOnBirksSaturation;
  G4double itsScintYieldFactor;
  G4bool   itsDoPlanckScattering;
  G4bool   itsCheckOverlaps;
  G4bool   itsStoreMuonTrajectories;
  G4double itsTrajCutGTZ;
  G4double itsTrajCutLTR;
  G4bool   itsStoreTrajectory;
  G4bool   itsStoreNeutronTrajectories;
  G4bool   itsIncludeIronMagFields;
  G4double itsLengthSafety;
  G4long   itsRandomSeed;
  G4int    itsNumberToGenerate;
  G4int    itsNumberOfEventsPerNtuple;
  G4int    itsEventNumberOffset;
  G4FieldManager* itsZeroFieldManager;
  // rotation
  void InitRotationMatrices();

  G4RotationMatrix* _RotY90;
  G4RotationMatrix* _RotYM90;
  G4RotationMatrix* _RotX90;
  G4RotationMatrix* _RotXM90;
  G4RotationMatrix* _RotYM90X90;
  G4RotationMatrix* _RotYM90XM90;

public:
  G4RotationMatrix* RotY90() const;
  G4RotationMatrix* RotYM90() const;
  G4RotationMatrix* RotX90() const;
  G4RotationMatrix* RotXM90() const;
  G4RotationMatrix* RotYM90X90() const;
  G4RotationMatrix* RotYM90XM90() const;

  G4double GetLWCalWidth();
  G4double GetLWCalOffset();
  G4String GetPipeMaterialName();
  G4String GetVacuumMaterial();
  G4String GetSoilMaterialName();
  G4String GetTunnelMaterialName();
  G4String GetTunnelCavityMaterialName();

private:
  G4double itsLWCalWidth;
  G4double itsLWCalOffset;
  G4String itsPipeMaterial;              //beampipe material
  G4String itsVacMaterial;               //vacuum inside beampipe
  G4String itsTunnelMaterialName;        //tunnel material
  G4String itsTunnelCavityMaterialName;  //tunnel cavity material
  G4String itsSoilMaterialName;          //material around tunnel
  // G4bool   itsSynchPrimaryGen;
  // G4double itsSynchPrimaryAngle;
  // G4double itsSynchPrimaryLength;
  G4bool   isWaitingForDump;
  G4bool   isDumping;
  G4bool   isReading;
  G4bool   isReadFromStack;
  G4String itsFifo; // fifo for BDSIM-placet
  G4AffineTransform itsDumpTransform; //transform of frame from start to current dump element
  G4String itsRefVolume;
  G4int    itsRefCopyNo;
  G4AffineTransform itsRefTransform;
  // Turn Control
  G4int    itsTurnsTaken;
  G4int    itsTurnsToTake;
  // Teleporter offset corrections
  G4ThreeVector teleporterdelta;
  G4double      teleporterlength;
  // beamline length
  G4double itsSMax;
  // logical volume info
  std::map<G4LogicalVolume* , BDSLogicalVolumeInfo*> logicalvolumeinfo;
};

inline G4double BDSGlobalConstants::GetElossHistoBinWidth()
{return itsElossHistoBinWidth;}

inline G4double BDSGlobalConstants::GetElossHistoTransBinWidth()
{return itsElossHistoTransBinWidth;}

inline G4double BDSGlobalConstants::GetDefaultRangeCut()
{return itsDefaultRangeCut;}

inline G4double BDSGlobalConstants::GetFFact()
{return itsFFact;}

inline G4double BDSGlobalConstants::GetMinimumEpsilonStep()
{return itsMinimumEpsilonStep;}

inline G4double BDSGlobalConstants::GetMaximumEpsilonStep()
{return itsMaximumEpsilonStep;}

inline G4double BDSGlobalConstants::GetMaxTime()
{return itsMaxTime;}

inline G4double BDSGlobalConstants::GetDeltaOneStep()
{return itsDeltaOneStep;}

inline G4double BDSGlobalConstants::GetBeamKineticEnergy()
{return itsBeamKineticEnergy;}

inline void BDSGlobalConstants::SetBeamKineticEnergy(G4double val)
{itsBeamKineticEnergy = val;}

inline G4double BDSGlobalConstants::GetLPBFraction()
{return itsLPBFraction;}

inline void BDSGlobalConstants::SetLPBFraction(G4double val)
{if(val>1.0)
    {itsLPBFraction = 1.0;}
  else if(val<0.0)
    {itsLPBFraction = 0.0;}
  else 
    {itsLPBFraction = val;}
}

inline G4double BDSGlobalConstants::GetBeamTotalEnergy()
{return itsBeamTotalEnergy;}

inline void BDSGlobalConstants::SetBeamTotalEnergy(G4double val)
{itsBeamTotalEnergy = val;}

inline G4double BDSGlobalConstants::GetBeamMomentum()
{return itsBeamMomentum;}

inline void BDSGlobalConstants::SetBeamMomentum(G4double val)
{itsBeamMomentum = val;}

inline G4ParticleDefinition* BDSGlobalConstants::GetParticleDefinition()
{return itsBeamParticleDefinition;}

inline G4String BDSGlobalConstants::GetParticleName()
{return itsParticleName;}

inline void BDSGlobalConstants::SetParticleDefinition(G4ParticleDefinition* aBeamParticleDefinition)
{itsBeamParticleDefinition = aBeamParticleDefinition;}

inline void BDSGlobalConstants::SetParticleName(G4String aParticleName)
{itsParticleName = aParticleName;}

inline G4double BDSGlobalConstants::GetVacuumPressure()
{return itsVacuumPressure;}

inline G4double BDSGlobalConstants::GetPlanckScatterFe()
{return itsPlanckScatterFe;}

inline G4double BDSGlobalConstants::GetGammaToMuFe()
{return itsGammaToMuFe;}

inline G4double BDSGlobalConstants::GetAnnihiToMuFe()
{return itsAnnihiToMuFe;}

inline G4double BDSGlobalConstants::GetEeToHadronsFe()
{return itsEeToHadronsFe;}

inline G4bool BDSGlobalConstants::GetSampleDistRandomly(){
  return itsSampleDistRandomly;}

inline G4bool BDSGlobalConstants::GetUseEMLPB(){
  return itsUseEMLPB;}

inline G4bool BDSGlobalConstants::GetUseHadLPB(){
  return itsUseHadLPB;}

//Booleans determining which types of components are sensitive
inline  G4bool BDSGlobalConstants::GetSensitiveComponents()
{return itsSensitiveComponents;}

inline  G4bool BDSGlobalConstants::GetSensitiveBeamPipe()
{return itsSensitiveBeamPipe;}

inline  G4bool BDSGlobalConstants::GetSensitiveBLMs()
{return itsSensitiveBLMs;}

inline G4double BDSGlobalConstants::GetComponentBoxSize()
{return itsComponentBoxSize;}

inline G4double BDSGlobalConstants::GetMagnetPoleSize()
{return itsMagnetPoleSize;}

inline G4double BDSGlobalConstants::GetMagnetPoleRadius()
{return itsMagnetPoleRadius;}

inline G4bool BDSGlobalConstants::GetBuildTunnel()
{return itsBuildTunnel;}

inline G4bool BDSGlobalConstants::GetBuildTunnelFloor()
{return itsBuildTunnelFloor;}

inline G4double BDSGlobalConstants::GetTunnelRadius()
{return itsTunnelRadius;}

inline void BDSGlobalConstants::SetTunnelRadius(G4double radius)
{itsTunnelRadius=radius;}

inline G4double BDSGlobalConstants::GetTunnelThickness()
{return itsTunnelThickness;}

inline G4double BDSGlobalConstants::GetTunnelSoilThickness()
{return itsTunnelSoilThickness;}

inline G4double BDSGlobalConstants::GetTunnelFloorOffset()
{return itsTunnelFloorOffset;}

inline G4double BDSGlobalConstants::GetTunnelOffsetX()
{return itsTunnelOffsetX;}

inline G4double BDSGlobalConstants::GetTunnelOffsetY()
{return itsTunnelOffsetY;}

inline G4bool BDSGlobalConstants::GetShowTunnel()
{return itsShowTunnel;}

inline G4bool BDSGlobalConstants::GetGeometryBias()
{return itsGeometryBias;}

//Beam loss monitors

inline G4double BDSGlobalConstants::GetBlmRad()
{return itsBlmRad;}

inline G4double BDSGlobalConstants::GetBlmLength()
{return itsBlmLength;}


// inline  G4double BDSGlobalConstants::GetHorizontalBeamlineOffset()
// {return itsHorizontalBeamlineOffset;}
// inline  G4double BDSGlobalConstants::GetVerticalBeamlineOffset()
// {return itsVerticalBeamlineOffset;}



inline G4double BDSGlobalConstants::GetBeampipeRadius() 
{return itsBeampipeRadius;}

inline G4double BDSGlobalConstants::GetBeampipeThickness() 
{return itsBeampipeThickness;}

inline G4double BDSGlobalConstants::GetSamplerDiameter() 
{return itsSamplerDiameter;}

inline G4double BDSGlobalConstants::GetSamplerLength() 
{return itsSamplerLength;}

inline G4double BDSGlobalConstants::GetDeltaChord() 
{return itsDeltaChord;}

inline G4double BDSGlobalConstants::GetDeltaIntersection() 
{return itsDeltaIntersection;}

inline G4double BDSGlobalConstants::GetChordStepMinimum() 
{return itsChordStepMinimum;}

inline G4double BDSGlobalConstants::GetThresholdCutCharged() 
{return itsThresholdCutCharged;}

inline G4double BDSGlobalConstants::GetThresholdCutPhotons() 
{return itsThresholdCutPhotons;}

inline G4double BDSGlobalConstants::GetProdCutPhotons() 
{return itsProdCutPhotons;}

inline G4double BDSGlobalConstants::GetProdCutPhotonsP() 
{return itsProdCutPhotonsP;}

inline G4double BDSGlobalConstants::GetProdCutPhotonsA() 
{return itsProdCutPhotonsA;}

inline G4double BDSGlobalConstants::GetProdCutElectrons() 
{return itsProdCutElectrons;}

inline G4double BDSGlobalConstants::GetProdCutElectronsP() 
{return itsProdCutElectronsP;}

inline G4double BDSGlobalConstants::GetProdCutElectronsA() 
{return itsProdCutElectronsA;}

inline G4double BDSGlobalConstants::GetProdCutPositrons() 
{return itsProdCutPositrons;}

inline G4double BDSGlobalConstants::GetProdCutPositronsP() 
{return itsProdCutPositronsP;}

inline G4double BDSGlobalConstants::GetProdCutPositronsA() 
{return itsProdCutPositronsA;}


//inline G4double BDSGlobalConstants::GetWorldSizeZ() 
//{return itsWorldSizeZ;}
//inline void BDSGlobalConstants::SetWorldSizeZ(G4double WorldSizeZ) 
//{itsWorldSizeZ=WorldSizeZ;}

// inline void BDSGlobalConstants::SetVerticalComponentOffset(G4double VerticalComponentOffset)
// {itsVerticalComponentOffset=VerticalComponentOffset;}
// inline void BDSGlobalConstants::SetHorizontalComponentOffset(G4double HorizontalComponentOffset)
// {itsHorizontalComponentOffset=HorizontalComponentOffset;}

// inline void BDSGlobalConstants::AddVerticalComponentOffset(G4double VerticalComponentOffset)
// {itsVerticalComponentOffset+=VerticalComponentOffset;}
// inline void BDSGlobalConstants::AddHorizontalComponentOffset(G4double HorizontalComponentOffset)
// {itsHorizontalComponentOffset+=HorizontalComponentOffset;}

// inline G4double BDSGlobalConstants::GetVerticalComponentOffset()
// {return itsVerticalComponentOffset;}
// inline G4double BDSGlobalConstants::GetHorizontalComponentOffset()
// {return itsHorizontalComponentOffset;}

inline G4String BDSGlobalConstants::GetBDSIMPATH()
{return itsBDSIMPATH;}

inline G4String BDSGlobalConstants::GetPhysListName()
{return itsPhysListName;}

inline void BDSGlobalConstants::SetPhysListName(G4String val)
{itsPhysListName = val;}

inline G4bool BDSGlobalConstants::GetSynchRadOn()
{return itsSynchRadOn;}

inline G4bool BDSGlobalConstants::GetDecayOn()
{return itsDecayOn;}

inline G4bool BDSGlobalConstants::GetSynchTrackPhotons()
{return itsSynchTrackPhotons ;}

inline void BDSGlobalConstants::SetSynchTrackPhotons(G4bool srTrackPhotons)
{itsSynchTrackPhotons=srTrackPhotons ;}

inline G4double BDSGlobalConstants::GetSynchLowX()
{return itsSynchLowX ;}

inline G4double BDSGlobalConstants::GetSynchLowGamE()
{return itsSynchLowGamE ;}

inline G4int BDSGlobalConstants::GetSynchPhotonMultiplicity()
{return itsSynchPhotonMultiplicity ;}

inline G4int BDSGlobalConstants::GetSynchMeanFreeFactor()
{return itsSynchMeanFreeFactor ;}

inline G4double BDSGlobalConstants::GetLaserwireWavelength()
{return itsLaserwireWavelength ;}

inline G4ThreeVector BDSGlobalConstants::GetLaserwireDir()
{return itsLaserwireDir ;}

inline G4bool BDSGlobalConstants::GetLaserwireTrackPhotons()
{return itsLaserwireTrackPhotons ;}

inline G4bool BDSGlobalConstants::GetLaserwireTrackElectrons()
{return itsLaserwireTrackElectrons ;}

// inline G4bool BDSGlobalConstants::GetReadBunchFile()
// {return itsReadBunchFile;}

// inline G4bool BDSGlobalConstants::GetExtractBunchFile()
// {return itsExtractBunchFile;}

// inline G4bool BDSGlobalConstants::GetWriteBunchFile()
// {return itsWriteBunchFile ;}

inline G4double BDSGlobalConstants::GetLengthSafety()
{return itsLengthSafety;}

inline G4bool BDSGlobalConstants::GetTurnOnCerenkov()
{return itsTurnOnCerenkov;}

inline  G4bool BDSGlobalConstants::GetTurnOnOpticalAbsorption()
{ return itsTurnOnOpticalAbsorption;}

inline  G4bool BDSGlobalConstants::GetTurnOnRayleighScattering()
{return itsTurnOnRayleighScattering;}

inline  G4bool BDSGlobalConstants::GetTurnOnMieScattering()
{return itsTurnOnMieScattering;}

inline  G4bool BDSGlobalConstants::GetTurnOnOpticalSurface()
{return itsTurnOnOpticalSurface;}

inline  G4bool BDSGlobalConstants::GetTurnOnBirksSaturation()
{return itsTurnOnBirksSaturation;}

inline  G4double BDSGlobalConstants::GetScintYieldFactor()
{return itsScintYieldFactor;}

inline G4bool BDSGlobalConstants::GetIncludeIronMagFields()
{return itsIncludeIronMagFields;}

inline G4bool BDSGlobalConstants::GetStoreMuonTrajectories()
{return itsStoreMuonTrajectories;}

inline G4double BDSGlobalConstants::GetTrajCutGTZ()
{return itsTrajCutGTZ;}

inline G4double BDSGlobalConstants::GetTrajCutLTR()
{return itsTrajCutLTR;}

inline G4bool BDSGlobalConstants::GetStoreNeutronTrajectories()
{return itsStoreNeutronTrajectories;}

inline G4bool BDSGlobalConstants::GetStoreTrajectory()
{return itsStoreTrajectory;}

inline G4bool BDSGlobalConstants::GetStopTracks()
{return stopTracks;}

inline void BDSGlobalConstants::SetStopTracks(G4bool val)
{stopTracks = val;}

inline G4long BDSGlobalConstants::GetRandomSeed()
{return itsRandomSeed;}

inline G4int BDSGlobalConstants::GetNumberToGenerate()
{return itsNumberToGenerate;}

inline G4int BDSGlobalConstants::GetNumberOfEventsPerNtuple()
{return itsNumberOfEventsPerNtuple;}

inline G4int BDSGlobalConstants::GetEventNumberOffset()
{return itsEventNumberOffset;}

inline G4FieldManager* BDSGlobalConstants::GetZeroFieldManager()
{return itsZeroFieldManager;}


inline  G4double BDSGlobalConstants::GetLWCalWidth()
{return itsLWCalWidth;}

inline  G4double BDSGlobalConstants::GetLWCalOffset()
{return itsLWCalOffset;}

//inline  G4String BDSGlobalConstants::GetLWCalMaterial()
//{return itsLWCalMaterial;}

inline G4String BDSGlobalConstants::GetPipeMaterialName()
{return itsPipeMaterial;}

inline G4String BDSGlobalConstants::GetVacuumMaterial()
{return itsVacMaterial;}

inline G4String BDSGlobalConstants::GetSoilMaterialName()
{return itsSoilMaterialName;}

inline G4String BDSGlobalConstants::GetTunnelMaterialName()
{return itsTunnelMaterialName;}

inline G4String BDSGlobalConstants::GetTunnelCavityMaterialName()
{return itsTunnelCavityMaterialName;}

// inline G4bool BDSGlobalConstants::GetUseSynchPrimaryGen()
// {return itsSynchPrimaryGen;}
// inline G4double BDSGlobalConstants::GetSynchPrimaryAngle()
// {return itsSynchPrimaryAngle;}
// inline G4double BDSGlobalConstants::GetSynchPrimaryLength()
// {return itsSynchPrimaryLength;}

inline G4bool BDSGlobalConstants::GetDoPlanckScattering() 
{return itsDoPlanckScattering;}

inline G4bool BDSGlobalConstants::GetCheckOverlaps() 
{return itsCheckOverlaps;}

//for map of laserwire wavelengths
inline G4double BDSGlobalConstants::GetLaserwireWavelength(G4String aName)
{return lwWavelength[aName];}

//for map of laserwire wavelengths
inline G4ThreeVector BDSGlobalConstants::GetLaserwireDir(G4String aName)
{return lwDirection[aName];}

inline void BDSGlobalConstants::SetLaserwireWavelength(G4String aName, G4double aWavelength)
{lwWavelength[aName]=aWavelength;}

inline void BDSGlobalConstants::SetLaserwireDir(G4String aName, G4ThreeVector aDirection)
{lwDirection[aName]=aDirection;}

inline void BDSGlobalConstants::setWaitingForDump(G4bool flag)
{isWaitingForDump = flag;} // waiting before all tracks arrive at a dump element

inline G4bool BDSGlobalConstants::getWaitingForDump() 
{return isWaitingForDump;}

inline void BDSGlobalConstants::setDumping(G4bool flag) 
{isDumping = flag;} // all tracks are pending - for stacking manager 

inline G4bool BDSGlobalConstants::getDumping()
{return isDumping;}

inline void BDSGlobalConstants::setReading(G4bool flag)
{isReading = flag;}

inline G4bool BDSGlobalConstants::getReading()
{return isReading;}

inline void BDSGlobalConstants::setReadFromStack(G4bool flag)
{isReadFromStack = flag;}

inline G4bool BDSGlobalConstants::getReadFromStack()
{return isReadFromStack;}

inline G4String BDSGlobalConstants::GetFifo()
{return itsFifo;}

inline G4AffineTransform BDSGlobalConstants::GetDumpTransform()
{return itsDumpTransform;}

inline void BDSGlobalConstants::SetDumpTransform(G4AffineTransform tf)
{itsDumpTransform=tf;}

inline G4String BDSGlobalConstants::GetRefVolume()
{return itsRefVolume;}

inline G4int BDSGlobalConstants::GetRefCopyNo()
{return itsRefCopyNo;}

inline const G4AffineTransform* BDSGlobalConstants::GetRefTransform()
{return &itsRefTransform;}

inline void BDSGlobalConstants::SetRefTransform(G4AffineTransform& aTransform)
{itsRefTransform=aTransform;}

inline G4int BDSGlobalConstants::GetTurnsTaken()
{return itsTurnsTaken;}

inline void  BDSGlobalConstants::IncrementTurnNumber()
{itsTurnsTaken += 1;}

inline void  BDSGlobalConstants::ResetTurnNumber()
{itsTurnsTaken = 1;}

inline G4int BDSGlobalConstants::GetTurnsToTake()
{return itsTurnsToTake;}

inline void  BDSGlobalConstants::SetTurnsToTake(G4int TurnsToTake)
{itsTurnsToTake = TurnsToTake;}

inline G4double BDSGlobalConstants::GetSMax() 
{return itsSMax;}

inline void BDSGlobalConstants::SetSMax(G4double smax)
{itsSMax=smax;}

inline G4ThreeVector BDSGlobalConstants::GetTeleporterDelta()
{return teleporterdelta;}

inline void BDSGlobalConstants::SetTeleporterDelta(G4ThreeVector newteleporterdelta)
{teleporterdelta = newteleporterdelta;}

inline void BDSGlobalConstants::SetTeleporterLength(G4double newteleporterlength)
{teleporterlength = newteleporterlength;}

inline G4double BDSGlobalConstants::GetTeleporterLength()
{return teleporterlength;}

inline BDSLogicalVolumeInfo* BDSGlobalConstants::GetLogicalVolumeInfo(G4LogicalVolume* logvolpointer)
{return logicalvolumeinfo[logvolpointer];}

inline void BDSGlobalConstants::AddLogicalVolumeInfo(G4LogicalVolume* logvolpointer, BDSLogicalVolumeInfo* logvolinfo)
{logicalvolumeinfo[logvolpointer] = logvolinfo;}

inline std::map<G4LogicalVolume*,BDSLogicalVolumeInfo*>* BDSGlobalConstants::LogicalVolumeInfo()
{return &logicalvolumeinfo;}

// UNUSED INLINE FUNCTIONS

//inline G4double BDSGlobalConstants::GetSigmaT()
//{return itsSigmaT;}

//inline G4double BDSGlobalConstants::GetWorldSizeZ() 
//{return itsWorldSizeZ;}

//inline void BDSGlobalConstants::SetWorldSizeZ(G4double WorldSizeZ) 
//{itsWorldSizeZ=WorldSizeZ;}

//inline void BDSGlobalConstants::SetVerticalComponentOffset(G4double VerticalComponentOffset)
//{itsVerticalComponentOffset=VerticalComponentOffset;}

//inline void BDSGlobalConstants::SetHorizontalComponentOffset(G4double HorizontalComponentOffset)
//{itsHorizontalComponentOffset=HorizontalComponentOffset;}

//inline void BDSGlobalConstants::AddVerticalComponentOffset(G4double VerticalComponentOffset)
//{itsVerticalComponentOffset+=VerticalComponentOffset;}

//inline void BDSGlobalConstants::AddHorizontalComponentOffset(G4double HorizontalComponentOffset)
//{itsHorizontalComponentOffset+=HorizontalComponentOffset;}

//inline G4double BDSGlobalConstants::GetVerticalComponentOffset()
//{return itsVerticalComponentOffset;}

//inline G4double BDSGlobalConstants::GetHorizontalComponentOffset()
//{return itsHorizontalComponentOffset;}

//inline  G4String BDSGlobalConstants::GetLWCalMaterial()
//{return itsLWCalMaterial;}

//inline G4bool BDSGlobalConstants::GetReadBunchFile()
//{return itsReadBunchFile;}

//inline G4bool BDSGlobalConstants::GetExtractBunchFile()
//{return itsExtractBunchFile;}

//inline G4bool BDSGlobalConstants::GetWriteBunchFile()
//{return itsWriteBunchFile ;}

//inline G4bool BDSGlobalConstants::GetUseSynchPrimaryGen()
//{return itsSynchPrimaryGen;}

//inline G4double BDSGlobalConstants::GetSynchPrimaryAngle()
//{return itsSynchPrimaryAngle;}

//inline G4double BDSGlobalConstants::GetSynchPrimaryLength()
//{return itsSynchPrimaryLength;}

//inline  G4double BDSGlobalConstants::GetHorizontalBeamlineOffset()
//{return itsHorizontalBeamlineOffset;}

//inline  G4double BDSGlobalConstants::GetVerticalBeamlineOffset()
//{return itsVerticalBeamlineOffset;}

#endif

