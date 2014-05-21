/** BDSIM, v0.4

L. Deacon
A class for counting the number of photons generated by scintillators.

**/

//==============================================================

#ifndef BDSPhotonCounter_h
#define BDSPhotonCounter_h 

//#include <fstream>
#include <deque>
#include <map>

#include "G4ThreeVector.hh"
#include "G4String.hh"
#include "G4AffineTransform.hh"

#include "BDSParticle.hh"

class G4FieldManager;
class G4ParticleDefinition;
class G4UniformMagField;

struct Options;

struct strCmp {
  G4bool operator()( const G4String s1, const G4String s2 ) const {
    return strcmp(s1,s2) < 0;}
};

class BDSPhotonCounter 
{
public:
 
  static BDSPhotonCounter* Instance();
  ~BDSPhotonCounter();
  
  G4double GetPI();

  G4bool GetDoPlanckScattering();

  G4bool GetCheckOverlaps();

  G4bool DoTwiss();
  void   SetDoTwiss(G4bool val); 


  G4double GetMinimumEpsilonStep();
  G4double GetMaximumEpsilonStep();
  G4double GetMaxTime();
  G4double GetDeltaOneStep();

  G4String StringFromInt(G4int anInt);
  G4String StringFromDigit(G4int anInt);

  G4ParticleDefinition* GetParticleDefinition();
  void SetParticleDefinition(G4ParticleDefinition* aBeamParticleDefinition);

  G4String GetParticleName();
  void     SetParticleName(G4String aParticleName);

  G4double GetLPBFraction();
  void     SetLPBFraction(G4double val);

  G4double GetElossHistoBinWidth();
  G4double GetElossHistoTransBinWidth(); //The transverse (x,y) bin width
  G4double GetDefaultRangeCut();

  G4double GetFFact();

  G4double GetBeamKineticEnergy();
  void SetBeamKineticEnergy(G4double val);

  G4double GetBeamTotalEnergy();
  void SetBeamTotalEnergy(G4double val);

  G4double GetBeamMomentum();
  void SetBeamMomentum(G4double val);
  
  G4double GetVacuumPressure();
  G4double GetPlanckScatterFe();

  G4double GetGammaToMuFe();
  G4double GetAnnihiToMuFe();
  G4double GetEeToHadronsFe();

  G4bool GetSampleDistRandomly();
  G4bool GetGeometryBias();
  G4bool GetUseEMLPB();
  G4bool GetUseHadLPB();

  // Booleans determining which types of components are sensitive
  G4bool GetSensitiveComponents();
  G4bool GetSensitiveBeamPipe();
  G4bool GetSensitiveBLMs();
 
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
  G4String GetBDSIMHOME();

  // Physical processes etc.

  G4String GetPhysListName();
  void SetPhysListName(G4String val);

  G4bool GetSynchRadOn();
  G4bool GetDecayOn();
  G4bool GetSynchRescale();
  void SetSynchRescale(G4bool srRescale);
  G4bool GetSynchTrackPhotons();
  void SetSynchTrackPhotons(G4bool srTrackPhotons);
  G4double GetSynchLowX();
  G4double GetSynchLowGamE();
  G4int GetSynchPhotonMultiplicity();
  G4int GetSynchMeanFreeFactor();

  G4double GetLaserwireWavelength();
  G4ThreeVector GetLaserwireDir();

  // Use map to generate multiple laserwires with independent wavelength 
  // and direction
  G4double      GetLaserwireWavelength(G4String aName); 
  G4ThreeVector GetLaserwireDir(G4String aName); 
  void          SetLaserwireWavelength(G4String aName, G4double aWavelength);
  void          SetLaserwireDir(G4String aName, G4ThreeVector aDirection);

  G4bool GetLaserwireTrackPhotons();
  G4bool GetLaserwireTrackElectrons();

