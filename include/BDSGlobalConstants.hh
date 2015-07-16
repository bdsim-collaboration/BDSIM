#ifndef BDSGlobalConstants_h
#define BDSGlobalConstants_h 

#include <deque>
#include <map>

#include "G4ThreeVector.hh"
#include "G4String.hh"
#include "G4AffineTransform.hh"

#include "BDSBeamPipeType.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSParticle.hh"
#include "BDSTunnelInfo.hh"

class G4FieldManager;
class G4LogicalVolume;
class G4ParticleDefinition;
class G4UniformMagField;
class G4UserLimits;
class G4VisAttributes;
class G4VPhysicalVolume;

struct Options;

struct strCmp {
  G4bool operator()( const G4String s1, const G4String s2 ) const {
    return strcmp(s1,s2) < 0;}
};

/**
 * @brief a class that holds global options and constants
 * 
 * singleton pattern
 */
class BDSGlobalConstants 
{

protected:
  BDSGlobalConstants(struct Options&);

private:
  static BDSGlobalConstants* _instance;

public:
   /// access method 
  static BDSGlobalConstants* Instance();
  ~BDSGlobalConstants();
  
  G4bool   GetDoPlanckScattering() const;
  G4bool   GetCheckOverlaps() const;
  G4double GetMinimumEpsilonStep() const;
  G4double GetMaximumEpsilonStep() const;
  G4double GetMaxTime() const;
  G4double GetDeltaOneStep() const;

  G4ParticleDefinition* GetParticleDefinition() const;
  void     SetParticleDefinition(G4ParticleDefinition* aBeamParticleDefinition);
  G4String GetParticleName() const;
  void     SetParticleName(G4String aParticleName);

  G4double GetLPBFraction() const;
  G4double GetElossHistoBinWidth() const;
  G4double GetElossHistoTransBinWidth() const; //The transverse (x,y) bin width
  G4double GetDefaultRangeCut() const;

  /// magnetic field switch flag
  G4double GetFFact() const;

  G4double GetBeamKineticEnergy() const;
  void     SetBeamKineticEnergy(G4double val);
  G4double GetBeamTotalEnergy() const;
  void     SetBeamTotalEnergy(G4double val);
  G4double GetBeamMomentum() const;
  void     SetBeamMomentum(G4double val);


  G4double GetParticleKineticEnergy() const;
  void     SetParticleKineticEnergy(G4double val);
  G4double GetParticleTotalEnergy() const;
  G4double GetParticleMomentum() const;
  void     SetParticleMomentum(G4double val);

  G4double GetPlanckScatterFe() const;

  G4double GetGammaToMuFe() const;
  G4double GetAnnihiToMuFe() const;
  G4double GetEeToHadronsFe() const;
  G4bool   GetSampleDistRandomly() const;
  G4bool   GetGeometryBias() const;
  G4bool   GetUseEMLPB() const;
  G4bool   GetUseHadLPB() const;
  // Booleans determining which types of components are sensitive
  G4bool   GetSensitiveComponents() const;
  G4bool   GetSensitiveBeamPipe() const;
  G4bool   GetSensitiveBLMs() const;

  // Magnet geometry variable
  G4double GetComponentBoxSize() const;
  BDSMagnetGeometryType GetMagnetGeometryType() const;
  G4String GetOuterMaterialName() const;
  G4double GetOuterDiameter() const;
  G4double GetMagnetPoleSize() const;
  G4double GetMagnetPoleRadius() const; 

  /// tunnel
  G4String      GetTunnelMaterial()const;
  G4String      GetTunnelSoilMaterial()const;
  G4bool        GetSensitiveTunnel()const;
  BDSTunnelInfo GetTunnelInfo()const;

  // Beam loss monitors
  G4double GetBlmRad() const;
  G4double GetBlmLength() const;

  /// Beampipe
  G4double GetBeamPipeRadius() const;
  G4double GetAper1() const;
  G4double GetAper2() const;
  G4double GetAper3() const;
  G4double GetAper4() const;
  G4double GetBeamPipeThickness() const; 

  /// Sampler
  G4double GetSamplerDiameter() const;
  G4double GetSamplerLength() const;

