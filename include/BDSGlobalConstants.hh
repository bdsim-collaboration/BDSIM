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
 * Singleton pattern.
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

  // Options that access GMAD::options instance
  // Executable options
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

  // regular options from here on
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
  inline G4double GetDefaultRangeCut()     const {return G4double(options.defaultRangeCut)*CLHEP::m;}
  inline G4double GetBeamTotalEnergy()     const {return G4double(options.beamEnergy)*CLHEP::GeV;}
  inline G4double GetBlmRad()              const {return G4double(options.blmRad)*CLHEP::m;}
  inline G4double GetBlmLength()           const {return G4double(options.blmLength)*CLHEP::m;}
  inline G4double GetSamplerDiameter()     const {return G4double(options.samplerDiameter)*CLHEP::m;}
  inline G4double GetThresholdCutCharged() const {return G4double(options.thresholdCutCharged)*CLHEP::GeV;}
  inline G4double GetThresholdCutPhotons() const {return G4double(options.thresholdCutPhotons)*CLHEP::GeV;}
  inline G4double GetProdCutPhotons()      const {return G4double(options.prodCutPhotons)*CLHEP::m;}
  inline G4double GetProdCutPhotonsP()     const {return G4double(options.prodCutPhotonsP)*CLHEP::m;}
  inline G4double GetProdCutPhotonsA()     const {return G4double(options.prodCutPhotonsA)*CLHEP::m;}
  inline G4double GetProdCutElectrons()    const {return G4double(options.prodCutElectrons)*CLHEP::m;}
  inline G4double GetProdCutElectronsP()   const {return G4double(options.prodCutElectronsP)*CLHEP::m;}
  inline G4double GetProdCutElectronsA()   const {return G4double(options.prodCutElectronsA)*CLHEP::m;}
  inline G4double GetProdCutPositrons()    const {return G4double(options.prodCutPositrons)*CLHEP::m;}
  inline G4double GetProdCutPositronsP()   const {return G4double(options.prodCutPositronsP)*CLHEP::m;}
  inline G4double GetProdCutPositronsA()   const {return G4double(options.prodCutPositronsA)*CLHEP::m;}
  inline G4double GetProdCutProtons()      const {return G4double(options.prodCutProtons)*CLHEP::m;}
  inline G4double GetProdCutProtonsP()     const {return G4double(options.prodCutProtonsP)*CLHEP::m;}
  inline G4double GetProdCutProtonsA()     const {return G4double(options.prodCutProtonsA)*CLHEP::m;}
  inline G4double GetDeltaChord()          const {return G4double(options.deltaChord)*CLHEP::m;}
  inline G4double GetDeltaIntersection()   const {return G4double(options.deltaIntersection)*CLHEP::m;}
  inline G4double GetChordStepMinimum()    const {return G4double(options.chordStepMinimum)*CLHEP::m;}
  inline G4double GetDeltaOneStep()        const {return G4double(options.deltaOneStep)*CLHEP::m;}
  inline G4double GetMinimumEpsilonStep()  const {return G4double(options.minimumEpsilonStep);}
  inline G4double GetMaximumEpsilonStep()  const {return G4double(options.maximumEpsilonStep);}
  inline G4double GetMaxTime()             const {return G4double(options.maximumTrackingTime)*CLHEP::s;}
  inline G4bool   GetDoPlanckScattering()  const {return G4bool  (options.doPlanckScattering);}
  inline G4String GetParticleName()        const {return G4String(options.particleName);}
  inline G4double GetFFact()               const {return G4double(options.ffact);}
  inline G4double GetParticleTotalEnergy() const {return G4double(options.E0)*CLHEP::GeV;}
  inline G4bool   SensitiveComponents()    const {return G4bool  (options.sensitiveBeamlineComponents);}
  inline G4bool   GetSensitiveBeamPipe()   const {return G4bool  (options.sensitiveBeamPipe);}
  inline G4bool   GetSensitiveBLMs()       const {return G4bool  (options.sensitiveBLMs);}
  inline G4bool   GetCheckOverlaps()       const {return G4bool  (options.checkOverlaps);}
  inline G4int    GetEventNumberOffset()   const {return G4int   (options.eventNumberOffset);}
  inline G4long   GetRandomSeed()          const {return G4long  (options.randomSeed);}
  inline G4bool   GetUseEMLPB()            const {return G4bool  (options.useEMLPB);}
  inline G4bool   GetUseHadLPB()           const {return G4bool  (options.useHadLPB);}
  inline G4double GetTrajCutGTZ()          const {return G4double(options.trajCutGTZ);}
  inline G4double GetTrajCutLTR()          const {return G4double(options.trajCutLTR);}
  inline G4bool   GetStoreTrajectory()     const {return G4bool  (options.storeTrajectory);}
  inline G4bool   GetStopSecondaries()     const {return G4bool  (options.stopSecondaries);}
  inline G4bool   GetStopTracks()          const {return G4bool  (options.stopTracks);}
  inline G4double GetScintYieldFactor()    const {return G4double(options.scintYieldFactor);}
  inline G4String GetVacuumMaterial()      const {return G4String(options.vacMaterial);}
  inline G4String GetEmptyMaterial()       const {return "G4_Galactic";}

  // options with long names
  inline G4bool   GetStoreMuonTrajectories()    const {return G4bool(options.storeMuonTrajectories);}
  inline G4bool   GetStoreNeutronTrajectories() const {return G4bool(options.storeNeutronTrajectories);}
  inline G4bool   GetIncludeIronMagFields()     const {return G4bool(options.includeIronMagFields);}
  inline G4bool   GetTurnOnCerenkov()           const {return G4bool(options.turnOnCerenkov);}
  inline G4bool   GetTurnOnOpticalAbsorption()  const {return G4bool(options.turnOnOpticalAbsorption);}
  inline G4bool   GetTurnOnRayleighScattering() const {return G4bool(options.turnOnRayleighScattering);}
  inline G4bool   GetTurnOnMieScattering()      const {return G4bool(options.turnOnMieScattering);}
  inline G4bool   GetTurnOnOpticalSurface()     const {return G4bool(options.turnOnOpticalSurface);}
  inline G4bool   GetTurnOnBirksSaturation()    const {return G4bool(options.turnOnBirksSaturation);}
  inline G4int    GetNumberOfEventsPerNtuple()  const {return G4int (options.numberOfEventsPerNtuple);}
  inline G4double GetElossHistoTransBinWidth()  const {return G4double(options.elossHistoTransBinWidth)*CLHEP::m;}


  // options that require members in this class (for value checking or because they're another class)
  inline G4double GetLengthSafety()        const {return itsLengthSafety;}
  inline G4int    GetTurnsTaken()          const {return itsTurnsTaken;}
  inline G4int    GetTurnsToTake()         const {return itsTurnsToTake;}
  inline G4double GetLPBFraction()         const {return itsLPBFraction;}  
  inline G4double GetBeamKineticEnergy()   const {return itsBeamKineticEnergy;}
  inline G4double GetBeamMomentum()        const {return itsBeamMomentum;}
  inline G4double GetParticleKineticEnergy() const {return itsParticleKineticEnergy;}
  inline G4double GetParticleMomentum()    const {return itsParticleMomentum;}
  inline G4double GetTeleporterLength()    const {return teleporterlength;}
  inline G4double GetSMax()                const {return itsSMax;}
  inline G4RotationMatrix*     RotY90()                  const {return _RotY90;}
  inline G4RotationMatrix*     RotYM90()                 const {return _RotYM90;}
  inline G4RotationMatrix*     RotX90()                  const {return _RotX90;}
  inline G4RotationMatrix*     RotXM90()                 const {return _RotXM90;}
  inline G4RotationMatrix*     RotYM90X90()              const {return _RotYM90X90;}
  inline G4RotationMatrix*     RotYM90XM90()             const {return _RotYM90XM90;}
  inline G4ThreeVector         GetTeleporterDelta()      const {return teleporterdelta;}
  inline G4ParticleDefinition* GetParticleDefinition()   const {return itsBeamParticleDefinition;}
  inline BDSBeamPipeInfo*      GetDefaultBeamPipeModel() const {return defaultBeamPipeModel;}
  inline BDSMagnetGeometryType GetMagnetGeometryType()   const {return itsMagnetGeometryType;}
  inline BDSTunnelInfo*        TunnelInfo()              const {return tunnelInfo;}
  inline G4FieldManager*       GetZeroFieldManager()     const {return itsZeroFieldManager;}
  inline BDSParticle           GetInitialPoint()         const {return itsInitialPoint;}
  inline G4VisAttributes*      GetInvisibleVisAttr()     const {return invisibleVisAttr;}
  inline G4VisAttributes*      GetVisibleDebugVisAttr()  const {return visibleDebugVisAttr;}
  inline G4UserLimits*         GetDefaultUserLimits()    const {return defaultUserLimits;}

  // refactor out of classes that use this
  inline G4double GetMagnetPoleSize() const {return itsMagnetPoleSize;}
  inline G4double GetMagnetPoleRadius() const {return itsMagnetPoleRadius;}
  inline G4bool   GetSampleDistRandomly() const {return true;}
  inline G4double GetLWCalWidth()          const {return itsLWCalWidth;}
  inline G4double GetLWCalOffset()         const {return itsLWCalOffset;}

  // Setters
  void     SetParticleDefinition(G4ParticleDefinition* aBeamParticleDefinition);
  void     SetParticleName(G4String aParticleName);
  void     SetBeamKineticEnergy(G4double val);
  void     SetBeamMomentum(G4double val);
  void     SetParticleKineticEnergy(G4double val);
  void     SetParticleMomentum(G4double val);
  void     SetTeleporterDelta(G4ThreeVector newteleporterdelta);
  void     SetTeleporterLength(G4double newteleporterlength);
  void     SetInitialPoint(BDSParticle& particle);

  // inline setters
  inline void SetSMax(G4double smax) {itsSMax=smax;}
  inline void IncrementTurnNumber()  {itsTurnsTaken += 1;}
  inline void ResetTurnNumber()      {itsTurnsTaken = 0;}
  inline void SetNumberToGenerate(G4int numberToGenerate) {options.set_value("nGenerate", (int)numberToGenerate);}

  // laserwire stuff that probably shouldn't be in global constants
  inline G4double      GetLaserwireWavelength()     const {return itsLaserwireWavelength;}
  inline G4ThreeVector GetLaserwireDir()            const {return itsLaserwireDir;}
  inline G4bool        GetLaserwireTrackPhotons()   const {return itsLaserwireTrackPhotons;}
  inline G4bool        GetLaserwireTrackElectrons() const {return itsLaserwireTrackElectrons;}
  inline G4double      GetLaserwireWavelength(G4String aName) const {return lwWavelength.at(aName);}
  inline G4ThreeVector GetLaserwireDir(G4String aName)        const {return lwDirection.at(aName);}
  void SetLaserwireWavelength(G4String aName, G4double aWavelength);
  void SetLaserwireDir(G4String aName, G4ThreeVector aDirection);