  G4bool GetTurnOnCerenkov();
  G4bool GetTurnOnOpticalAbsorption();
  G4bool GetTurnOnMieScattering();
  G4bool GetTurnOnRayleighScattering();
  G4bool GetTurnOnOpticalSurface();
  G4bool GetTurnOnBirksSaturation();

  G4double GetScintYieldFactor();

  G4bool   GetStoreMuonTrajectories();
  G4double GetTrajCutGTZ();
  G4double GetTrajCutLTR();

  G4bool GetStoreNeutronTrajectories();
  G4bool GetStoreTrajectory();

  G4bool GetIncludeIronMagFields();

  G4bool GetStopTracks();
  void SetStopTracks(G4bool);
  G4bool stopTracks; // kill tracks after interactions

  G4double GetLengthSafety();

  G4long GetRandomSeed();
  G4int GetNumberToGenerate();

  G4int GetNumberOfEventsPerNtuple();

  G4int GetEventNumberOffset();



  G4FieldManager* GetZeroFieldManager();

  // G4bool   GetUseSynchPrimaryGen();
  // G4double GetSynchPrimaryAngle();
  // G4double GetSynchPrimaryLength();

  // AI : for placet synchronization
  void   setWaitingForDump(G4bool flag);
  G4bool getWaitingForDump();

  void   setDumping(G4bool flag);
  G4bool getDumping();

  void   setReading(G4bool flag);
  G4bool getReading();

  void   setReadFromStack(G4bool flag);
  G4bool getReadFromStack();

  G4String GetFifo();

  G4AffineTransform GetDumpTransform();
  void              SetDumpTransform(G4AffineTransform tf);

  G4String GetRefVolume();
  G4int    GetRefCopyNo();

  const G4AffineTransform* GetRefTransform();
  void                     SetRefTransform(G4AffineTransform& aTransform);

  // SPM : temp filestream for placet to read and write
  //  std::ofstream fileDump;
  // ifstream fileRead; replaced with FILE* fifo in code for consistency with Placet. SPM

  std::deque<BDSParticle> holdingQueue;
  std::deque<BDSParticle> outputQueue;
  std::deque<BDSParticle> transformedQueue;
  std::deque<G4double*> referenceQueue;
  /// particles are reference bunches
  G4bool isReference;

protected:
  BDSPhotonCounter(struct Options&);


private:
  static BDSPhotonCounter* _instance;

  G4UniformMagField* zeroMagField;

  G4double itsElossHistoBinWidth;
  G4double itsElossHistoTransBinWidth;
  G4double itsDefaultRangeCut;
  G4double itsFFact;
  G4double PI;
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
  G4bool itsSampleDistRandomly;
  G4bool itsGeometryBias;
  G4bool itsUseEMLPB;
  G4bool itsUseHadLPB;
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
  G4bool itsShowTunnel;
  //Booleans determining which types of components are sensitive
  G4bool itsSensitiveComponents;
  G4bool itsSensitiveBeamPipe;
  G4bool itsSensitiveBLMs;
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
  G4String itsBDSIMHOME;
  G4String itsPhysListName;
  G4bool itsSynchRadOn;
  G4bool itsDecayOn;
  G4bool itsSynchRescale;
  G4bool itsSynchTrackPhotons;
  G4double itsSynchLowX;
  G4double itsSynchLowGamE;
  G4int itsSynchMeanFreeFactor;
  G4int itsSynchPhotonMultiplicity;
  // test map container for laserwire parameters - Steve
  std::map<const G4String, G4double, strCmp> lwWavelength;
  std::map<const G4String, G4ThreeVector, strCmp> lwDirection;
  G4double itsLaserwireWavelength;
  G4ThreeVector itsLaserwireDir;
  G4bool itsLaserwireTrackPhotons;
  G4bool itsLaserwireTrackElectrons;
  G4bool itsTurnOnCerenkov;
  G4bool itsTurnOnOpticalAbsorption;
  G4bool itsTurnOnMieScattering;
  G4bool itsTurnOnRayleighScattering;
  G4bool itsTurnOnOpticalSurface;
  G4bool itsTurnOnBirksSaturation;
  G4double itsScintYieldFactor;
  G4bool doTwiss;
  G4bool itsDoPlanckScattering;
  G4bool itsCheckOverlaps;
  G4bool itsStoreMuonTrajectories;
  G4double itsTrajCutGTZ;
  G4double itsTrajCutLTR;
  G4bool itsStoreTrajectory;
  G4bool itsStoreNeutronTrajectories;
  G4bool itsIncludeIronMagFields;
  G4double itsLengthSafety;
  G4long itsRandomSeed;
  G4int itsNumberToGenerate;
  G4int itsNumberOfEventsPerNtuple;
  G4int itsEventNumberOffset;
  G4FieldManager* itsZeroFieldManager;

private:
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
  G4String itsPipeMaterial; //beampipe material
  G4String itsVacMaterial;  //vacuum inside beampipe
  G4String itsTunnelMaterialName;  //tunnel material
  G4String itsTunnelCavityMaterialName;  //tunnel cavity material
  G4String itsSoilMaterialName;  //material around tunnel
  // G4bool itsSynchPrimaryGen;
  // G4double itsSynchPrimaryAngle;
  // G4double itsSynchPrimaryLength;
  G4bool isWaitingForDump;
  G4bool isDumping;
  G4bool isReading;
  G4bool isReadFromStack;
  G4String itsFifo; // fifo for BDSIM-placet
  G4AffineTransform itsDumpTransform; //transform of frame from start to current dump element
  G4String itsRefVolume;
  G4int itsRefCopyNo;
  G4AffineTransform itsRefTransform;
};

