#ifndef BDSCAVITYFACTORYBASE_H
#define BDSCAVITYFACTORYBASE_H

#include "globals.hh"
#include "G4String.hh"

class BDSCavity;
class BDSCavityInfo;

class G4Material;

/**
 * @brief Abstract base class for rf cavity geometry factories.
 *
 * @author Laurie Nevay
 */

class BDSCavityFactoryBase
{
public:
  virtual ~BDSCavityFactoryBase(){;}

  virtual BDSCavity* CreateCavity(G4String             name,
				  G4double             totalChordLength,
				  const BDSCavityInfo* info,
				  G4Material*          vacuumMaterial) = 0;

protected:
  BDSCavityFactoryBase();
};

#endif
