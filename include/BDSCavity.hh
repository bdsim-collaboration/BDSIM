#ifndef BDSCAVITY
#define BDSCAVITY

#include "BDSAcceleratorComponent.hh" //To inherit.  BDSAcceleratorComponent 
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"

//At the moment just considering getting shape and so on...  will have to edit later for field
class BDSCavity: public BDSAcceleratorComponent
{
public:

  BDSCavity(
	    G4String name, 
	    G4String type, 
    	    G4double length,
	    G4String cavityMaterial,
	    G4String vacuumMaterial,
	    G4double equatorRadius, //Largest radial distance from z axis
	    G4double irisRadius, //radius at ends
	    G4double thickness 
	    );
  

private:
  BDSCavity();
  
protected:


  

  virtual void BuildContainerLogicalVolume(); 


  void BuildEllipticalCavityGeometry(G4double irisYSemiAxis, //iris ellipse vertical semiaxis.
				     G4double irisZSemiAxis, //iris ellipse horizontal semiaxis
				     G4double equatorYSemiAxis, //equator ellipse vertical semiaxis
				     G4double equatorZSemiAxis, //equator ellipse horizontal semiaxis
				     G4double tangentAngle, //Angle of line connecting ellipses to the vertical.  Ought to be the common tangent with most negative gradient for reasonable output
				     G4int noPoints //number of points forming each curved section.  Total points entire shape will be 4*nopoints.
				     );
  
  //Something here for N cells??
  
  //Solids
  G4VSolid *cavitySolid;
  G4VSolid *vacuumSolid;

  G4LogicalVolume *cavityLV;
  G4LogicalVolume *vacuumLV;

  G4String *cavityMaterial;
  G4String *vacuumMaterial;

  G4double *irisRadius;
  G4double *equatorRadius;
  G4double *thickness;
 

};

#endif
