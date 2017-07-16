#ifndef BDSOutput_h
#define BDSOutput_h 

#include "globals.hh"

#include <ctime>
#include <ostream>
#include <set>
#include <string>
#include <vector>

// forward declarations
template <class T> class G4THitsCollection;
class BDSEnergyCounterHit;
typedef G4THitsCollection<BDSEnergyCounterHit> BDSEnergyCounterHitsCollection;
class BDSEventInfo;
class BDSOutputROOTEventInfo;
class BDSSamplerHit;
typedef G4THitsCollection<BDSSamplerHit> BDSSamplerHitsCollection;
class BDSTrajectory;
class BDSTrajectoryPoint;
typedef std::vector<BDSTrajectoryPoint*>  BDSTrajectoryPointsContainer;

class G4PrimaryVertex;

/**
 * @brief Output base class that defines interface for all output types.
 */

class BDSOutput
{
public:
  /// Constructor with base file name (without extentsion or number suffix.
  BDSOutput(G4String fileName,
	    G4int    fileNumberOffset);
  virtual ~BDSOutput(){;}

  /// Open a new file.
  virtual void NewFile() = 0;

  /// Setup any geometry dependent output structures in the output file - such
  /// as samplers. This is run after the geometry has been constructed and 'closed'.
  virtual void InitialiseGeometryDependent() = 0;

  /// Fill the local structure with primary vertex information. Utility function
  /// to translate from G4PrimaryVertex to simple numbers.
  void FillPrimary(const G4PrimaryVertex* vertex,
		   const G4int            eventNumber,
		   const G4int            turnsTaken);

  /// Fill the local structure with primary vertex information.
  virtual void FillPrimary(const G4double E,
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
			   const G4int    turnsTaken) = 0;

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

  /// Copy from local event structures to the actual file.  Ony event level
  /// structures are copied.
  virtual void WriteFileEventLevel() = 0;

  /// Copy from local run structures to the actual file.  Only run level
  /// structures are copied.
  virtual void WriteFileRunLevel() = 0;

  /// Write any unwritten contents and close the currently open file. The instance
  /// should be safe to delete after calling this method.
  virtual void CloseFile() = 0;
  
  /// Test whether a sampler name is invalid or not.
  static bool InvalidSamplerName(const G4String& samplerName);

  /// Feedback for protected names.
  static void PrintProtectedNames(std::ostream& out);

protected:
  /// Fill event summary information.
  virtual void FillEventInfo(const BDSEventInfo* info) = 0;

  /// Enum for different types of sampler hits that can be written out.
  enum class HitsType {plane, cylinder};

  /// Fill sampler hits into output structures.
  virtual void FillSamplerHits(const BDSSamplerHitsCollection* hits,
			       const HitsType type) = 0;

  /// Enum for different types of energy loss that can be written out.
  enum class LossType {energy, tunnel};

  /// Fill a collection of energy hits into the appropriate output structure.
  virtual void FillEnergyLoss(const BDSEnergyCounterHitsCollection *loss,
			      const LossType type) = 0;
  
  /// Fill the hit where the primary particle impact.
  virtual void FillPrimaryHit(const BDSTrajectoryPoint* phits) = 0;
  
  /// Fill the hit where the primary stopped being a primary.
  virtual void FillPrimaryLoss(const BDSTrajectoryPoint* ploss) = 0;

  /// Copy a set of trajectories to the output structure.
  virtual void FillTrajectories(const std::vector<BDSTrajectory*>& trajectories) = 0;

  /// Fill run level summary information.
  virtual void FillRunInfo(const BDSEventInfo* info) = 0;

  /// Clear the local structures in this class in preparation for a new event.
  virtual void ClearStructuresEventLevel(){;}

  /// Clear the local structures in this class in preparation for a new run.
  virtual void ClearStructuresRunLevel(){;}

  /// Whether primaries are to be written to file or not.
  inline G4bool WritePrimaries() const {return writePrimaries;}
  
  G4String filename;        ///< Base file name.
  G4int eventNumber;        ///< Current event number.
  G4int numberEventPerFile; ///< Number of events stored per file.
  G4int outputFileNumber;   ///< Number of output file.
  G4bool writePrimaries;    ///< Whether to write primaries or not.

private:
  BDSOutput() = delete;

  /// Invalid names for samplers - kept here as this is where the output structures are created.
  const static std::set<G4String> protectedNames;
};

#endif