inline G4double BDSPhotonCounter::GetElossHistoBinWidth(){
  return itsElossHistoBinWidth;
}

inline G4double BDSPhotonCounter::GetElossHistoTransBinWidth(){
  return itsElossHistoTransBinWidth;
}

inline G4double BDSPhotonCounter::GetDefaultRangeCut(){
  return itsDefaultRangeCut;
}

inline G4double BDSPhotonCounter::GetFFact(){
  return itsFFact;
}

inline G4double BDSPhotonCounter::GetPI()
{
  return PI;
}


inline G4double BDSPhotonCounter::GetMinimumEpsilonStep()
{
  return itsMinimumEpsilonStep;
}

inline G4double BDSPhotonCounter::GetMaximumEpsilonStep()
{
  return itsMaximumEpsilonStep;
}

inline G4double BDSPhotonCounter::GetMaxTime()
{
  return itsMaxTime;
}

inline G4double BDSPhotonCounter::GetDeltaOneStep()
{
  return itsDeltaOneStep;
}

inline G4double BDSPhotonCounter::GetBeamKineticEnergy()
{
  return itsBeamKineticEnergy;
}

inline void BDSPhotonCounter::SetBeamKineticEnergy(G4double val)
{
  itsBeamKineticEnergy = val;
}

inline G4double BDSPhotonCounter::GetLPBFraction()
{
  return itsLPBFraction;
}

inline void BDSPhotonCounter::SetLPBFraction(G4double val)
{
  if(val>1.0){
    itsLPBFraction = 1.0;
  }
  else if(val<0.0){
    itsLPBFraction = 0.0;
  }
  else {
    itsLPBFraction = val;
  }
}

inline G4double BDSPhotonCounter::GetBeamTotalEnergy()
{
  return itsBeamTotalEnergy;
}

inline void BDSPhotonCounter::SetBeamTotalEnergy(G4double val)
{
  itsBeamTotalEnergy = val;
}


inline G4double BDSPhotonCounter::GetBeamMomentum()
{
  return itsBeamMomentum;
}

inline void BDSPhotonCounter::SetBeamMomentum(G4double val)
{
  itsBeamMomentum = val;
}


inline G4ParticleDefinition* BDSPhotonCounter::GetParticleDefinition()
{
  return itsBeamParticleDefinition;
}

inline G4String BDSPhotonCounter::GetParticleName()
{
  return itsParticleName;
}

