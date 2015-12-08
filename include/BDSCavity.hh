#ifndef BDSCAVITY_H
#define BDSCAVITY_H

#include "BDSAcceleratorComponent.hh"
#include "BDSCavityInfo.hh"

#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "globals.hh" // geant4 globals / types

//Field header files
#include "G4FieldManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ChordFinder.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4ElectroMagneticField.hh"
#include "G4EqMagElectricField.hh"

class G4Material;

class BDSCavity: public BDSAcceleratorComponent
{
public:
  BDSCavity(G4String name, 
	    G4double length,
	    G4String type, 
	    G4Material* cavityMaterial,
	    G4Material* vacuumMaterial,
	    G4double cavityRadius, //Largest radial distance from z axis
	    G4double irisRadius, //radius at ends
	    G4double thickness,
	    G4String cavityModel);

  BDSCavity(G4String       name,
	    G4double       length,
	    G4double       fieldAmplitude,
	    BDSCavityInfo* cavityInfoIn);

  ~BDSCavity();

protected:
  BDSCavity();

  /// Creates container solid and logical volume
  virtual void BuildContainerLogicalVolume() override;

  /// The full construction sequence that calls all methods in order
  virtual void Build() override;
  
  /// Creates field objects - doesn't nothing by default and derived classes can override.
  virtual void BuildField();

  /// Attach the created field to the vacuum logical volume - only if field exists. Does
  /// nothing by default as no field by default.
  virtual void AttachField();

  void PlaceComponents();    //initializes physical volumes
  void BuildEllipticalCavityGeometry();  //Builds the geometry and the vacuum of this shape.
  void BuildPillBoxCavityGeometry();   //Builds the pillbox geometry and the vacuum to go indoors.
  
  //Solids  
  G4VSolid* cavitySolid; //Set by e.g BuildEllipticalCavityGeometry
  G4VSolid* innerSolid; //Used only PillBox atm Should expand to elliptical as well
  G4VSolid* vacuumSolid; //Set by e.g BuildEllipticalCavityGeometry

  G4LogicalVolume* cavityLV; //Set at same time as cavitySolid
  G4LogicalVolume* vacuumLV; //Set at same time as vacuumSolid

  G4Material* cavityMaterial;  //The material the cavity is to be made out of
  G4Material* vacuumMaterial;  //The material the vacuum is to be made of
 
  G4double cavityRadius;      //largest value of r from z.
  G4double irisRadius;         //radius of the iris (aperture).
  G4double thickness;          //thickness.  Constant thickness.  Any deviation is an artifact.
  G4String cavityModel; ///< cavity model name
  
  G4int nvar = 8; //to integrate over position, momentum, energy and time (8)
  
  G4ElectroMagneticField* itsField;    //field object
  G4FieldManager* itsFieldManager;    //field manager
  G4MagIntegratorStepper* itsStepper; //stepper
  G4ChordFinder* itsChordFinder;      //chord finder
  G4MagInt_Driver* itsIntgrDriver;  //Provides a driver that talks to the Integrator Stepper, and insures that 
  

  G4double       fieldAmplitude;
  BDSCavityInfo* cavityInfo;
  
};

#endif
