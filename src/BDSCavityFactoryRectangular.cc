#include "BDSCavityFactoryRectangular.hh"


BDSCavityFactoryRectangular::BDSCavityFactoryRectangular()
{;}

BDSCavityFactoryRectangular::~BDSCavityFactoryRectangular()
{;}

BDSCavity* BDSCavityFactoryRectangular::CreateCavity(G4String             name,
						     G4double             totalChordLength,
						     const BDSCavityInfo* info,
						     G4Material*          vacuumMaterial)
{
  return nullptr;
}