inline void BDSPhotonCounter::SetParticleDefinition(G4ParticleDefinition* aBeamParticleDefinition)
{
  itsBeamParticleDefinition = aBeamParticleDefinition;
}

inline void BDSPhotonCounter::SetParticleName(G4String aParticleName)
{
  itsParticleName = aParticleName;
}

//inline G4double BDSPhotonCounter::GetSigmaT()
//{
//  return itsSigmaT;
//}

inline G4double BDSPhotonCounter::GetVacuumPressure()
{return itsVacuumPressure;}

inline G4double BDSPhotonCounter::GetPlanckScatterFe()
{return itsPlanckScatterFe;}

inline G4double BDSPhotonCounter::GetGammaToMuFe()
{return itsGammaToMuFe;}

inline G4double BDSPhotonCounter::GetAnnihiToMuFe()
{return itsAnnihiToMuFe;}

inline G4double BDSPhotonCounter::GetEeToHadronsFe()
{return itsEeToHadronsFe;}

inline G4bool BDSPhotonCounter::GetSampleDistRandomly(){
  return itsSampleDistRandomly;}

inline G4bool BDSPhotonCounter::GetUseEMLPB(){
  return itsUseEMLPB;}

inline G4bool BDSPhotonCounter::GetUseHadLPB(){
  return itsUseHadLPB;}

//Booleans determining which types of components are sensitive
inline  G4bool BDSPhotonCounter::GetSensitiveComponents()
{return itsSensitiveComponents;}
inline  G4bool BDSPhotonCounter::GetSensitiveBeamPipe()
{return itsSensitiveBeamPipe;}
inline  G4bool BDSPhotonCounter::GetSensitiveBLMs()
{return itsSensitiveBLMs;}

inline G4double BDSPhotonCounter::GetComponentBoxSize()
{return itsComponentBoxSize;}

inline G4double BDSPhotonCounter::GetMagnetPoleSize()
{return itsMagnetPoleSize;}

inline G4double BDSPhotonCounter::GetMagnetPoleRadius()
{return itsMagnetPoleRadius;}

inline G4bool BDSPhotonCounter::GetBuildTunnel()
{return itsBuildTunnel;}

inline G4bool BDSPhotonCounter::GetBuildTunnelFloor()
{return itsBuildTunnelFloor;}

inline G4double BDSPhotonCounter::GetTunnelRadius()
{return itsTunnelRadius;}

inline void BDSPhotonCounter::SetTunnelRadius(G4double radius)
{itsTunnelRadius=radius;}

inline G4double BDSPhotonCounter::GetTunnelThickness()
{return itsTunnelThickness;}

inline G4double BDSPhotonCounter::GetTunnelSoilThickness()
{return itsTunnelSoilThickness;}

inline G4double BDSPhotonCounter::GetTunnelFloorOffset()
{return itsTunnelFloorOffset;}

inline G4double BDSPhotonCounter::GetTunnelOffsetX()
{return itsTunnelOffsetX;}

inline G4double BDSPhotonCounter::GetTunnelOffsetY()
{return itsTunnelOffsetY;}

inline G4bool BDSPhotonCounter::GetShowTunnel()
{return itsShowTunnel;}

inline G4bool BDSPhotonCounter::GetGeometryBias()
{return itsGeometryBias;}

//Beam loss monitors

inline G4double BDSPhotonCounter::GetBlmRad()
{return itsBlmRad;}

inline G4double BDSPhotonCounter::GetBlmLength()
{return itsBlmLength;}


// inline  G4double BDSPhotonCounter::GetHorizontalBeamlineOffset()
// {return itsHorizontalBeamlineOffset;}
// inline  G4double BDSPhotonCounter::GetVerticalBeamlineOffset()
// {return itsVerticalBeamlineOffset;}



inline G4double BDSPhotonCounter::GetBeampipeRadius() 
{return itsBeampipeRadius;}

