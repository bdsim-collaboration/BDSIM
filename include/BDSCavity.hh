#ifndef BDSCavity_h
#define BDSCavity_h

#include "BDSAcceleratorComponent.hh"

#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "globals.hh"


//Field header files
#include "G4FieldManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ChordFinder.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4ElectroMagneticField.hh"
#include "G4EqMagElectricField.hh"

//At the moment just considering getting shape and so on...  will have to edit later for field
class BDSCavity: public BDSAcceleratorComponent
{
public:
  BDSCavity(G4String name, 
	    G4String type, 
    	    G4double length,
	    G4Material* cavityMaterial,
	    G4Material* vacuumMaterial,
	    G4double equatorRadius, //Largest radial distance from z axis
	    G4double irisRadius, //radius at ends
	    G4double thickness 
	    );
  

private:
  BDSCavity();
  
protected:

  virtual void BuildField(); //Creates field.  Currently hardcoded pillboxfield.
  virtual void AttachField();  //attaches the created field to the vacuum logical volume
  void PlaceComponents();    //initializes physical volumes
  virtual void BuildContainerLogicalVolume(); //initializes logical volume in which everything else  is placed
  void BuildEllipticalCavityGeometry(/*G4double irisYSemiAxis, //iris ellipse vertical semiaxis.
				     G4double irisZSemiAxis, //iris ellipse horizontal semiaxis
				     G4double equatorYSemiAxis, //equator ellipse vertical semiaxis
				     G4double equatorZSemiAxis, //equator ellipse horizontal semiaxis
				     G4double tangentAngle, //Angle of line connecting ellipses to the vertical.  Ought to be the common tangent with most negative gradient for reasonable output
				     G4int noPoints //number of points forming each curved section.  Total points entire shape will be 4*nopoints.
				     */);  //Builds the geometry and the vacuum of this shape.
  void BuildPillBoxCavityGeometry();   //Builds the pillbox geometry and the vacuum to go indoors.

  //Something here for N cells??
  
  //Solids  Do I need the G4VSolids??  
  G4VSolid* cavitySolid; //Set by e.g BuildEllipticalCavityGeometry
  G4VSolid* innerSolid; //Used only PillBox atm Should expand to elliptical as well
  G4VSolid* vacuumSolid; //Set by e.g BuildEllipticalCavityGeometry

  G4LogicalVolume* cavityLV; //Set at same time as cavitySolid
  G4LogicalVolume* vacuumLV; //Set at same time as vacuumSolid

  G4Material* cavityMaterial;  //The material the cavity is to be made out of
  G4Material* vacuumMaterial;  //The material the vacuum is to be made of
 
  G4double equatorRadius;      //largest value of r from z.
  G4double irisRadius;         //radius of the iris (aperture).
  G4double thickness;          //thickness.  Constant thickness.  Any deviation is an artifact.
  
  //members for fields
  //How many of these do I actually need?  Some are used simply for immediately initializing something
  //else.  Still needed?
  G4int nvar = 8; //to integrate over position, momentum, energy and time (8)
  
  G4ElectroMagneticField* itsField; 
  G4FieldManager* itsFieldManager;    //field manager
  G4MagIntegratorStepper* itsStepper; //stepper
  G4ChordFinder* itsChordFinder;      //chord finder
  G4MagInt_Driver* itsIntgrDriver;  //Provides a driver that talks to the Integrator Stepper, and insures that 
  // the error is within acceptable bounds
  //do instasntiated methods stay instantiated further up the inheritance chain??

  
};

#endif
