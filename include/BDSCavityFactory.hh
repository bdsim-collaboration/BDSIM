#ifndef BDSCAVITYFACTORY_H
#define BDSCAVITYFACTORY_H

#include "BDSCavityType.hh"

#include "globals.hh"
#include "G4String.hh"

class BDSCavity;
class BDSCavityFactoryBase;
class BDSCavityFactoryElliptical;
class BDSCavityFactoryRectangular;
class BDSCavityFactoryPillBox;
class BDSCavityInfo;

class G4Material;

/**
 * @brief Interface to create any RF cavity geometry.
 *
 * @author Laurie Nevay
 */

class BDSCavityFactory
{
public:
  static BDSCavityFactory* Instance(); ///< Singleton accessor.

  /// Reset static singleton instance to nullptr and delete the singleton
  /// factories for different beam pipe styles. Delete them here as they
  /// should only be accessed through this class.
  ~BDSCavityFactory();

  /// Main function to create a piece of cavity geometry.
  BDSCavity* CreateCavity(G4String             name,
			  G4double             totalChordLength,
			  const BDSCavityInfo* info,
			  G4Material*          vacuumMaterial);

private:
  BDSCavityFactory(); ///< Private constructor as singleton pattern.

  static BDSCavityFactory* instance; ///< Singleton instance.

  BDSCavityFactoryBase* GetAppropriateFactory(const BDSCavityType cavityType);

  /// @{ Point to factory this main interface owns.
  BDSCavityFactoryElliptical*  elliptical;
  BDSCavityFactoryRectangular* rectangular;
  BDSCavityFactoryPillBox*     pillBox;
  /// @}
};

#endif