inline G4double BDSPhotonCounter::GetBeampipeThickness() 
{return itsBeampipeThickness;}

inline G4double BDSPhotonCounter::GetSamplerDiameter() 
{return itsSamplerDiameter;}

inline G4double BDSPhotonCounter::GetSamplerLength() 
{return itsSamplerLength;}

inline G4double BDSPhotonCounter::GetDeltaChord() 
{return itsDeltaChord;}

inline G4double BDSPhotonCounter::GetDeltaIntersection() 
{return itsDeltaIntersection;}

inline G4double BDSPhotonCounter::GetChordStepMinimum() 
{return itsChordStepMinimum;}

inline G4double BDSPhotonCounter::GetThresholdCutCharged() 
{return itsThresholdCutCharged;}
inline G4double BDSPhotonCounter::GetThresholdCutPhotons() 
{return itsThresholdCutPhotons;}

inline G4double BDSPhotonCounter::GetProdCutPhotons() 
{return itsProdCutPhotons;}
inline G4double BDSPhotonCounter::GetProdCutPhotonsP() 
{return itsProdCutPhotonsP;}
inline G4double BDSPhotonCounter::GetProdCutPhotonsA() 
{return itsProdCutPhotonsA;}
inline G4double BDSPhotonCounter::GetProdCutElectrons() 
{return itsProdCutElectrons;}
inline G4double BDSPhotonCounter::GetProdCutElectronsP() 
{return itsProdCutElectronsP;}
inline G4double BDSPhotonCounter::GetProdCutElectronsA() 
{return itsProdCutElectronsA;}
inline G4double BDSPhotonCounter::GetProdCutPositrons() 
{return itsProdCutPositrons;}
inline G4double BDSPhotonCounter::GetProdCutPositronsP() 
{return itsProdCutPositronsP;}
inline G4double BDSPhotonCounter::GetProdCutPositronsA() 
{return itsProdCutPositronsA;}


//inline G4double BDSPhotonCounter::GetWorldSizeZ() 
//{return itsWorldSizeZ;}
//inline void BDSPhotonCounter::SetWorldSizeZ(G4double WorldSizeZ) 
//{itsWorldSizeZ=WorldSizeZ;}

// inline void BDSPhotonCounter::SetVerticalComponentOffset(G4double VerticalComponentOffset)
// {itsVerticalComponentOffset=VerticalComponentOffset;}
// inline void BDSPhotonCounter::SetHorizontalComponentOffset(G4double HorizontalComponentOffset)
// {itsHorizontalComponentOffset=HorizontalComponentOffset;}

// inline void BDSPhotonCounter::AddVerticalComponentOffset(G4double VerticalComponentOffset)
// {itsVerticalComponentOffset+=VerticalComponentOffset;}
// inline void BDSPhotonCounter::AddHorizontalComponentOffset(G4double HorizontalComponentOffset)
// {itsHorizontalComponentOffset+=HorizontalComponentOffset;}

// inline G4double BDSPhotonCounter::GetVerticalComponentOffset()
// {return itsVerticalComponentOffset;}
// inline G4double BDSPhotonCounter::GetHorizontalComponentOffset()
// {return itsHorizontalComponentOffset;}

inline G4String BDSPhotonCounter::GetBDSIMHOME()
{
  return itsBDSIMHOME;
}

inline G4String BDSPhotonCounter::GetPhysListName()
{
  return itsPhysListName;
}

inline void BDSPhotonCounter::SetPhysListName(G4String val)
{
  itsPhysListName = val;
}

inline G4bool BDSPhotonCounter::GetSynchRadOn()
{return itsSynchRadOn;}

inline G4bool BDSPhotonCounter::GetDecayOn()
{return itsDecayOn;}

inline G4bool BDSPhotonCounter::GetSynchRescale()
{return itsSynchRescale;}

inline void BDSPhotonCounter::SetSynchRescale(G4bool srRescale)
{itsSynchRescale = srRescale;}

inline G4bool BDSPhotonCounter::GetSynchTrackPhotons()
{return itsSynchTrackPhotons ;}