private:

  G4UniformMagField* zeroMagField;
  G4FieldManager* itsZeroFieldManager;

  /// Initial bunch parameters
  G4ParticleDefinition* itsBeamParticleDefinition;

  /// Reference beam energy
  G4double itsBeamMomentum, itsBeamKineticEnergy;

  /// Particle energy
  G4double itsParticleMomentum, itsParticleKineticEnergy;

  /// Beamline length in mm
  G4double itsSMax;
  
  ///@{ Magnet geometry
  BDSMagnetGeometryType itsMagnetGeometryType;
  G4double itsMagnetPoleSize;
  G4double itsMagnetPoleRadius;
  ///@}

  /// Default beam pipe model information
  BDSBeamPipeInfo* defaultBeamPipeModel;
  
  /// Tunnel model
  BDSTunnelInfo* tunnelInfo;
  
  // test map container for laserwire parameters - Steve
  std::map<const G4String, G4double> lwWavelength;
  std::map<const G4String, G4ThreeVector> lwDirection;
  G4double      itsLaserwireWavelength;
  G4ThreeVector itsLaserwireDir;
  G4bool        itsLaserwireTrackPhotons;
  G4bool        itsLaserwireTrackElectrons;
  G4double      itsLWCalWidth;
  G4double      itsLWCalOffset;
  
  G4double itsLengthSafety;

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
  
  ///@{ Turn Control
  G4int    itsTurnsTaken;
  G4int    itsTurnsToTake;
  ///@}
  ///@{ Teleporter offset corrections
  G4ThreeVector teleporterdelta;
  G4double      teleporterlength;
  ///@}
  
  /// initial particle for production of sampler hit
  BDSParticle itsInitialPoint;

  BDSOutputFormat outputFormat;

  G4double itsLPBFraction;
};

