#include "BDSCavityRF.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSCavity.hh"

#include "globals.hh"
#include "G4Material.hh"


BDSCavityRF::BDSCavityRF(G4String name, 
			 G4String type,  //??
			 G4double length,
			 G4Material* cavityMaterialIn,
			 G4Material* vacuumMaterialIn,
			 G4double equatorRadiusIn, //lrgst radial distance from z axs
			 G4double irisRadiusIn, //radius at ends (assuming equal..)
			 G4double thicknessIn
			 ):
    BDSCavity(name, type, length, cavityMaterialIn, vacuumMaterialIn, equatorRadiusIn, irisRadiusIn, thicknessIn)

{
}

void BDSCavityRF::Build()
{
  //BuildEllipticalCavityGeometry();
  BuildPillBoxCavityGeometry();
  BDSAcceleratorComponent::Build();
  PlaceComponents();
}
