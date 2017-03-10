#ifdef USE_GDML
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactoryGDML.hh"
#include "BDSGeometryInspector.hh"
#include "BDSGlobalConstants.hh"

#include "globals.hh"
#include "G4GDMLParser.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

#include <utility>
#include <vector>

class G4VSolid;

BDSGeometryFactoryGDML* BDSGeometryFactoryGDML::instance = nullptr;

BDSGeometryFactoryGDML::BDSGeometryFactoryGDML()
{;}

BDSGeometryFactoryGDML::~BDSGeometryFactoryGDML()
{
  instance = nullptr;
}

BDSGeometryFactoryGDML* BDSGeometryFactoryGDML::Instance()
{
  if (!instance)
    {instance = new BDSGeometryFactoryGDML();}
  return instance;
}

BDSGeometryExternal* BDSGeometryFactoryGDML::Build(G4String fileName,
						   std::map<G4String, G4Colour*>* mapping,
						   G4double /*suggestedLength*/,
						   G4double /*suggestedOuterDiameter*/)
{
  CleanUp();
  
  G4GDMLParser* parser = new G4GDMLParser();
  parser->Read(fileName, /*validate=*/true);

  G4VPhysicalVolume* containerPV = parser->GetWorldVolume();
  G4LogicalVolume*   containerLV = containerPV->GetLogicalVolume();
  G4VSolid*       containerSolid = containerLV->GetSolid();
  G4ThreeVector gdmlWorldOrigin(0,0,0);
  if (containerPV->GetName() == "world_volume_lv_PV")
  {
    gdmlWorldOrigin = parser->GetPosition("PygdmlOrigin"); // TODO check if Pygdml geometry
    gdmlWorldOrigin[2] = 0.0;
  }

  // record all pvs and lvs used in this loaded geometry
  std::vector<G4VPhysicalVolume*> pvs;
  std::vector<G4LogicalVolume*>   lvs;
  GetAllLogicalAndPhysical(containerPV, pvs, lvs);

  auto vises = ApplyColourMapping(lvs, mapping);

  /// Now overwrite container lv vis attributes
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());

  std::pair<BDSExtent, BDSExtent> outerInner = BDS::DetermineExtents(containerSolid);
  
  BDSGeometryExternal* result = new BDSGeometryExternal(containerSolid, containerLV,
                                                        outerInner.first,  /*outer*/
                                                        outerInner.second, /*inner*/
                                                        gdmlWorldOrigin);
  result->RegisterLogicalVolume(lvs);
  result->RegisterPhysicalVolume(pvs);
  result->RegisterVisAttributes(vises);

  delete parser;
  return result;
}

void BDSGeometryFactoryGDML::GetAllLogicalAndPhysical(const G4VPhysicalVolume*         volume,
						      std::vector<G4VPhysicalVolume*>& pvs,
						      std::vector<G4LogicalVolume*>&   lvs)
{
  const auto& lv = volume->GetLogicalVolume();
  lvs.push_back(lv);
  for (G4int i = 0; i < lv->GetNoDaughters(); i++)
    {
      const auto& pv = lv->GetDaughter(i);
      pvs.push_back(pv);
      lvs.push_back(lv);
      GetAllLogicalAndPhysical(pv, pvs, lvs); // recurse into daughter
    }
}
#endif
