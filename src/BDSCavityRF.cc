#include "BDSCavityRF.hh"
#include "BDSFieldBuilder.hh"

#include "globals.hh"

BDSCavityRF::BDSCavityRF(G4String            name,
			 G4double            length,
			 const BDSFieldInfo* vacuumFieldIn):
  BDSCavity(name, length, vacuumFieldIn)
{;}

void BDSCavityRF::BuildField()
{
  BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumField,
							    vacuumLV,
							    true);
}


