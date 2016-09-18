#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactoryGDML.hh"
#include "BDSGlobalConstants.hh"

#include "globals.hh"
#include "G4GDMLParser.hh"
#include "G4LogicalVolume.hh"

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

BDSGeometryExternal* BDSGeometryFactoryGDML::Build(G4String fileName)
{
  std::vector<G4VPhysicalVolume*> physicalVolumes;
  std::vector<G4LogicalVolume*>   logicalVolumes;
  
  G4GDMLParser* parser = new G4GDMLParser();
  parser->Read(fileName, /*validate=*/true);
  
  G4LogicalVolume* containerLV    = parser->GetWorldVolume()->GetLogicalVolume();
  G4VSolid*        containerSolid = containerLV->GetSolid();

  for (G4int i = 0; i < containerLV->GetNoDaughters(); i++)
    {
      auto aPV = containerLV->GetDaughter(i);
      physicalVolumes.push_back(aPV);
      logicalVolumes.push_back(aPV->GetLogicalVolume());
    }

  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());

  BDSExtent defaultExtent = BDSExtent(1*CLHEP::m, 1*CLHEP::m, 1*CLHEP::m);
  
  BDSGeometryExternal* result = new BDSGeometryExternal(containerSolid, containerLV, defaultExtent);
  result->RegisterLogicalVolume(logicalVolumes);
  result->RegisterPhysicalVolume(physicalVolumes);

  delete parser;
  return result;
}