  /// Chord stepping
  G4double GetDeltaIntersection() const;
  G4double GetDeltaChord() const;
  G4double GetChordStepMinimum() const;

  /// Threshold and Production cuts
  G4double GetThresholdCutCharged() const;
  G4double GetThresholdCutPhotons() const;

  G4double GetProdCutPhotons() const;
  G4double GetProdCutElectrons() const;
  G4double GetProdCutPositrons() const;

  G4double GetProdCutPhotonsP() const;
  G4double GetProdCutPhotonsA() const;
  G4double GetProdCutElectronsP() const;
  G4double GetProdCutElectronsA() const;
  G4double GetProdCutPositronsP() const;
  G4double GetProdCutPositronsA() const;

  G4double GetProdCutHadrons() const;

  // Physical processes etc.

  G4String GetPhysListName() const;
  G4bool   GetSynchRadOn() const;
  G4bool   GetDecayOn() const;
  G4bool   GetSynchTrackPhotons() const;
  G4double GetSynchLowX() const;
  G4double GetSynchLowGamE() const;
  G4int    GetSynchPhotonMultiplicity() const;
  G4int    GetSynchMeanFreeFactor() const;
  G4double GetLaserwireWavelength() const;
  G4ThreeVector GetLaserwireDir() const;

  // Use map to generate multiple laserwires with independent wavelength 
  // and direction
  G4double GetLaserwireWavelength(G4String aName) const; 
  G4ThreeVector GetLaserwireDir(G4String aName) const; 
  void     SetLaserwireWavelength(G4String aName, G4double aWavelength);
  void     SetLaserwireDir(G4String aName, G4ThreeVector aDirection);
  G4bool   GetLaserwireTrackPhotons() const;
  G4bool   GetLaserwireTrackElectrons() const;
  G4bool   GetTurnOnCerenkov() const;
  G4bool   GetTurnOnOpticalAbsorption() const;
  G4bool   GetTurnOnRayleighScattering() const;
  G4bool   GetTurnOnMieScattering() const;
  G4bool   GetTurnOnOpticalSurface() const;
  G4bool   GetTurnOnBirksSaturation() const;
  G4double GetScintYieldFactor() const;

  G4bool   GetStoreMuonTrajectories() const;
  G4double GetTrajCutGTZ() const;
  G4double GetTrajCutLTR() const;
  G4bool   GetStoreNeutronTrajectories() const;
  G4bool   GetStoreTrajectory() const;
  G4bool   GetIncludeIronMagFields() const;
  G4bool   GetStopTracks() const;
  G4bool stopTracks; // kill tracks after interactions

  G4double GetLengthSafety() const;
  G4long   GetRandomSeed() const;
  G4int    GetNumberToGenerate() const;
  void     SetNumberToGenerate(G4int);
  G4int    GetNumberOfEventsPerNtuple() const;
  G4int    GetEventNumberOffset() const;
  G4FieldManager* GetZeroFieldManager() const;

  // G4bool   GetUseSynchPrimaryGen() const;
  // G4double GetSynchPrimaryAngle() const;
  // G4double GetSynchPrimaryLength() const;

  // AI : for placet synchronization
  void     setWaitingForDump(G4bool flag);
  G4bool   getWaitingForDump() const;
  G4bool   getDumping() const;
  G4bool   getReading() const;
  void     setReadFromStack(G4bool flag);
  G4bool   getReadFromStack() const;
  G4String GetFifo() const;
  G4int    GetTurnsTaken() const;
  void     IncrementTurnNumber();
  void     ResetTurnNumber();
  G4int    GetTurnsToTake() const;

  G4AffineTransform GetDumpTransform() const;
  void              SetDumpTransform(G4AffineTransform tf);

  G4double GetSMax() const;
  void     SetSMax(G4double);
  G4ThreeVector GetTeleporterDelta() const;
  void          SetTeleporterDelta(G4ThreeVector newteleporterdelta);
  void          SetTeleporterLength(G4double newteleporterlength);
  G4double      GetTeleporterLength() const;
  
  /// initial particle
  BDSParticle GetInitialPoint() const;
  void SetInitialPoint(BDSParticle& particle);
  
