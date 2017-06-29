#ifndef BDSCAVITYFACTORYBASE_H
#define BDSCAVITYFACTORYBASE_H

#include "G4String.hh"

class BDSCavity;
class BDSCavityInfo;

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
				  const BDSCavityInfo* info) = 0;

protected:
  BDSCavityFactoryBase();

};

#endif
