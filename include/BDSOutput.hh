#ifndef BDSOUTPUT_H
#define BDSOUTPUT_H 

#include "BDSOutputStructures.hh"

#include "globals.hh"

#include <ctime>
#include <ostream>
#include <set>
#include <vector>

// forward declarations
template <class T> class G4THitsCollection;
class BDSEnergyCounterHit;
typedef G4THitsCollection<BDSEnergyCounterHit> BDSEnergyCounterHitsCollection;
class BDSEventInfo;
class BDSSamplerHit;
typedef G4THitsCollection<BDSSamplerHit> BDSSamplerHitsCollection;
class BDSTrajectory;
class BDSTrajectoryPoint;

class G4PrimaryVertex;

namespace GMAD {
  class OptionsBase;
}

/**
 * @brief Output base class that defines interface for all output types.
 */

class BDSOutput: protected BDSOutputStructures
{
public:
  /// Constructor with base file name (without extension or number suffix).
  BDSOutput(G4String fileName,
	    G4int    fileNumberOffset);
  virtual ~BDSOutput(){;}

  /// Open a new file.
  virtual void NewFile() = 0;

  /// Write any unwritten contents and close the currently open file. The instance
  /// should be safe to delete after calling this method.
  virtual void CloseFile() = 0;

  /// Setup any geometry dependent output structures in the output file - such
  /// as samplers. This is run after the geometry has been constructed and 'closed'.
  /// This also sets up histograms based along S now the beam line is known.
  virtual void InitialiseGeometryDependent();

  /// Fill the local structure options with the original ones from the parser.
  void FillOptions(const GMAD::OptionsBase* options);

  /// Fill the local structure model from the beam line and sampler registry.
  void FillModel();

  /// Fill the local structure with primary vertex information. A unique interface
  /// for the case of generating primaries only. This fills the primary structure,
  /// and calls WriteFileEventLevel() and then clears the structures. It therefore
  /// should not be used in conjunction with FillEvent().
  void FillEventPrimaryOnly(const G4double E,
			    const G4double x0,
			    const G4double y0,
			    const G4double z0,
			    const G4double xp,
			    const G4double yp,
			    const G4double zp,
			    const G4double t,
			    const G4double weight,
			    const G4int    PDGType,
			    const G4int    eventNumber,
			    const G4int    turnsTaken);
  
  /// Copy event information from Geant4 simulation structures to output structures.
  void FillEvent(const BDSEventInfo*                   info,
		 const G4PrimaryVertex*                vertex,
		 const BDSSamplerHitsCollection*       samplerHitsPlane,
		 const BDSSamplerHitsCollection*       samplerHitsCylinder,
		 const BDSEnergyCounterHitsCollection* energyLoss,
		 const BDSEnergyCounterHitsCollection* tunnelLoss,
		 const BDSTrajectoryPoint*             primaryHit,
		 const BDSTrajectoryPoint*             primaryLoss,
		 const std::vector<BDSTrajectory*>&    trajectories,
		 const G4int                           turnsTaken);

  /// Close a file and open a new one.
  void CloseAndOpenNewFile();

  /// Copy run information to output structure.
  void FillRun(const BDSEventInfo* info);
  
  /// Test whether a sampler name is invalid or not.
  static G4bool InvalidSamplerName(const G4String& samplerName);

  /// Feedback for protected names.
  static void PrintProtectedNames(std::ostream& out);

protected:
  /// Get the next file name based on the base file name and the accrued number of files.
  G4String GetNextFileName();

  /// Whether primaries are to be written to file or not.
  inline G4bool WritePrimaries() const {return writePrimaries;}

private:
  /// Enum for different types of sampler hits that can be written out.
  enum class HitsType {plane, cylinder};

  /// Enum for different types of energy loss that can be written out.
  enum class LossType {energy, tunnel};

  /// Write the options.
  virtual void WriteOptions() = 0;

  /// Write a representation of the samplers and beamline.
  virtual void WriteModel() = 0;

  /// Copy from local event structures to the actual file. Only event level
  /// structures are copied.
  virtual void WriteFileEventLevel() = 0;

  /// Copy from local run structures to the actual file. Only run level
  /// structures are copied.
  virtual void WriteFileRunLevel() = 0;

  /// Calculate the number of bins and required maximum s.
  void CalculateHistogramParameters();

  /// Create histograms.
  void CreateHistograms();
  
  /// Fill the local structure with primary vertex information. Utility function
  /// to translate from G4PrimaryVertex to simple numbers.
  void FillPrimary(const G4PrimaryVertex* vertex,
		   const G4int            eventNumber,
		   const G4int            turnsTaken);

  /// Fill the primary particle structure with the primary coordinates.
  void FillPrimary(const G4double E,
		   const G4double x0,
		   const G4double y0,
		   const G4double z0,
		   const G4double xp,
		   const G4double yp,
		   const G4double zp,
		   const G4double t,
		   const G4double weight,
		   const G4int    PDGType,
		   const G4int    eventNumber,
		   const G4int    turnsTaken);
  
  /// Fill event summary information.
  void FillEventInfo(const BDSEventInfo* info);

  /// Fill sampler hits into output structures.
  void FillSamplerHits(const BDSSamplerHitsCollection* hits,
		       const HitsType hType);

  /// Fill the hit where the primary particle impact.
  void FillPrimaryHit(const BDSTrajectoryPoint* phits);

  /// Fill a collection of energy hits into the appropriate output structure.
  void FillEnergyLoss(const BDSEnergyCounterHitsCollection *loss,
		      const LossType type);
  
  /// Fill the hit where the primary stopped being a primary.
  void FillPrimaryLoss(const BDSTrajectoryPoint* ploss);

  /// Copy a set of trajectories to the output structure.
  void FillTrajectories(const std::vector<BDSTrajectory*>& trajectories);

  /// Fill run level summary information.
  void FillRunInfo(const BDSEventInfo* info);

  /// No default constructor.
  BDSOutput() = delete;
  
  G4String filename;        ///< Base file name.
  G4int numberEventPerFile; ///< Number of events stored per file.
  G4int outputFileNumber;   ///< Number of output file.
  G4bool writePrimaries;    ///< Whether to write primaries or not.

  /// Invalid names for samplers - kept here as this is where the output structures are created.
  const static std::set<G4String> protectedNames;

  /// Whether the single 3D histogram will be built.
  G4bool useScoringMap;

  /// The maximum s in mm such that there is an integer number of
  /// elossHistoBinWidths along the line. Used for histogramming purposes.
  G4double sMaxHistograms;

  /// Number of bins for each histogram required.
  G4int nbins;
};

#endif