  // SPM : temp filestream for placet to read and write
  //  std::ofstream fileDump;
  // ifstream fileRead; replaced with FILE* fifo in code for consistency with Placet. SPM

  std::deque<BDSParticle> holdingQueue;
  std::deque<BDSParticle> outputQueue;
  std::deque<BDSParticle> transformedQueue;

private:

  G4UniformMagField* zeroMagField;

  G4double itsElossHistoBinWidth;
  G4double itsElossHistoTransBinWidth;
  G4double itsDefaultRangeCut;
  /// fudge factor, flips magnetic fields in elements (except for bends and kicks defined by angle, so that opposite charge and ffact -1 will produce same trajectory)
  /// similar to BV flag in MadX
  G4double itsFFact;

  // initial bunch parameters
  G4String itsParticleName;
  G4ParticleDefinition* itsBeamParticleDefinition;
  /// reference beam energy
  G4double itsBeamTotalEnergy, itsBeamMomentum, itsBeamKineticEnergy;
  /// particle energy
  G4double itsParticleTotalEnergy, itsParticleMomentum, itsParticleKineticEnergy;
  G4double itsLPBFraction;
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

  // magnet geometry
  BDSMagnetGeometryType itsMagnetGeometryType;
  G4String itsOuterMaterialName;
  G4double itsOuterDiameter;
  //G4double itsComponentBoxSize;
  G4double itsMagnetPoleSize;
  G4double itsMagnetPoleRadius;
  
  // new tunnel
  G4String tunnelMaterial;
  G4String tunnelSoilMaterial;
  G4bool   tunnelSensitive;

  // new - default tunnel model
  BDSTunnelInfo tunnelInfo;
  
  //Booleans determining which types of components are sensitive
  G4bool   itsSensitiveComponents;
  G4bool   itsSensitiveBeamPipe;
  G4bool   itsSensitiveBLMs;
  //Beam loss monitor geometry
  G4double itsBlmRad;
  G4double itsBlmLength;
  G4double itsBeamPipeRadius;
  G4double itsAper1;
  G4double itsAper2;
  G4double itsAper3;
  G4double itsAper4;
  G4double itsBeamPipeThickness;
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
  G4double itsProdCutHadrons;
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

  void InitVisAttributes();
  G4VisAttributes* invisibleVisAttr;
  G4VisAttributes* visibleDebugVisAttr;

  void InitDefaultUserLimits();
  G4UserLimits* defaultUserLimits;

public:
  G4RotationMatrix* RotY90() const;
  G4RotationMatrix* RotYM90() const;
  G4RotationMatrix* RotX90() const;
  G4RotationMatrix* RotXM90() const;
  G4RotationMatrix* RotYM90X90() const;
  G4RotationMatrix* RotYM90XM90() const;

  G4double GetLWCalWidth() const;
  G4double GetLWCalOffset() const;
  BDSBeamPipeType GetApertureType() const;
  G4String GetBeamPipeMaterialName() const;
  G4String GetVacuumMaterial() const;
  G4String GetEmptyMaterial() const;

  G4VisAttributes* GetInvisibleVisAttr() const;
  G4VisAttributes* GetVisibleDebugVisAttr() const;

  G4UserLimits* GetDefaultUserLimits() const;
  
private:
  G4double itsLWCalWidth;
  G4double itsLWCalOffset;
  BDSBeamPipeType itsApertureType;       //aperture model to use by default
  G4String itsBeamPipeMaterial;          //beampipe material
  G4String itsVacuumMaterial;            //vacuum inside beampipe
  G4String itsEmptyMaterial;             //empty material for e.g. marker volumes
  G4bool   isWaitingForDump;
  G4bool   isDumping;
  G4bool   isReading;
  G4bool   isReadFromStack;
  G4String itsFifo; // fifo for BDSIM-placet
  G4AffineTransform itsDumpTransform; //transform of frame from start to current dump element
  
  ///@{ Turn Control
  G4int    itsTurnsTaken;
  G4int    itsTurnsToTake;
  ///@}
  ///@{ Teleporter offset corrections
  G4ThreeVector teleporterdelta;
  G4double      teleporterlength;
  ///@}
  /// beamline length in mm
  G4double itsSMax;
  /// initial particle
  BDSParticle itsInitialPoint;

