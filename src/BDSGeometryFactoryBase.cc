#include "BDSColours.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactoryBase.hh"

#include "globals.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4String.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include <map>
#include <vector>

BDSGeometryFactoryBase::BDSGeometryFactoryBase()
{;}

BDSGeometryFactoryBase::~BDSGeometryFactoryBase()
{;}

std::vector<G4VisAttributes*> BDSGeometryFactoryBase::ApplyColourMapping(std::vector<G4LogicalVolume*>& lvs,
									 std::map<G4String, G4Colour*>* mapping)
{
  std::vector<G4VisAttributes*> visAttributes; // empty vector

  // no mapping, just return.
  if (!mapping)
    {return visAttributes;}
  
  if (mapping->size() == 1)
    {// only one colour for all - simpler
      G4VisAttributes* vis = new G4VisAttributes(*BDSColours::Instance()->GetColour("gdml"));
      vis->SetVisibility(true);
      visAttributes.push_back(vis);
      for (auto lv : lvs)
	{lv->SetVisAttributes(*vis);}
      return visAttributes;
    }

  // else iterate over all lvs and required vis attributes
  // prepare required vis attributes
  std::map<G4String, G4VisAttributes*> attMap;
  visAttributes.reserve(mapping->size()); // expand but don't initialise
  for (const auto& it : *mapping)
    {
      G4VisAttributes* vis = new G4VisAttributes(*(it.second));
      vis->SetVisibility(true);
      visAttributes.push_back(vis);
      attMap[it.first] = vis;
    }

  for (auto lv : lvs)
    {// iterate over all volumes
      const G4String& name = lv->GetName();
      for (const auto& it : attMap)
	{// iterate over all mappings to find first one that matches substring
	  if (name.contains(it.first))
	    {
	      lv->SetVisAttributes(it.second);
	      break;
	    }
	}
    }
  return visAttributes;
}
