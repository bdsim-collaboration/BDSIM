#include "BDSCavity.hh"


class BDSCavityRF: public BDSCavity
{
public:
  BDSCavityRF(G4String name, 
		      G4String type, 
		      G4double length,
		      G4String cavityMaterialIn,
		      G4String vacuumMaterialIn,
		      G4double equatorRadiusIn //Lrgst r from z axis
	      );

  virtual void Build();



}
