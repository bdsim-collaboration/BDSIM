#ifndef BDSCAVITYFACTORYPILLBOX_H
#define BDSCAVITYFACTORYPILLBOX_H

#include "BDSCavityFactoryBase.hh"

#include "G4String.hh"

class BDSCavity;
class BDSCavityInfo;

/**
 * @brief Factory for elliptical SRF cavity geometry.
 *
 * @author Stuart Walker.
 */

class BDSCavityFactoryPillBox: public BDSCavityFactoryBase
{
public:
  BDSCavityFactoryPillBox();
  virtual ~BDSCavityFactoryPillBox();

  virtual BDSCavity* CreateCavity(G4String             name,
				  const BDSCavityInfo* info);
};

#endif
