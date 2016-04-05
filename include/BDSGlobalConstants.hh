#ifndef BDSGLOBALCONSTANTS_H
#define BDSGLOBALCONSTANTS_H 

#include "BDSMagnetGeometryType.hh"
#include "BDSOutputFormat.hh"
#include "BDSParticle.hh"
#include "BDSTunnelInfo.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4AffineTransform.hh"

#include "parser/options.h"

#include <map>

class G4FieldManager;
class G4LogicalVolume;
class G4ParticleDefinition;
class G4UniformMagField;
class G4UserLimits;
class G4VisAttributes;
class G4VPhysicalVolume;

class BDSBeamPipeInfo;

/**
 * @brief A class that holds global options and constants.
 *
 * This wraps an instance of GMAD::Options and also converts
 * from the std C++ types used in the options to the Geant4 types
 * solely used in BDSIM. Additionally, some more refined information 
 * is provided - for example, simple strings are converted to enum types
 * for outputs and geometry, as well as G4Materials.
 *
 * This is almost entirely constant apart from a very few variables 
 * that by necessity can be updated. Generally, this is not used as
 * as a way to dynamically pass around information at a global level
 * but purely as constants.
 * 
 * Singleton pattern
 */

class BDSGlobalConstants
{
protected:
  /// Protected constructor based on a set of gmad options.
  BDSGlobalConstants(const GMAD::Options&);

private:
  /// Singleton instance
  static BDSGlobalConstants* instance;

  /// Options instance that this is largely based on and extends
  GMAD::Options options;

public:
   /// Access method 
  static BDSGlobalConstants* Instance();
  ~BDSGlobalConstants();

  inline G4String InputFileName()          const {return G4String(options.inputFileName);}
  inline G4String VisMacroFileName()       const {return G4String(options.visMacroFileName);}
  inline G4bool   VisDebug()               const {return G4bool  (options.visDebug);}
  inline G4String OutputFileName()         const {return G4String(options.outputFileName);}
  inline G4bool   OutputFileNameSet()      const {return G4bool  (options.HasBeenSet("outputFileName"));}
  inline BDSOutputFormat OutputFormat()    const {return outputFormat;}
  inline G4bool   Survey()                 const {return G4bool  (options.survey);}
  inline G4String SurveyFileName()         const {return G4String(options.surveyFileName);}
  inline G4bool   GFlash()                 const {return G4bool  (options.gflash);}
  inline G4double GFlashEMax()             const {return G4double(options.gflashemax);}
  inline G4double GFlashEMin()             const {return G4double(options.gflashemin);}
  inline G4bool   Batch()                  const {return G4bool  (options.batch);}
  inline G4bool   Verbose()                const {return G4bool  (options.verbose);}
  inline G4bool   VerboseEvent()           const {return G4bool  (options.verboseEvent);}
  inline G4bool   VerboseStep()            const {return G4bool  (options.verboseStep);}
  inline G4int    VerboseEventNumber()     const {return G4int   (options.verboseEventNumber);}
  inline G4int    VerboseRunLevel()        const {return G4int   (options.verboseRunLevel);}
  inline G4int    VerboseEventLevel()      const {return G4int   (options.verboseEventLevel);}
  inline G4int    VerboseTrackingLevel()   const {return G4int   (options.verboseTrackingLevel);}
  inline G4int    VerboseSteppingLevel()   const {return G4int   (options.verboseSteppingLevel);}
  inline G4bool   Circular()               const {return G4bool  (options.circular);}
  inline G4int    Seed()                   const {return G4int   (options.seed);}
  inline G4bool   SetSeedState()           const {return G4bool  (options.setSeedState);}
  inline G4String SeedStateFileName()      const {return G4String(options.seedStateFileName);}
  inline G4String BDSIMPath()              const {return G4String(options.bdsimPath);}
  inline G4int    NGenerate()              const {return G4int   (options.nGenerate);}
  inline G4bool   GeneratePrimariesOnly()  const {return G4bool  (options.generatePrimariesOnly);}
  inline G4bool   ExportGeometry()         const {return G4bool  (options.exportGeometry);}
  inline G4String ExportType()             const {return G4String(options.exportType);}
  inline G4String ExportFileName()         const {return G4String(options.exportFileName);}
  