inline void BDSPhotonCounter::SetSynchTrackPhotons(G4bool srTrackPhotons)
{itsSynchTrackPhotons=srTrackPhotons ;}

inline G4double BDSPhotonCounter::GetSynchLowX()
{return itsSynchLowX ;}

inline G4double BDSPhotonCounter::GetSynchLowGamE()
{return itsSynchLowGamE ;}

inline G4int BDSPhotonCounter::GetSynchPhotonMultiplicity()
{return itsSynchPhotonMultiplicity ;}

inline G4int BDSPhotonCounter::GetSynchMeanFreeFactor()
{return itsSynchMeanFreeFactor ;}

inline G4double BDSPhotonCounter::GetLaserwireWavelength()
{return itsLaserwireWavelength ;}

inline G4ThreeVector BDSPhotonCounter::GetLaserwireDir()
{return itsLaserwireDir ;}

inline G4bool BDSPhotonCounter::GetLaserwireTrackPhotons()
{return itsLaserwireTrackPhotons ;}

inline G4bool BDSPhotonCounter::GetLaserwireTrackElectrons()
{return itsLaserwireTrackElectrons ;}

// inline G4bool BDSPhotonCounter::GetReadBunchFile()
// {return itsReadBunchFile;}

// inline G4bool BDSPhotonCounter::GetExtractBunchFile()
// {return itsExtractBunchFile;}

// inline G4bool BDSPhotonCounter::GetWriteBunchFile()
// {return itsWriteBunchFile ;}

inline G4double BDSPhotonCounter::GetLengthSafety()
{return itsLengthSafety;}

inline G4bool BDSPhotonCounter::GetTurnOnCerenkov()
{return itsTurnOnCerenkov;}
inline G4bool BDSPhotonCounter::GetTurnOnOpticalAbsorption()
{return itsTurnOnOpticalAbsorption;}
inline G4bool BDSPhotonCounter::GetTurnOnMieScattering()
{return itsTurnOnMieScattering;}
inline G4bool BDSPhotonCounter::GetTurnOnRayleighScattering()
{return itsTurnOnRayleighScattering;}
inline G4bool BDSPhotonCounter::GetTurnOnOpticalSurface()
{return itsTurnOnOpticalSurface;}
inline G4bool BDSPhotonCounter::GetTurnOnBirksSaturation()
{return itsTurnOnBirksSaturation;}

inline G4double BDSPhotonCounter::GetScintYieldFactor()
{return itsScintYieldFactor;}

inline G4bool BDSPhotonCounter::GetIncludeIronMagFields()
{return itsIncludeIronMagFields;}


inline G4bool BDSPhotonCounter::GetStoreMuonTrajectories()
{return itsStoreMuonTrajectories;}

inline G4double BDSPhotonCounter::GetTrajCutGTZ(){ return itsTrajCutGTZ; }
inline G4double BDSPhotonCounter::GetTrajCutLTR(){ return itsTrajCutLTR; }

inline G4bool BDSPhotonCounter::GetStoreNeutronTrajectories()
{return itsStoreNeutronTrajectories;}

inline G4bool BDSPhotonCounter::GetStoreTrajectory()
{return itsStoreTrajectory;}

inline G4bool BDSPhotonCounter::GetStopTracks()
{ return stopTracks; }

inline void BDSPhotonCounter::SetStopTracks(G4bool val)
{ stopTracks = val; }

inline G4long BDSPhotonCounter::GetRandomSeed()
{return itsRandomSeed;}
inline G4int BDSPhotonCounter::GetNumberToGenerate()
{return itsNumberToGenerate;}

inline G4int BDSPhotonCounter::GetNumberOfEventsPerNtuple()
{return itsNumberOfEventsPerNtuple;}

inline G4int BDSPhotonCounter::GetEventNumberOffset()
{return itsEventNumberOffset;}

inline G4FieldManager* BDSPhotonCounter::GetZeroFieldManager()
{return itsZeroFieldManager;}


