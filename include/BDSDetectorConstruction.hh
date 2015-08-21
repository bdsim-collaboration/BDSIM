#ifndef BDSDetectorConstruction_h
#define BDSDetectorConstruction_h 

#include "BDSGlobalConstants.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

#include "G4Region.hh"

//GFlash parameterisation
#include "GFlashHomoShowerParameterisation.hh"
#include "BDSShowerModel.hh"
#include "GFlashHitMaker.hh"
#include "GFlashParticleBounds.hh"


class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4UniformMagField;
class G4UserLimits;
class G4VSensitiveDetector;

class BDSBeamline;
class ElementList;

class BDSDetectorConstruction: public G4VUserDetectorConstruction
{
public:
  BDSDetectorConstruction();
  ~BDSDetectorConstruction();

  /// Overridden Geant4 method that must be implemented. Constructs the Geant4 geometry
  /// and returns the finished world physical volume.
  virtual G4VPhysicalVolume* Construct();
  
private:
  /// assignment and copy constructor not implemented nor used
  BDSDetectorConstruction& operator=(const BDSDetectorConstruction&);
  BDSDetectorConstruction(BDSDetectorConstruction&);

  /// Create and set parameters for various G4Regions
  void InitialiseRegions();
  
  /// Convert the parser beamline_list to BDSAcceleratorComponents with help of BDSComponentFactory
  /// and put in BDSBeamline container that calcualtes coordinates and extent of beamline
  void BuildBeamline();

  /// Build the tunnel around the already constructed flat beam line.
  void BuildTunnel();
  
  /// Build the world volume using the extent of the BDSBeamline instance created
  /// in BuildBeamline()
  void BuildWorld();
  
  /// Iterate over the beamline and place each BDSAcceleratorComponent in the world volume
  void ComponentPlacement();

  /// Create biasing operations 
  void BuildPhysicsBias();

  /// Initialise GFlash particle bounds - parameterised energy deposition.
  void InitialiseGFlash();
  
  /// Function to add the volume to the gflash parameterisation model
  void SetGFlashOnVolume(G4LogicalVolume* volume);

  G4bool verbose;
  G4bool checkOverlaps;

  G4Region*          precisionRegion;
  G4Region*          gasRegion;

  /// World physical volume
  G4VPhysicalVolume* worldPV;

  G4UniformMagField* magField;      //pointer to the magnetic field
  
  // Gflash members
  std::vector<GFlashHomoShowerParameterisation*> theParameterisation;
  GFlashHitMaker *theHitMaker;
  GFlashParticleBounds *theParticleBounds;
  //  GFlashParticleBounds *theParticleBoundsVac;
  std::vector<BDSShowerModel*> theFastShowerModel;
  std::vector<G4Region*> gFlashRegion;
};

#endif