  inline G4double GetPrintModuloFraction() const {return G4double(options.printModuloFraction);}
  inline G4double GetPlanckScatterFe()     const {return G4double(options.planckScatterFe);}
  inline G4double GetOuterDiameter()       const {return G4double(options.outerDiameter)*CLHEP::m;}
  inline G4double GetComponentBoxSize()    const {return GetOuterDiameter();}
  inline G4String GetOuterMaterialName()   const {return G4String(options.outerMaterialName);}
  inline G4bool   DontSplitSBends()        const {return G4bool  (options.dontSplitSBends);}
  inline G4bool   BuildTunnel()            const {return G4bool  (options.buildTunnel);}
  inline G4bool   BuildTunnelStraight()    const {return G4bool  (options.buildTunnelStraight);}
  inline G4double TunnelOffsetX()          const {return G4double(options.tunnelOffsetX)*CLHEP::m;}
  inline G4double TunnelOffsetY()          const {return G4double(options.tunnelOffsetY)*CLHEP::m;}
  inline G4double GetElossHistoBinWidth()  const {return G4double(options.elossHistoBinWidth)*CLHEP::m;}
  inline G4double GetElossHistoTransBinWidth() const {return G4double(options.elossHistoTransBinWidth)*CLHEP::m;}
  inline G4double GetDefaultRangeCut()     const {return G4double(options.defaultRangeCut)*CLHEP::m;}
  inline G4double GetBeamTotalEnergy()     const {return G4double(options.beamEnergy)*CLHEP::GeV;}
  
  
  G4bool   GetDoPlanckScattering() const;
  G4bool   GetCheckOverlaps() const;
  G4double GetMinimumEpsilonStep() const;
  G4double GetMaximumEpsilonStep() const;
  G4double GetMaxTime() const;
  G4double GetDeltaOneStep() const;

  G4ParticleDefinition* GetParticleDefinition() const;
  void     SetParticleDefinition(G4ParticleDefinition* aBeamParticleDefinition);
  G4String GetParticleName() const {return G4String(options.particleName);}
  void     SetParticleName(G4String aParticleName);

  G4double GetLPBFraction() const;
  

  /// Magnetic field switch flag
  G4double GetFFact() const {return G4double(options.ffact);}

  G4double GetBeamKineticEnergy() const;
  void     SetBeamKineticEnergy(G4double val);

  G4double GetBeamMomentum() const;
  void     SetBeamMomentum(G4double val);


  G4double GetParticleKineticEnergy() const;
  void     SetParticleKineticEnergy(G4double val);
  G4double GetParticleTotalEnergy() const {return options.E0*CLHEP::GeV;}
  G4double GetParticleMomentum() const;
  void     SetParticleMomentum(G4double val);


  G4bool   GetSampleDistRandomly() const;
  G4bool   GetUseEMLPB() const;
  G4bool   GetUseHadLPB() const;
  ///@{ Booleans determining which types of components are sensitive
  G4bool   SensitiveComponents()  const {return G4bool(options.sensitiveBeamlineComponents);}
  G4bool   GetSensitiveBeamPipe() const {return G4bool(options.sensitiveBeamPipe);}
  G4bool   GetSensitiveBLMs()     const {return G4bool(options.sensitiveBLMs);}
  ///@}

  BDSBeamPipeInfo* GetDefaultBeamPipeModel() const;
  
  /// Magnet geometry variable
  BDSMagnetGeometryType GetMagnetGeometryType() const;
  G4double GetMagnetPoleSize() const;
  G4double GetMagnetPoleRadius() const;

  /// Tunnel
  BDSTunnelInfo* TunnelInfo()          const;
  
  ///@{ Beam loss monitors
  G4double GetBlmRad() const;
  G4double GetBlmLength() const;
  ///@}
  
  
  ///@{ Sampler
  G4double GetSamplerDiameter() const;
  G4double GetSamplerLength() const;
  ///@}
  
