#ifndef BDSDETECTORCONSTRUCTION_H
#define BDSDETECTORCONSTRUCTION_H 

#include "globals.hh" // geant4 types / globals
#include "G4Version.hh"
#include "G4VUserDetectorConstruction.hh"

#include <list>
#include <string>
#include <vector>

class GFlashHomoShowerParameterisation;
class GFlashHitMaker;
class GFlashParticleBounds;
class G4LogicalVolume;
class G4Region;
class G4VPhysicalVolume;
class G4UserLimits;
class G4VSensitiveDetector;

class BDSFieldObjects;
class BDSShowerModel;

#if G4VERSION_NUMBER > 1009
class BDSBOptrMultiParticleChangeCrossSection;
#endif

/**
 * @brief Class that constructs a Geant4 model of an accelerator.
 *
 * Mandatory class that must be supplied for a valid Geant4 simulation.
 * As construction is based on user input (via the parser) for a potentially
 * large number of items, the construction process is factorised into key steps
 * with member functions and makes use of factories for components and tunnel segments.
 * 
 * Maintained by Laurie Nevay & Jochem Snuverink
 */

class BDSDetectorConstruction: public G4VUserDetectorConstruction
{
public:
  BDSDetectorConstruction();
  virtual ~BDSDetectorConstruction();

  /// Overridden Geant4 method that must be implemented. Constructs the Geant4 geometry
  /// and returns the finished world physical volume.
  virtual G4VPhysicalVolume* Construct();

  virtual void ConstructSDandField();

  /// Create biasing operations 
  void BuildPhysicsBias();
  
private:
  /// assignment and copy constructor not implemented nor used
  BDSDetectorConstruction& operator=(const BDSDetectorConstruction&);
  BDSDetectorConstruction(BDSDetectorConstruction&);

  /// Create and set parameters for various G4Regions
  void InitialiseRegions();
  
  /// Convert the parser beamline_list to BDSAcceleratorComponents with help of BDSComponentFactory
  /// and put in BDSBeamline container that calculates coordinates and extent of beamline
  void BuildBeamline();

  /// Build the tunnel around the already constructed flat beam line.
  void BuildTunnel();
  
  /// Build the world volume using the extent of the BDSBeamline instance created
  /// in BuildBeamline()
  void BuildWorld();
  
  /// Iterate over the beamline and place each BDSAcceleratorComponent in the world volume
  void ComponentPlacement();

  /// Initialise GFlash particle bounds - parameterised energy deposition.
  void InitialiseGFlash();
  
  /// Function to add the volume to the gflash parameterisation model
  void SetGFlashOnVolume(G4LogicalVolume* volume);

#if G4VERSION_NUMBER > 1009
  /// Function that creates physics biasing cross section
  BDSBOptrMultiParticleChangeCrossSection* BuildCrossSectionBias(const std::list<std::string>& biasList,
								 G4String defaultBias,
								 G4String elementName);

  /// List of bias objects - for memory management
  std::vector<BDSBOptrMultiParticleChangeCrossSection*> biasObjects;
#endif

#ifdef BDSDEBUG
  bool debug = true;
#else
  bool debug = false;
#endif

  G4bool verbose;
  G4bool checkOverlaps;

  /// World physical volume
  G4VPhysicalVolume* worldPV;

  // All fields
  std::vector<BDSFieldObjects*> fields;
  
  // Gflash members
  std::vector<GFlashHomoShowerParameterisation*> theParameterisation;
  GFlashHitMaker *theHitMaker;
  GFlashParticleBounds *theParticleBounds;
  //  GFlashParticleBounds *theParticleBoundsVac;
  std::vector<BDSShowerModel*> theFastShowerModel;
  std::vector<G4Region*> gFlashRegion;

  /// Whether or not to use the GFlash shower parameterisation.
  G4bool gflash;
};

#endif

