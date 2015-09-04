#ifndef BDSCavity_h
#define BDSCavity_h

#include "BDSAcceleratorComponent.hh"

#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "globals.hh"

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


  void PlaceComponents();

  virtual void BuildContainerLogicalVolume(); 


  void BuildEllipticalCavityGeometry(/*G4double irisYSemiAxis, //iris ellipse vertical semiaxis.
				     G4double irisZSemiAxis, //iris ellipse horizontal semiaxis
				     G4double equatorYSemiAxis, //equator ellipse vertical semiaxis
				     G4double equatorZSemiAxis, //equator ellipse horizontal semiaxis
				     G4double tangentAngle, //Angle of line connecting ellipses to the vertical.  Ought to be the common tangent with most negative gradient for reasonable output
				     G4int noPoints //number of points forming each curved section.  Total points entire shape will be 4*nopoints.
				     */);
  void BuildPillBoxCavityGeometry();
  
  //Something here for N cells??
  
  //Solids  Do I need the G4VSolids??  
  G4VSolid* cavitySolid; //Set by e.g BuildEllipticalCavityGeometry
  G4VSolid* innerSolid; //Used only PillBox atm Should expand to elliptical as well
  G4VSolid* vacuumSolid; //Set by e.g BuildEllipticalCavityGeometry

  G4LogicalVolume* cavityLV; //Set at same time as cavitySolid
  G4LogicalVolume* vacuumLV; //Set at same time as vacuumSolid

  G4Material* cavityMaterial;
  G4Material* vacuumMaterial;

  G4double irisRadius;
  G4double equatorRadius;
  G4double thickness;
 

};

#endif
