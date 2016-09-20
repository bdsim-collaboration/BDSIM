#ifndef BDSGEOMETRYFACTORYBASE_H
#define BDSGEOMETRYFACTORYBASE_H

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <vector>

class G4Colour;
class G4LogicalVolume;
class G4VisAttributes;
class BDSGeometryExternal;

/**
 * @brief Base class for external geometry loading factories.
 * 
 * @author Laurie Nevay
 */

class BDSGeometryFactoryBase
{
public:
  BDSGeometryFactoryBase();
  virtual ~BDSGeometryFactoryBase();

  /// Main method to load and construct geometry.
  virtual BDSGeometryExternal* Build(G4String fileName,
				     std::map<G4String, G4Colour*>* colourMapping = nullptr) = 0;


  /// Apply a colour mapping to a set of logical volumes.  This applies a colour from the map
  /// if the key value is found as a substring or whole part of the logical volume name. Ie
  /// the BDSColour* (red) is defined to key 'quad' and any logical volume with 'quad'
  /// (case sensitive) will be set as red. Caches common G4VisAttributes (so no repeates for
  /// same colour) and returns those constructed. Map is searched through so key order gives
  /// precidence order.
  virtual std::vector<G4VisAttributes*> ApplyColourMapping(std::vector<G4LogicalVolume*>& lvs,
							   std::map<G4String, G4Colour*>* mapping);
};

#endif
