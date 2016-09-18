#ifndef BDSGEOMETRYFACTORYBASE_H
#define BDSGEOMETRYFACTORYBASE_H

#include "globals.hh"
#include "G4String.hh"

#include <map>

class G4Colour;
class BDSGeometryExternal;

class BDSGeometryFactoryBase
{
public:
  BDSGeometryFactoryBase();
  virtual ~BDSGeometryFactoryBase();

  /// Main method to load and construct geometry.
  virtual BDSGeometryExternal* Build(G4String fileName) = 0;

  /// Construct required visualisation attributes, register them to
  /// the component and attached them to logical volumes in the component
  /// whose name contains the substring of the map key.
  static void ColourCode(BDSGeometryExternal*           component,
			 std::map<G4String, G4Colour*>* mapping = nullptr);

  static void SetVisibleWithAlpha(BDSGeometryExternal* component,
				  G4double             alpha = 1.0);
};

#endif
