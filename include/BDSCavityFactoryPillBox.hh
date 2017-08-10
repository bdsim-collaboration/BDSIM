#ifndef BDSCAVITYFACTORYPILLBOX_H
#define BDSCAVITYFACTORYPILLBOX_H

#include "BDSCavityFactoryBase.hh"

#include "globals.hh"
#include "G4String.hh"

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

  virtual G4double CreateSolids(G4String             name,
				G4double             totalChordLength,
				const BDSCavityInfo* info);
};

#endif