inline void BDSGlobalConstants::SetBeamKineticEnergy(G4double val)
{itsBeamKineticEnergy = val;}

inline void BDSGlobalConstants::SetBeamMomentum(G4double val)
{itsBeamMomentum = val;}

inline void BDSGlobalConstants::SetParticleDefinition(G4ParticleDefinition* aBeamParticleDefinition)
{itsBeamParticleDefinition = aBeamParticleDefinition;}

inline void BDSGlobalConstants::SetParticleName(G4String particleName)
{options.set_value("particleName", particleName);}

inline void BDSGlobalConstants::SetLaserwireWavelength(G4String aName, G4double aWavelength)
{lwWavelength[aName]=aWavelength;}

inline void BDSGlobalConstants::SetLaserwireDir(G4String aName, G4ThreeVector aDirection)
{lwDirection[aName]=aDirection;}

inline void BDSGlobalConstants::SetTeleporterDelta(G4ThreeVector newteleporterdelta)
{teleporterdelta = newteleporterdelta;}

inline void BDSGlobalConstants::SetTeleporterLength(G4double newteleporterlength)
{teleporterlength = newteleporterlength;}

inline void BDSGlobalConstants::SetParticleKineticEnergy(G4double val)
{itsParticleKineticEnergy = val;}

inline void BDSGlobalConstants::SetParticleMomentum(G4double val)
{itsParticleMomentum = val;}

inline void BDSGlobalConstants::SetInitialPoint(BDSParticle& particle)
{itsInitialPoint = particle;}

#endif