  // private set methods
  void     SetLPBFraction(G4double val);

};

inline G4double BDSGlobalConstants::GetElossHistoBinWidth() const
{return itsElossHistoBinWidth;}

inline G4double BDSGlobalConstants::GetElossHistoTransBinWidth() const
{return itsElossHistoTransBinWidth;}

inline G4double BDSGlobalConstants::GetDefaultRangeCut() const
{return itsDefaultRangeCut;}

inline G4double BDSGlobalConstants::GetFFact() const
{return itsFFact;}

inline G4double BDSGlobalConstants::GetMinimumEpsilonStep() const
{return itsMinimumEpsilonStep;}

inline G4double BDSGlobalConstants::GetMaximumEpsilonStep() const
{return itsMaximumEpsilonStep;}

inline G4double BDSGlobalConstants::GetMaxTime() const
{return itsMaxTime;}

inline G4double BDSGlobalConstants::GetDeltaOneStep() const
{return itsDeltaOneStep;}

inline G4double BDSGlobalConstants::GetBeamKineticEnergy() const
{return itsBeamKineticEnergy;}

inline void BDSGlobalConstants::SetBeamKineticEnergy(G4double val)
{itsBeamKineticEnergy = val;}

inline G4double BDSGlobalConstants::GetLPBFraction() const
{return itsLPBFraction;}

inline void BDSGlobalConstants::SetLPBFraction(G4double val)
{if(val>1.0)
    {itsLPBFraction = 1.0;}
  else if(val<0.0)
    {itsLPBFraction = 0.0;}
  else 
    {itsLPBFraction = val;}
}

inline G4double BDSGlobalConstants::GetBeamTotalEnergy() const
{return itsBeamTotalEnergy;}

inline void BDSGlobalConstants::SetBeamTotalEnergy(G4double val)
{itsBeamTotalEnergy = val;}

inline G4double BDSGlobalConstants::GetBeamMomentum() const
{return itsBeamMomentum;}

inline void BDSGlobalConstants::SetBeamMomentum(G4double val)
{itsBeamMomentum = val;}

inline G4ParticleDefinition* BDSGlobalConstants::GetParticleDefinition() const
{return itsBeamParticleDefinition;}

inline G4String BDSGlobalConstants::GetParticleName() const
{return itsParticleName;}

inline void BDSGlobalConstants::SetParticleDefinition(G4ParticleDefinition* aBeamParticleDefinition)
{itsBeamParticleDefinition = aBeamParticleDefinition;}

inline void BDSGlobalConstants::SetParticleName(G4String aParticleName)
{itsParticleName = aParticleName;}

inline G4double BDSGlobalConstants::GetPlanckScatterFe() const
{return itsPlanckScatterFe;}

inline G4double BDSGlobalConstants::GetGammaToMuFe() const
{return itsGammaToMuFe;}

inline G4double BDSGlobalConstants::GetAnnihiToMuFe() const
{return itsAnnihiToMuFe;}

inline G4double BDSGlobalConstants::GetEeToHadronsFe() const
{return itsEeToHadronsFe;}

inline G4bool BDSGlobalConstants::GetSampleDistRandomly() const{
  return itsSampleDistRandomly;}

inline G4bool BDSGlobalConstants::GetUseEMLPB() const{
  return itsUseEMLPB;}

inline G4bool BDSGlobalConstants::GetUseHadLPB() const{
  return itsUseHadLPB;}

//Booleans determining which types of components are sensitive
inline  G4bool BDSGlobalConstants::GetSensitiveComponents() const
{return itsSensitiveComponents;}

inline  G4bool BDSGlobalConstants::GetSensitiveBeamPipe() const
{return itsSensitiveBeamPipe;}

inline  G4bool BDSGlobalConstants::GetSensitiveBLMs() const
{return itsSensitiveBLMs;}

inline BDSMagnetGeometryType BDSGlobalConstants::GetMagnetGeometryType() const
{return itsMagnetGeometryType;}

inline G4String BDSGlobalConstants::GetOuterMaterialName() const
{return itsOuterMaterialName;}

inline G4double BDSGlobalConstants::GetOuterDiameter() const
{return itsOuterDiameter;}

