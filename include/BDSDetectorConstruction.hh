/** BDSIM, v0.1   

Last modified 15.11.2005 by Ilya Agapov

**/

//==============================================================

#ifndef BDSDetectorConstruction_h
#define BDSDetectorConstruction_h 

#include "BDSGlobalConstants.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

#include "BDSWorld.hh"
#include "BDSMaterials.hh"
#include "BDSBeamline.hh"

#include "G4Region.hh"

#include "G4GeometrySampler.hh"

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

class ElementList;

//==============================================================

class BDSDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  
  BDSDetectorConstruction();
  ~BDSDetectorConstruction();

public:
     
  virtual G4VPhysicalVolume* Construct();
  void SetMagField(const G4double afield);

  inline G4VPhysicalVolume* GetWorldVolume(){
    return physiWorld;
  }

  // inline G4GeometrySampler* GetGeometrySampler(){
  //   return itsGeometrySampler;
  // }

public:

  G4double* GetWorldSize();
  G4double GetWorldSizeX();
  G4double GetWorldSizeY();
  G4double GetWorldSizeZ();
  
private:
  /// assignment and copy constructor not implemented nor used
  BDSDetectorConstruction& operator=(const BDSDetectorConstruction&);
  BDSDetectorConstruction(BDSDetectorConstruction&);

  G4bool verbose;

  void SetWorldSizeX(G4double);
  void SetWorldSizeY(G4double);
  void SetWorldSizeZ(G4double);


  G4int    gflash;
  G4double gflashemax;
  G4double gflashemin;
    
  G4GeometrySampler* itsGeometrySampler;

  G4Region* precisionRegion;
  G4Region* gasRegion;

  //  BDSWorld* _world;

  G4Box*             solidWorld;    //pointer to the solid World 
  G4LogicalVolume*   logicWorld;    //pointer to the logical World
  G4VPhysicalVolume* physiWorld;    //pointer to the physical World
  std::vector<G4double> itsWorldSize;
  std::vector< G4VPhysicalVolume * > fPhysicalVolumeVector; //a vector with all the physical volumes

  void DefineMaterials();

  G4VPhysicalVolume* ConstructBDS(ElementList& beamline_list);
  G4UniformMagField* magField;      //pointer to the magnetic field
  G4UserLimits* BDSUserLimits;

  G4VSensitiveDetector *  BDSSensitiveDetector;
  
  // Gflash members                                                                                                                                                     
  std::vector<GFlashHomoShowerParameterisation*> theParameterisation;
  GFlashHitMaker *theHitMaker;
  GFlashParticleBounds *theParticleBounds;
  GFlashParticleBounds *theParticleBoundsVac;
  std::vector<BDSShowerModel*> theFastShowerModel;
  std::vector<G4Region*> gFlashRegion;

  G4RotationMatrix* _globalRotation;
};

#endif