  ///@{ Chord stepping
  G4double GetDeltaIntersection() const;
  G4double GetDeltaChord() const;
  G4double GetChordStepMinimum() const;
  ///@}
  
  ///@{ Threshold and Production cuts accessor
  G4double GetThresholdCutCharged() const;
  G4double GetThresholdCutPhotons() const;
  
  G4double GetProdCutPhotons()   const;
  G4double GetProdCutElectrons() const;
  G4double GetProdCutPositrons() const;
  G4double GetProdCutProtons()   const;
  
  G4double GetProdCutPhotonsP()   const;
  G4double GetProdCutElectronsP() const; 
  G4double GetProdCutPositronsP() const;
  G4double GetProdCutProtonsP()   const;

  G4double GetProdCutPhotonsA()   const;
  G4double GetProdCutElectronsA() const; 
  G4double GetProdCutPositronsA() const;
  G4double GetProdCutProtonsA()   const;
  ///@}
  
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
  G4bool   GetStopSecondaries() const;
  G4bool   GetStopTracks() const;

  G4double GetLengthSafety() const;
  G4long   GetRandomSeed() const;
  void     SetNumberToGenerate(G4int);
  G4int    GetNumberOfEventsPerNtuple() const;
  G4int    GetEventNumberOffset() const;
  G4FieldManager* GetZeroFieldManager() const;

  /// This is the number of complete turns already completed. Starts at 0 and
  /// increments to 1 after 1 complete turn.
  G4int    GetTurnsTaken() const;
  void     IncrementTurnNumber();

  /// Reset turn number to 0.
  void     ResetTurnNumber();

  /// Get the number of complete turns that should be simulated.
  G4int    GetTurnsToTake() const;

  G4double GetSMax() const;
  void     SetSMax(G4double);
  G4ThreeVector GetTeleporterDelta() const;
  void          SetTeleporterDelta(G4ThreeVector newteleporterdelta);
  void          SetTeleporterLength(G4double newteleporterlength);
  G4double      GetTeleporterLength() const;
  
  ///@{ Initial particle
  BDSParticle GetInitialPoint() const;
  void SetInitialPoint(BDSParticle& particle);
  ///@}
private:

  G4UniformMagField* zeroMagField;

  ///@{ Initial bunch parameters
  G4ParticleDefinition* itsBeamParticleDefinition;
  ///@}
  /// Reference beam energy
  G4double itsBeamMomentum, itsBeamKineticEnergy;
  /// Particle energy
  G4double itsParticleMomentum, itsParticleKineticEnergy;
  G4double itsLPBFraction;
  G4bool   itsSampleDistRandomly;
  G4bool   itsUseEMLPB;
  G4bool   itsUseHadLPB;
  G4double itsMinimumEpsilonStep;
  G4double itsMaximumEpsilonStep;
  G4double itsMaxTime;
  G4double itsDeltaOneStep;
  G4bool   stopTracks; ///< kill tracks after interactions
  G4bool   stopSecondaries; ///< kill secondaries
  
  ///@{ Magnet geometry
  BDSMagnetGeometryType itsMagnetGeometryType;
  G4double itsMagnetPoleSize;
  G4double itsMagnetPoleRadius;
  ///@}

  /// Default beam pipe model information
  BDSBeamPipeInfo* defaultBeamPipeModel;
  
  /// Tunnel model
  BDSTunnelInfo* tunnelInfo;
  
  ///@{ Beam loss monitor geometry
  G4double itsBlmRad;
  G4double itsBlmLength;
  ///@}
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
  G4double itsProdCutProtons;
  G4double itsProdCutProtonsP;
  G4double itsProdCutProtonsA;
  
  // test map container for laserwire parameters - Steve
  std::map<const G4String, G4double> lwWavelength;
  std::map<const G4String, G4ThreeVector> lwDirection;
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
  G4int    itsNumberOfEventsPerNtuple;
  G4int    itsEventNumberOffset;
  G4FieldManager* itsZeroFieldManager;
  /// rotation
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