inline G4double BDSGlobalConstants::GetComponentBoxSize() const
{return itsOuterDiameter;}

inline G4double BDSGlobalConstants::GetMagnetPoleSize() const
{return itsMagnetPoleSize;}

inline G4double BDSGlobalConstants::GetMagnetPoleRadius() const
{return itsMagnetPoleRadius;}

// tunnel
inline G4String BDSGlobalConstants::GetTunnelMaterial() const
{return tunnelMaterial;}

inline G4String BDSGlobalConstants::GetTunnelSoilMaterial() const
{return tunnelSoilMaterial;}

inline G4bool BDSGlobalConstants::GetSensitiveTunnel() const
{return tunnelSensitive;}

inline BDSTunnelInfo BDSGlobalConstants::GetTunnelInfo() const
{return tunnelInfo;}

inline G4bool BDSGlobalConstants::GetGeometryBias() const
{return itsGeometryBias;}

//Beam loss monitors

inline G4double BDSGlobalConstants::GetBlmRad() const
{return itsBlmRad;}

inline G4double BDSGlobalConstants::GetBlmLength() const
{return itsBlmLength;}

inline G4double BDSGlobalConstants::GetBeamPipeRadius() const
{return itsBeamPipeRadius;}

inline G4double BDSGlobalConstants::GetAper1() const
{return itsAper1;}

inline G4double BDSGlobalConstants::GetAper2() const
{return itsAper2;}

inline G4double BDSGlobalConstants::GetAper3() const
{return itsAper3;}

inline G4double BDSGlobalConstants::GetAper4() const
{return itsAper4;}

inline G4double BDSGlobalConstants::GetBeamPipeThickness() const 
{return itsBeamPipeThickness;}

inline G4double BDSGlobalConstants::GetSamplerDiameter() const 
{return itsSamplerDiameter;}

inline G4double BDSGlobalConstants::GetSamplerLength() const 
{return itsSamplerLength;}

inline G4double BDSGlobalConstants::GetDeltaChord() const 
{return itsDeltaChord;}

inline G4double BDSGlobalConstants::GetDeltaIntersection() const 
{return itsDeltaIntersection;}

inline G4double BDSGlobalConstants::GetChordStepMinimum() const 
{return itsChordStepMinimum;}

inline G4double BDSGlobalConstants::GetThresholdCutCharged() const 
{return itsThresholdCutCharged;}

inline G4double BDSGlobalConstants::GetThresholdCutPhotons() const 
{return itsThresholdCutPhotons;}

inline G4double BDSGlobalConstants::GetProdCutPhotons() const 
{return itsProdCutPhotons;}

inline G4double BDSGlobalConstants::GetProdCutPhotonsP() const 
{return itsProdCutPhotonsP;}

inline G4double BDSGlobalConstants::GetProdCutPhotonsA() const 
{return itsProdCutPhotonsA;}

inline G4double BDSGlobalConstants::GetProdCutElectrons() const 
{return itsProdCutElectrons;}

inline G4double BDSGlobalConstants::GetProdCutElectronsP() const 
{return itsProdCutElectronsP;}

inline G4double BDSGlobalConstants::GetProdCutElectronsA() const 
{return itsProdCutElectronsA;}

inline G4double BDSGlobalConstants::GetProdCutPositrons() const 
{return itsProdCutPositrons;}

inline G4double BDSGlobalConstants::GetProdCutPositronsP() const 
{return itsProdCutPositronsP;}

inline G4double BDSGlobalConstants::GetProdCutPositronsA() const 
{return itsProdCutPositronsA;}

inline G4double BDSGlobalConstants::GetProdCutHadrons() const 
{return itsProdCutHadrons;}

inline G4String BDSGlobalConstants::GetPhysListName() const
{return itsPhysListName;}

inline G4bool BDSGlobalConstants::GetSynchRadOn() const
{return itsSynchRadOn;}

inline G4bool BDSGlobalConstants::GetDecayOn() const
{return itsDecayOn;}

inline G4bool BDSGlobalConstants::GetSynchTrackPhotons() const
{return itsSynchTrackPhotons ;}

inline G4double BDSGlobalConstants::GetSynchLowX() const
{return itsSynchLowX ;}

