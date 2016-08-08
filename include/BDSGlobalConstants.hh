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
 * This wraps (but does not inherit) an instance of GMAD::Options and also converts
 * from the std C++ types used in the options to the Geant4 types
 * solely used in BDSIM. Additionally, some more refined information 
 * is provided - for example, simple strings are converted to enum types
 * for outputs and geometry, as well as G4Materials.
 *
 * This does modify its instance of GMAD::Options due to some parameter
 * checks.
 *
 * This is almost entirely constant apart from a very few variables 
 * that by necessity can be updated. Generally, this is not used as
 * as a way to dynamically pass around information at a global level
 * but purely as constants.
 * 
 * Singleton pattern. The (private) constructor requires a GMAD::Options
 * instance, but to maintain the singleton pattern this is accessed through
 * BDSParser singleton instance.
 *
 * Note, some options that are used by BDSBunchInterface derived classes 
 * don't have accessors here as they're only used directly there.
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

  /// Access the underlying parser options.
  const GMAD::Options& Options() const {return options;}

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
  inline G4bool   SeedSet()                const {return G4bool  (options.HasBeenSet("seed"));}
  inline G4String BDSIMPath()              const {return G4String(options.bdsimPath);}
  inline G4int    NGenerate()              const {return G4int   (options.nGenerate);}
  inline G4bool   GeneratePrimariesOnly()  const {return G4bool  (options.generatePrimariesOnly);}
  inline G4bool   ExportGeometry()         const {return G4bool  (options.exportGeometry);}
  inline G4String ExportType()             const {return G4String(options.exportType);}
  inline G4String ExportFileName()         const {return G4String(options.exportFileName);}  

  // regular options from here on
  inline G4double PrintModuloFraction()      const {return G4double(options.printModuloFraction);}
  inline G4double PlanckScatterFraction()    const {return G4double(options.planckScatterFe);}
  inline G4double LengthSafety()             const {return G4double(options.lengthSafety*CLHEP::m);}
  inline G4double OuterDiameter()            const {return G4double(options.outerDiameter)*CLHEP::m;}
  inline G4double ComponentBoxSize()         const {return OuterDiameter();}
  inline G4String OuterMaterialName()        const {return G4String(options.outerMaterialName);}
  inline G4bool   DontSplitSBends()          const {return G4bool  (options.dontSplitSBends);}
  inline G4bool   BuildTunnel()              const {return G4bool  (options.buildTunnel);}
  inline G4bool   BuildTunnelStraight()      const {return G4bool  (options.buildTunnelStraight);}
  inline G4double TunnelOffsetX()            const {return G4double(options.tunnelOffsetX)*CLHEP::m;}
  inline G4double TunnelOffsetY()            const {return G4double(options.tunnelOffsetY)*CLHEP::m;}
  inline G4double ElossHistoBinWidth()       const {return G4double(options.elossHistoBinWidth)*CLHEP::m;}
  inline G4double DefaultRangeCut()          const {return G4double(options.defaultRangeCut)*CLHEP::m;}
  inline G4double BeamTotalEnergy()          const {return G4double(options.beamEnergy)*CLHEP::GeV;}
  inline G4double BlmRad()                   const {return G4double(options.blmRad)*CLHEP::m;}
  inline G4double BlmLength()                const {return G4double(options.blmLength)*CLHEP::m;}
  inline G4double SamplerDiameter()          const {return G4double(options.samplerDiameter)*CLHEP::m;}
  inline G4double ThresholdCutCharged()      const {return G4double(options.thresholdCutCharged)*CLHEP::GeV;}
  inline G4double ThresholdCutPhotons()      const {return G4double(options.thresholdCutPhotons)*CLHEP::GeV;}
  inline G4double ProdCutPhotons()           const {return G4double(options.prodCutPhotons)*CLHEP::m;}
  inline G4double ProdCutPhotonsP()          const {return G4double(options.prodCutPhotonsP)*CLHEP::m;}
  inline G4double ProdCutPhotonsA()          const {return G4double(options.prodCutPhotonsA)*CLHEP::m;}
  inline G4double ProdCutElectrons()         const {return G4double(options.prodCutElectrons)*CLHEP::m;}
  inline G4double ProdCutElectronsP()        const {return G4double(options.prodCutElectronsP)*CLHEP::m;}
  inline G4double ProdCutElectronsA()        const {return G4double(options.prodCutElectronsA)*CLHEP::m;}
  inline G4double ProdCutPositrons()         const {return G4double(options.prodCutPositrons)*CLHEP::m;}
  inline G4double ProdCutPositronsP()        const {return G4double(options.prodCutPositronsP)*CLHEP::m;}
  inline G4double ProdCutPositronsA()        const {return G4double(options.prodCutPositronsA)*CLHEP::m;}
  inline G4double ProdCutProtons()           const {return G4double(options.prodCutProtons)*CLHEP::m;}
  inline G4double ProdCutProtonsP()          const {return G4double(options.prodCutProtonsP)*CLHEP::m;}
  inline G4double ProdCutProtonsA()          const {return G4double(options.prodCutProtonsA)*CLHEP::m;}
  inline G4double DeltaChord()               const {return G4double(options.deltaChord)*CLHEP::m;}
  inline G4double DeltaIntersection()        const {return G4double(options.deltaIntersection)*CLHEP::m;}
  inline G4double ChordStepMinimum()         const {return G4double(options.chordStepMinimum)*CLHEP::m;}
  inline G4double DeltaOneStep()             const {return G4double(options.deltaOneStep)*CLHEP::m;}
  inline G4double MinimumEpsilonStep()       const {return G4double(options.minimumEpsilonStep);}
  inline G4double MaximumEpsilonStep()       const {return G4double(options.maximumEpsilonStep);}
  inline G4double MaxTime()                  const {return G4double(options.maximumTrackingTime)*CLHEP::s;}
  inline G4int    TurnsToTake()              const {return G4int   (options.nturns);}
  inline G4bool   DoPlanckScattering()       const {return G4bool  (options.doPlanckScattering);}
  inline G4double FFact()                    const {return G4double(options.ffact);}
  inline G4double ParticleTotalEnergy()      const {return G4double(options.E0)*CLHEP::GeV;}
  inline G4bool   SensitiveComponents()      const {return G4bool  (options.sensitiveBeamlineComponents);}
  inline G4bool   SensitiveBeamPipe()        const {return G4bool  (options.sensitiveBeamPipe);}
  inline G4bool   SensitiveBLMs()            const {return G4bool  (options.sensitiveBLMs);}
  inline G4bool   CheckOverlaps()            const {return G4bool  (options.checkOverlaps);}
  inline G4int    EventNumberOffset()        const {return G4int   (options.eventNumberOffset);}
  inline G4bool   UseEMLPB()                 const {return G4bool  (options.useEMLPB);}
  inline G4bool   UseHadLPB()                const {return G4bool  (options.useHadLPB);}
  inline G4double LPBFraction()              const {return G4double(options.LPBFraction);}
  inline G4double TrajCutGTZ()               const {return G4double(options.trajCutGTZ);}
  inline G4double TrajCutLTR()               const {return G4double(options.trajCutLTR);}
  inline G4bool   StoreTrajectory()          const {return G4bool  (options.storeTrajectory);}
  inline G4int    StoreTrajectoryDepth()     const {return G4int   (options.storeTrajectoryDepth);}
  inline G4String StoreTrajectoryParticle()  const {return G4String(options.storeTrajectoryParticle);}
  inline G4double StoreTrajectoryEnergyThreshold() const {return G4double (options.storeTrajectoryEnergyThreshold);}
  inline G4bool   StopSecondaries()          const {return G4bool  (options.stopSecondaries);}
  inline G4bool   StopTracks()               const {return G4bool  (options.stopTracks);}
  inline G4bool   KillNeutrinos()            const {return G4bool  (options.killNeutrinos);}
  inline G4double ScintYieldFactor()         const {return G4double(options.scintYieldFactor);}
  inline G4String VacuumMaterial()           const {return G4String(options.vacMaterial);}
  inline G4String EmptyMaterial()            const {return G4String(options.emptyMaterial);}
  //inline G4bool   IncludeIronMagFields()     const {return G4bool  (options.includeIronMagFields);} // TBC
  inline G4bool   IncludeIronMagFields()     const {return false;}
  inline G4bool   TurnOnCerenkov()           const {return G4bool  (options.turnOnCerenkov);}
  inline G4bool   TurnOnOpticalAbsorption()  const {return G4bool  (options.turnOnOpticalAbsorption);}
  inline G4bool   TurnOnRayleighScattering() const {return G4bool  (options.turnOnRayleighScattering);}
  inline G4bool   TurnOnMieScattering()      const {return G4bool  (options.turnOnMieScattering);}
  inline G4bool   TurnOnOpticalSurface()     const {return G4bool  (options.turnOnOpticalSurface);}
  inline G4bool   TurnOnBirksSaturation()    const {return G4bool  (options.turnOnBirksSaturation);}
  inline G4int    NumberOfEventsPerNtuple()  const {return G4int   (options.numberOfEventsPerNtuple);}
  inline G4double ElossHistoTransBinWidth()  const {return G4double(options.elossHistoTransBinWidth)*CLHEP::m;}
  inline G4int    NSegmentsPerCircle()       const {return G4int   (options.nSegmentsPerCircle);}

  // options that require members in this class (for value checking or because they're from another class)
  inline G4int    TurnsTaken()               const {return turnsTaken;}
  inline G4double BeamKineticEnergy()        const {return beamKineticEnergy;}
  inline G4double BeamMomentum()             const {return beamMomentum;}
  inline G4double ParticleKineticEnergy()    const {return particleKineticEnergy;}
  inline G4double ParticleMomentum()         const {return particleMomentum;}
  inline G4String ParticleName()             const {return particleName;}
  inline G4double TeleporterLength()         const {return teleporterlength;}
  inline G4double SMax()                     const {return sMax;}
  inline G4double SMaxHistograms()           const {return sMaxHistograms;}
  inline G4int    NBins()                    const {return nBins;}
  inline G4RotationMatrix*     RotY90()                  const {return rotY90;}
  inline G4RotationMatrix*     RotYM90()                 const {return rotYM90;}
  inline G4RotationMatrix*     RotX90()                  const {return rotX90;}
  inline G4RotationMatrix*     RotXM90()                 const {return rotXM90;}
  inline G4RotationMatrix*     RotYM90X90()              const {return rotYM90X90;}
  inline G4RotationMatrix*     RotYM90XM90()             const {return rotYM90XM90;}
  inline G4ThreeVector         GetTeleporterDelta()      const {return teleporterdelta;}
  inline G4ParticleDefinition* GetParticleDefinition()   const {return beamParticleDefinition;}
  inline BDSBeamPipeInfo*      GetDefaultBeamPipeModel() const {return defaultBeamPipeModel;}
  inline BDSMagnetGeometryType GetMagnetGeometryType()   const {return magnetGeometryType;}
  inline BDSTunnelInfo*        TunnelInfo()              const {return tunnelInfo;}
  inline G4FieldManager*       GetZeroFieldManager()     const {return zeroFieldManager;}
  inline BDSParticle           GetInitialPoint()         const {return initialPoint;}
  inline G4VisAttributes*      GetInvisibleVisAttr()     const {return invisibleVisAttr;}
  inline G4VisAttributes*      GetVisibleDebugVisAttr()  const {return visibleDebugVisAttr;}
  inline G4VisAttributes*      GetContainerVisAttr()     const {return options.visDebug ? visibleDebugVisAttr : invisibleVisAttr;}
  inline G4UserLimits*         GetDefaultUserLimits()    const {return defaultUserLimits;}

  // refactor out of classes that use this
  inline G4double MagnetPoleSize()     const {return itsMagnetPoleSize;}
  inline G4double MagnetPoleRadius()   const {return itsMagnetPoleRadius;}
  inline G4double LWCalWidth()         const {return itsLWCalWidth;}
  inline G4double LWCalOffset()        const {return itsLWCalOffset;}

  // Setters
  inline void SetParticleDefinition(G4ParticleDefinition* aBeamParticleDefinition);
  inline void SetParticleName(G4String aParticleName);
  inline void SetBeamKineticEnergy(G4double val);
  inline void SetBeamMomentum(G4double val);
  inline void SetParticleKineticEnergy(G4double val);
  inline void SetParticleMomentum(G4double val);
  inline void SetTeleporterDelta(G4ThreeVector newteleporterdelta);
  inline void SetTeleporterLength(G4double newteleporterlength);
  inline void SetInitialPoint(BDSParticle& particle);
  inline void SetSMax(G4double sMaxIn);
  
  // inline setters
  inline void IncrementTurnNumber()  {turnsTaken += 1;}
  inline void ResetTurnNumber()      {turnsTaken = 0;}
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
  G4FieldManager*    zeroFieldManager;

  /// Initial bunch parameters
  G4ParticleDefinition* beamParticleDefinition;

  /// Reference beam energy
  G4double beamMomentum, beamKineticEnergy;

  /// Particle energy
  G4double particleMomentum, particleKineticEnergy;

  /// Particle name
  G4String particleName;
  
  /// Beamline length in mm
  G4double sMax;
  
  /// The maximum s in mm such that there is an integer number of
  /// elossHistoBinWidths along the line. Used for histogramming purposes.
  G4double sMaxHistograms;

  G4int nBins; ///< Number of bins for each histogram required.

  /// Calculate the number of bins and required maximum s.
  void CalculateHistogramParameters();
  
  ///@{ Magnet geometry
  BDSMagnetGeometryType magnetGeometryType;
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
  
  /// rotation
  void InitRotationMatrices();
  G4RotationMatrix* rotY90;
  G4RotationMatrix* rotYM90;
  G4RotationMatrix* rotX90;
  G4RotationMatrix* rotXM90;
  G4RotationMatrix* rotYM90X90;
  G4RotationMatrix* rotYM90XM90;

  void InitVisAttributes();
  G4VisAttributes* invisibleVisAttr;
  G4VisAttributes* visibleDebugVisAttr;

  void InitDefaultUserLimits();
  G4UserLimits* defaultUserLimits;
  
  /// Turn Control
  G4int    turnsTaken;
  ///@{ Teleporter offset corrections
  G4ThreeVector teleporterdelta;
  G4double      teleporterlength;
  ///@}
  
  /// initial particle for production of sampler hit
  BDSParticle initialPoint;

  BDSOutputFormat outputFormat;
};

