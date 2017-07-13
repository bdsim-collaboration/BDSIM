#ifndef BDSCAVITYFACTORYELLIPTICAL_H
#define BDSCAVITYFACTORYELLIPTICAL_H

#include "BDSCavityFactoryBase.hh"

#include "globals.hh"
#include "G4String.hh"

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
  
  virtual G4double CreateSolids(G4String             name,
				G4double             totalChordLength,
				const BDSCavityInfo* info);

  /// Overload base class method, but use base class method with different colour.
  virtual void SetVisAttributes(G4String colourName);
};

#endif