inline G4double BDSGlobalConstants::GetSynchLowGamE() const
{return itsSynchLowGamE ;}

inline G4int BDSGlobalConstants::GetSynchPhotonMultiplicity() const
{return itsSynchPhotonMultiplicity ;}

inline G4int BDSGlobalConstants::GetSynchMeanFreeFactor() const
{return itsSynchMeanFreeFactor ;}

inline G4double BDSGlobalConstants::GetLaserwireWavelength() const
{return itsLaserwireWavelength ;}

inline G4ThreeVector BDSGlobalConstants::GetLaserwireDir() const
{return itsLaserwireDir ;}

inline G4bool BDSGlobalConstants::GetLaserwireTrackPhotons() const
{return itsLaserwireTrackPhotons ;}

inline G4bool BDSGlobalConstants::GetLaserwireTrackElectrons() const
{return itsLaserwireTrackElectrons ;}

inline G4double BDSGlobalConstants::GetLengthSafety() const
{return itsLengthSafety;}

inline G4bool BDSGlobalConstants::GetTurnOnCerenkov() const
{return itsTurnOnCerenkov;}

inline  G4bool BDSGlobalConstants::GetTurnOnOpticalAbsorption() const
{ return itsTurnOnOpticalAbsorption;}

inline  G4bool BDSGlobalConstants::GetTurnOnRayleighScattering() const
{return itsTurnOnRayleighScattering;}

inline  G4bool BDSGlobalConstants::GetTurnOnMieScattering() const
{return itsTurnOnMieScattering;}

inline  G4bool BDSGlobalConstants::GetTurnOnOpticalSurface() const
{return itsTurnOnOpticalSurface;}

inline  G4bool BDSGlobalConstants::GetTurnOnBirksSaturation() const
{return itsTurnOnBirksSaturation;}

inline  G4double BDSGlobalConstants::GetScintYieldFactor() const
{return itsScintYieldFactor;}

inline G4bool BDSGlobalConstants::GetIncludeIronMagFields() const
{return itsIncludeIronMagFields;}

inline G4bool BDSGlobalConstants::GetStoreMuonTrajectories() const
{return itsStoreMuonTrajectories;}

inline G4double BDSGlobalConstants::GetTrajCutGTZ() const
{return itsTrajCutGTZ;}

inline G4double BDSGlobalConstants::GetTrajCutLTR() const
{return itsTrajCutLTR;}

inline G4bool BDSGlobalConstants::GetStoreNeutronTrajectories() const
{return itsStoreNeutronTrajectories;}

inline G4bool BDSGlobalConstants::GetStoreTrajectory() const
{return itsStoreTrajectory;}

inline G4bool BDSGlobalConstants::GetStopTracks() const
{return stopTracks;}

inline G4long BDSGlobalConstants::GetRandomSeed() const
{return itsRandomSeed;}

inline G4int BDSGlobalConstants::GetNumberToGenerate() const
{return itsNumberToGenerate;}

inline void BDSGlobalConstants::SetNumberToGenerate(G4int numberToGenerate)
{itsNumberToGenerate = numberToGenerate;}

inline G4int BDSGlobalConstants::GetNumberOfEventsPerNtuple() const
{return itsNumberOfEventsPerNtuple;}

inline G4int BDSGlobalConstants::GetEventNumberOffset() const
{return itsEventNumberOffset;}

inline G4FieldManager* BDSGlobalConstants::GetZeroFieldManager() const
{return itsZeroFieldManager;}

inline  G4double BDSGlobalConstants::GetLWCalWidth() const
{return itsLWCalWidth;}

inline  G4double BDSGlobalConstants::GetLWCalOffset() const
{return itsLWCalOffset;}

inline BDSBeamPipeType BDSGlobalConstants::GetApertureType() const
{return itsApertureType;}

inline G4String BDSGlobalConstants::GetBeamPipeMaterialName() const
{return itsBeamPipeMaterial;}

inline G4String BDSGlobalConstants::GetVacuumMaterial() const
{return itsVacuumMaterial;}

inline G4String BDSGlobalConstants::GetEmptyMaterial() const
{return itsEmptyMaterial;}