inline  G4double BDSPhotonCounter::GetLWCalWidth()
{return itsLWCalWidth;}

inline  G4double BDSPhotonCounter::GetLWCalOffset()
{return itsLWCalOffset;}

//inline  G4String BDSPhotonCounter::GetLWCalMaterial()
//{return itsLWCalMaterial;}

inline G4String BDSPhotonCounter::GetPipeMaterialName()
{return itsPipeMaterial;}

inline G4String BDSPhotonCounter::GetVacuumMaterial()
{return itsVacMaterial;}

inline G4String BDSPhotonCounter::GetSoilMaterialName()
{return itsSoilMaterialName;}

inline G4String BDSPhotonCounter::GetTunnelMaterialName()
{return itsTunnelMaterialName;}

inline G4String BDSPhotonCounter::GetTunnelCavityMaterialName()
{return itsTunnelCavityMaterialName;}

// inline G4bool BDSPhotonCounter::GetUseSynchPrimaryGen()
// {return itsSynchPrimaryGen;}
// inline G4double BDSPhotonCounter::GetSynchPrimaryAngle()
// {return itsSynchPrimaryAngle;}
// inline G4double BDSPhotonCounter::GetSynchPrimaryLength()
// {return itsSynchPrimaryLength;}

inline G4bool BDSPhotonCounter::DoTwiss() 
{return doTwiss;}

inline void BDSPhotonCounter::SetDoTwiss(G4bool val) 
{doTwiss = val;}

inline G4bool BDSPhotonCounter::GetDoPlanckScattering() 
{return itsDoPlanckScattering;}

inline G4bool BDSPhotonCounter::GetCheckOverlaps() 
{return itsCheckOverlaps;}

//for map of laserwire wavelengths
inline G4double BDSPhotonCounter::GetLaserwireWavelength(G4String aName)
{return lwWavelength[aName];}

//for map of laserwire wavelengths
inline G4ThreeVector BDSPhotonCounter::GetLaserwireDir(G4String aName)
{return lwDirection[aName];}

inline void BDSPhotonCounter::SetLaserwireWavelength(G4String aName, G4double aWavelength)
{lwWavelength[aName]=aWavelength;}

inline void BDSPhotonCounter::SetLaserwireDir(G4String aName, G4ThreeVector aDirection)
{lwDirection[aName]=aDirection;}

inline void BDSPhotonCounter::setWaitingForDump(G4bool flag) { isWaitingForDump = flag; } // waiting before all tracks arrive at a dump element
inline G4bool BDSPhotonCounter::getWaitingForDump() { return isWaitingForDump; }

inline void BDSPhotonCounter::setDumping(G4bool flag) { isDumping = flag; } // all tracks are pending - for stacking manager 
inline G4bool BDSPhotonCounter::getDumping() { return isDumping; }

inline void BDSPhotonCounter::setReading(G4bool flag) { isReading = flag; }
inline G4bool BDSPhotonCounter::getReading() { return isReading; }

inline void BDSPhotonCounter::setReadFromStack(G4bool flag) { isReadFromStack = flag; }
inline G4bool BDSPhotonCounter::getReadFromStack() { return isReadFromStack; }

inline G4String BDSPhotonCounter::GetFifo() {return itsFifo;}

inline G4AffineTransform BDSPhotonCounter::GetDumpTransform() {return itsDumpTransform;}
inline void BDSPhotonCounter::SetDumpTransform(G4AffineTransform tf)
{itsDumpTransform=tf;}

inline G4String BDSPhotonCounter::GetRefVolume() { return itsRefVolume; }
inline G4int BDSPhotonCounter::GetRefCopyNo() { return itsRefCopyNo; }

inline const G4AffineTransform* BDSPhotonCounter::GetRefTransform() { return &itsRefTransform; }
inline void BDSPhotonCounter::SetRefTransform(G4AffineTransform& aTransform)
{itsRefTransform=aTransform;}

#endif

