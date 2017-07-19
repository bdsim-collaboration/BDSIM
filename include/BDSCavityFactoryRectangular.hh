#ifndef BDSCAVITYFACTORYRECTANGULAR_H
#define BDSCAVITYFACTORYRECTANGULAR_H

#include "BDSCavityFactoryBase.hh"

#include "globals.hh"
#include "G4String.hh"

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

  virtual G4double CreateSolids(G4String             name,
				G4double             totalChordLength,
				const BDSCavityInfo* info);
};

#endif
