#include "BDSColours.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactoryBase.hh"

#include "globals.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include <map>

BDSGeometryFactoryBase::BDSGeometryFactoryBase()
{;}

BDSGeometryFactoryBase::~BDSGeometryFactoryBase()
{;}

void BDSGeometryFactoryBase::ColourCode(BDSGeometryExternal* /*component*/,
					std::map<G4String, G4Colour*>* /*mapping*/)
{return;}

void BDSGeometryFactoryBase::SetVisibleWithAlpha(BDSGeometryExternal* component,
						 G4double             alpha)
{
  G4VisAttributes* newVis = new G4VisAttributes(*BDSColours::Instance()->GetColour("gdml"));
  newVis->SetVisibility(true);
  G4bool registered = false;

  std::map<const G4VisAttributes*, G4VisAttributes*> alreadySetVis;
  for (auto& lv : component->GetAllLogicalVolumes())
    {
      const auto& vis = lv->GetVisAttributes();
      if (vis)
	{// update vis attributes while saving memory - they could all have the same vis attributes or mix
	  auto search = alreadySetVis.find(vis);
	  if (search != alreadySetVis.end())
	    {// was found
	      lv->SetVisAttributes(search->second);
	    }
	  else
	    {// wasn't found.  Can't fiddle a vis attributs, so copy and reregister with new alpha
	      G4VisAttributes* localNewVis = new G4VisAttributes(*vis);
	      localNewVis->SetVisibility(true);
	      const G4Colour& c = vis->GetColour();
	      G4Colour newColour = G4Colour(c.GetRed(), c.GetGreen(), c.GetBlue(), alpha);
	      localNewVis->SetColour(newColour);
	      lv->SetVisAttributes(localNewVis);
	      alreadySetVis[vis] = localNewVis;
	    } 
	}
      else
	{	  
	  lv->SetVisAttributes(newVis);
	  if (!registered)
	    {
	      component->RegisterVisAttributes(newVis);
	      registered = true;
	    }
	}
    }

  if (!registered)
    {// wasn't used, so clean up
      delete newVis;
    }
}
