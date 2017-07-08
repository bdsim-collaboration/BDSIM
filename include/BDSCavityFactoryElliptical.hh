#ifndef BDSCAVITYFACTORYELLIPTICAL_H
#define BDSCAVITYFACTORYELLIPTICAL_H

#include "BDSCavityFactoryBase.hh"

#include "globals.hh"
#include "G4String.hh"

class BDSCavity;
class BDSCavityInfo;

class G4Material;

/**
 * @brief Factory for elliptical SRF cavity geometry.
 *
 * @author Stuart Walker.
 */

class BDSCavityFactoryElliptical: public BDSCavityFactoryBase
{
public:
  BDSCavityFactoryElliptical();
  virtual ~BDSCavityFactoryElliptical();

  virtual BDSCavity* CreateCavity(G4String             name,
				  G4double             totalChordLength,
				  const BDSCavityInfo* info,
				  G4Material*          vacuumMaterial);
};

#endif
