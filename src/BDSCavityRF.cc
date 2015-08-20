#include "BDSCavityElliptical.hh"


BDSCavityRF::BDSCavityRF(G4String name, 
			 G4String type,  //??
			 G4double length,
			 G4String cavityMaterialIn,
			 G4String vacuumMaterialIn,
			 G4double equatorRadiusIn, //lrgst radial distance from z axs
			 G4double irisRadiusIn, //radius at ends (assuming equal..)
			 G4double thicknessIn
			 ):
    BDSCavity(name, type, length, cavityMaterialIn, vacuumMaterialIn, equatorRadiusIn, irisRadiusIn, thicknessIn),

{
}

void Build()
{
  virtual
}
