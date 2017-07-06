#ifndef BDSCAVITYFACTORYRECTANGULAR_H
#define BDSCAVITYFACTORYRECTANGULAR_H

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

class BDSCavityFactoryRectangular: public BDSCavityFactoryBase
{
public:
  BDSCavityFactoryRectangular();
  virtual ~BDSCavityFactoryRectangular();

  virtual BDSCavity* CreateCavity(G4String             name,
				  G4double             totalChordLength,
				  const BDSCavityInfo* info,
				  G4Material*          vacuumMaterial);

protected:
  /// Override base class method to build a box instead of a tubs.
  virtual void BuildContainerLogicalVolume(G4String name,
					   G4double chordLength,
					   G4double outerRadius);
};

#endif
