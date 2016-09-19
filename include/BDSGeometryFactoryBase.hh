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

class BDSGeometryFactoryBase
{
public:
  BDSGeometryFactoryBase();
  virtual ~BDSGeometryFactoryBase();

  /// Main method to load and construct geometry.
  virtual BDSGeometryExternal* Build(G4String fileName,
				     std::map<G4String, G4Colour*>* colourMapping = nullptr) = 0;

  
  virtual std::vector<G4VisAttributes*> ApplyColourMapping(std::vector<G4LogicalVolume*>& lvs,
							   std::map<G4String, G4Colour*>* mapping);
};

#endif
