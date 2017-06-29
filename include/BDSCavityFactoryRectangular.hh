#ifndef BDSCAVITYFACTORYRECTANGULAR_H
#define BDSCAVITYFACTORYRECTANGULAR_H

#include "BDSCavityFactoryBase.hh"

#include "G4String.hh"

class BDSCavity;
class BDSCavityInfo;

/**
 * @brief Factory for elliptical SRF cavity geometry.
 *
 * @author Stuart Walker.
 */

class BDSCavityFactoryRectangular: public BDSCavityFactoryBase
{
public:
  BDSCavityFactoryRectangular();
  virtual ~BDSCavityFactoryRectangular();

  virtual BDSCavity* CreateCavity(G4String             name,
				  const BDSCavityInfo* info);
};

#endif
