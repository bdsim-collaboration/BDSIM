#ifndef BDSCAVITY_H
#define BDSCAVITY_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 globals / types

class G4FieldManager;
class G4ChordFinder;
class G4ElectroMagneticField;
class G4EqMagElectricField;
class G4LogicalVolume;
class G4MagInt_Driver;
class G4MagIntegratorStepper;
class G4Material;
class G4VSolid;

class BDSCavityInfo;

/** 
 * @brief Cavity class
 *  
 * @author Stuart Walker
 */
class BDSCavity: public BDSAcceleratorComponent
{
public:
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

  void PlaceComponents();    ///<initializes physical volumes
  void BuildEllipticalCavityGeometry();  ///<Builds the geometry and the vacuum of this shape.
  void BuildPillBoxCavityGeometry();   ///<Builds the pillbox geometry and the vacuum to go indoors.
  
  //Solids  
  G4VSolid* cavitySolid = nullptr; ///<Set by e.g BuildEllipticalCavityGeometry
  G4VSolid* innerSolid  = nullptr; ///<Used only PillBox atm Should expand to elliptical as well
  G4VSolid* vacuumSolid = nullptr; ///<Set by e.g BuildEllipticalCavityGeometry

  G4LogicalVolume* cavityLV = nullptr; ///<Set at same time as cavitySolid
  G4LogicalVolume* vacuumLV = nullptr; ///<Set at same time as vacuumSolid
 
  G4double cavityRadius; ///<largest value of r from z.
  G4double irisRadius;   ///<radius of the iris (aperture).
  G4double thickness;    ///<thickness.  Constant thickness.  Any deviation is an artifact.
  
  G4int nvar = 8; ///<to integrate over position, momentum, energy and time (8)
  
  G4ElectroMagneticField* itsField = nullptr;    ///<field object
  G4FieldManager* itsFieldManager = nullptr;    ///<field manager
  G4MagIntegratorStepper* itsStepper = nullptr; ///<stepper
  G4ChordFinder* itsChordFinder = nullptr;      ///<chord finder
  G4MagInt_Driver* itsIntgrDriver = nullptr;  ///<Provides a driver that talks to the Integrator Stepper, and insures that 
  

  G4double       fieldAmplitude;
  BDSCavityInfo* cavityInfo = nullptr;
  
};

#endif