inline G4bool BDSGlobalConstants::GetDoPlanckScattering() const 
{return itsDoPlanckScattering;}

inline G4bool BDSGlobalConstants::GetCheckOverlaps() const 
{return itsCheckOverlaps;}

//for map of laserwire wavelengths
inline G4double BDSGlobalConstants::GetLaserwireWavelength(G4String aName) const
{return lwWavelength.at(aName);}

//for map of laserwire wavelengths
inline G4ThreeVector BDSGlobalConstants::GetLaserwireDir(G4String aName) const
{return lwDirection.at(aName);}

inline void BDSGlobalConstants::SetLaserwireWavelength(G4String aName, G4double aWavelength)
{lwWavelength[aName]=aWavelength;}

inline void BDSGlobalConstants::SetLaserwireDir(G4String aName, G4ThreeVector aDirection)
{lwDirection[aName]=aDirection;}

inline void BDSGlobalConstants::setWaitingForDump(G4bool flag)
{isWaitingForDump = flag;} // waiting before all tracks arrive at a dump element

inline G4bool BDSGlobalConstants::getWaitingForDump() const 
{return isWaitingForDump;}

inline G4bool BDSGlobalConstants::getDumping() const
{return isDumping;}

inline G4bool BDSGlobalConstants::getReading() const
{return isReading;}

inline void BDSGlobalConstants::setReadFromStack(G4bool flag)
{isReadFromStack = flag;}

inline G4bool BDSGlobalConstants::getReadFromStack() const
{return isReadFromStack;}

inline G4String BDSGlobalConstants::GetFifo() const
{return itsFifo;}

inline G4AffineTransform BDSGlobalConstants::GetDumpTransform() const
{return itsDumpTransform;}

inline void BDSGlobalConstants::SetDumpTransform(G4AffineTransform tf)
{itsDumpTransform=tf;}

inline G4int BDSGlobalConstants::GetTurnsTaken() const
{return itsTurnsTaken;}

inline void  BDSGlobalConstants::IncrementTurnNumber()
{itsTurnsTaken += 1;}

inline void  BDSGlobalConstants::ResetTurnNumber()
{itsTurnsTaken = 1;}

inline G4int BDSGlobalConstants::GetTurnsToTake() const
{return itsTurnsToTake;}

inline G4double BDSGlobalConstants::GetSMax() const 
{return itsSMax;}

inline void BDSGlobalConstants::SetSMax(G4double smax)
{itsSMax=smax;}

inline G4ThreeVector BDSGlobalConstants::GetTeleporterDelta() const
{return teleporterdelta;}

inline void BDSGlobalConstants::SetTeleporterDelta(G4ThreeVector newteleporterdelta)
{teleporterdelta = newteleporterdelta;}

inline void BDSGlobalConstants::SetTeleporterLength(G4double newteleporterlength)
{teleporterlength = newteleporterlength;}

inline G4double BDSGlobalConstants::GetTeleporterLength() const
{return teleporterlength;}

inline G4double BDSGlobalConstants::GetParticleKineticEnergy() const
{return itsParticleKineticEnergy;}

inline void BDSGlobalConstants::SetParticleKineticEnergy(G4double val)
{itsParticleKineticEnergy = val;}

inline G4double BDSGlobalConstants::GetParticleTotalEnergy() const
{return itsParticleTotalEnergy;}

inline G4double BDSGlobalConstants::GetParticleMomentum() const
{return itsParticleMomentum;}

inline void BDSGlobalConstants::SetParticleMomentum(G4double val)
{itsParticleMomentum = val;}

inline BDSParticle BDSGlobalConstants::GetInitialPoint() const
{return itsInitialPoint;}

inline void BDSGlobalConstants::SetInitialPoint(BDSParticle& particle)
{itsInitialPoint = particle;}

inline G4VisAttributes* BDSGlobalConstants::GetInvisibleVisAttr() const
{return invisibleVisAttr;}

inline G4VisAttributes* BDSGlobalConstants::GetVisibleDebugVisAttr() const
{return visibleDebugVisAttr;}

inline G4UserLimits* BDSGlobalConstants::GetDefaultUserLimits() const
{return defaultUserLimits;}

#endif