  G4String GetVacuumMaterial() const {return G4String(options.vacMaterial);}
  G4String GetEmptyMaterial() const;

  G4VisAttributes* GetInvisibleVisAttr() const;
  G4VisAttributes* GetVisibleDebugVisAttr() const;

  G4UserLimits* GetDefaultUserLimits() const;
  
private:
  G4double itsLWCalWidth;
  G4double itsLWCalOffset;
  
  G4String itsEmptyMaterial;          ///<empty material for e.g. marker volumes
  
  ///@{ Turn Control
  G4int    itsTurnsTaken;
  G4int    itsTurnsToTake;
  ///@}
  ///@{ Teleporter offset corrections
  G4ThreeVector teleporterdelta;
  G4double      teleporterlength;
  ///@}
  /// Beamline length in mm
  G4double itsSMax;
  /// initial particle for production of sampler hit
  BDSParticle itsInitialPoint;

  // private set methods
  void     SetLPBFraction(G4double val);


  BDSOutputFormat outputFormat;
};

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

inline G4double BDSGlobalConstants::GetBeamMomentum() const
{return itsBeamMomentum;}

inline void BDSGlobalConstants::SetBeamMomentum(G4double val)
{itsBeamMomentum = val;}

inline G4ParticleDefinition* BDSGlobalConstants::GetParticleDefinition() const
{return itsBeamParticleDefinition;}

inline void BDSGlobalConstants::SetParticleDefinition(G4ParticleDefinition* aBeamParticleDefinition)
{itsBeamParticleDefinition = aBeamParticleDefinition;}

inline void BDSGlobalConstants::SetParticleName(G4String particleName)
{options.set_value("particleName", particleName);}

inline G4bool BDSGlobalConstants::GetSampleDistRandomly() const{
  return itsSampleDistRandomly;}

inline G4bool BDSGlobalConstants::GetUseEMLPB() const{
  return itsUseEMLPB;}

inline G4bool BDSGlobalConstants::GetUseHadLPB() const{
  return itsUseHadLPB;}

inline  BDSBeamPipeInfo* BDSGlobalConstants::GetDefaultBeamPipeModel() const
{return defaultBeamPipeModel;}  

inline BDSMagnetGeometryType BDSGlobalConstants::GetMagnetGeometryType() const
{return itsMagnetGeometryType;}

inline G4double BDSGlobalConstants::GetMagnetPoleSize() const
{return itsMagnetPoleSize;}

inline G4double BDSGlobalConstants::GetMagnetPoleRadius() const
{return itsMagnetPoleRadius;}

inline BDSTunnelInfo* BDSGlobalConstants::TunnelInfo() const
{return tunnelInfo;}

//Beam loss monitors

inline G4double BDSGlobalConstants::GetBlmRad() const
{return itsBlmRad;}

inline G4double BDSGlobalConstants::GetBlmLength() const
{return itsBlmLength;}

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

inline G4double BDSGlobalConstants::GetProdCutProtons() const 
{return itsProdCutProtons;}

inline G4double BDSGlobalConstants::GetProdCutProtonsP() const 
{return itsProdCutProtonsP;}

inline G4double BDSGlobalConstants::GetProdCutProtonsA() const 
{return itsProdCutProtonsA;}

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

inline G4bool BDSGlobalConstants::GetStopSecondaries() const
{return stopSecondaries;}

inline G4bool BDSGlobalConstants::GetStopTracks() const
{return stopTracks;}

inline G4long BDSGlobalConstants::GetRandomSeed() const
{return itsRandomSeed;}

inline void BDSGlobalConstants::SetNumberToGenerate(G4int numberToGenerate)
{options.set_value("nGenerate", (int)numberToGenerate);}

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

inline G4int BDSGlobalConstants::GetTurnsTaken() const
{return itsTurnsTaken;}

inline void  BDSGlobalConstants::IncrementTurnNumber()
{itsTurnsTaken += 1;}

inline void  BDSGlobalConstants::ResetTurnNumber()
{itsTurnsTaken = 0;}

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