inline void BDSGlobalConstants::SetSMax(G4double sMaxIn)
{
  sMax = sMaxIn;
  CalculateHistogramParameters();
}

inline void BDSGlobalConstants::SetBeamKineticEnergy(G4double val)
{beamKineticEnergy = val;}

inline void BDSGlobalConstants::SetBeamMomentum(G4double val)
{beamMomentum = val;}

inline void BDSGlobalConstants::SetParticleDefinition(G4ParticleDefinition* aBeamParticleDefinition)
{beamParticleDefinition = aBeamParticleDefinition;}

inline void BDSGlobalConstants::SetParticleName(G4String aParticleName)
{particleName = aParticleName;}

inline void BDSGlobalConstants::SetLaserwireWavelength(G4String aName, G4double aWavelength)
{lwWavelength[aName]=aWavelength;}

inline void BDSGlobalConstants::SetLaserwireDir(G4String aName, G4ThreeVector aDirection)
{lwDirection[aName]=aDirection;}

inline void BDSGlobalConstants::SetTeleporterDelta(G4ThreeVector newteleporterdelta)
{teleporterdelta = newteleporterdelta;}

inline void BDSGlobalConstants::SetTeleporterLength(G4double newteleporterlength)
{teleporterlength = newteleporterlength;}

inline void BDSGlobalConstants::SetParticleKineticEnergy(G4double val)
{particleKineticEnergy = val;}

inline void BDSGlobalConstants::SetParticleMomentum(G4double val)
{particleMomentum = val;}

inline void BDSGlobalConstants::SetInitialPoint(BDSParticle& particle)
{initialPoint = particle;}

#endif
