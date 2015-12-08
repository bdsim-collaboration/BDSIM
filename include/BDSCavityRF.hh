#ifndef BDSCavityRF_h
#define BDSCavityRF_h

#include "BDSCavity.hh"

#include "G4Material.hh"
#include "globals.hh"


class BDSCavityRF: public BDSCavity
{
public:

  BDSCavityRF(G4String name, 
	      G4double length,
	      G4String type, 
	      G4Material* cavityMaterial,
	      G4Material* vacuumMaterial,
	      G4double cavityRadius, //Largest r from z axis
	      G4double irisRadius,
	      G4double thickness,
	      G4double frequency,
	      G4double phase,
	      G4String cavityModel
	      );

  virtual void Build(); //Builds geometry and vacuum, and builds and attaches field to vacuum
  virtual void BuildField(); //creates the field
  virtual void AttachField(); //attaches field to field manager and then field manager onto the vacuum logical volume

protected:
  G4double frequency;
  G4double phase;


};
#endif
