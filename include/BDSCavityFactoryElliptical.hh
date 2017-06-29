#ifndef BDSCAVITYFACTORYELLIPTICAL_H
#define BDSCAVITYFACTORYELLIPTICAL_H

#include "BDSCavityFactoryBase.hh"

#include "G4String.hh"

class BDSCavity;
class BDSCavityInfo;

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
				  const BDSCavityInfo* info);
};

#endif
