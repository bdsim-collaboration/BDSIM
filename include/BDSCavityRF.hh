#ifndef BDSCavityRF_h
#define BDSCavityRF_h

#include "BDSCavity.hh"

#include "G4Material.hh"
#include "globals.hh"


class BDSCavityRF: public BDSCavity
{
public:
  BDSCavityRF(G4String name, 
		G4String type, 
		G4double length,
		G4Material* cavityMaterial,
		G4Material* vacuumMaterial,
		G4double equatorRadius, //Lrgst r from z axis
		G4double irisRadius,
		G4double thickness
	      );

  virtual void Build();



};

#endif
