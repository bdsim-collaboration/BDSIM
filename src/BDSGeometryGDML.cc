#ifdef USE_GDML
#include "BDSColours.hh"
#include "BDSGeometryGDML.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"

#include "G4GDMLParser.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include <cstdlib>
#include <cstring>

BDSGeometryGDML::BDSGeometryGDML(G4String GDMLfileIn):
  markerVol(nullptr),
  gdmlWorld(nullptr)
{
  GDMLfile = GDMLfileIn;
}

BDSGeometryGDML::~BDSGeometryGDML()
{;}

void BDSGeometryGDML::Construct(G4LogicalVolume *marker)
{
  markerVol = marker;
  G4GDMLParser *parser = new G4GDMLParser();
  parser->Read(GDMLfile);
  
  gdmlWorld = parser->GetWorldVolume()->GetLogicalVolume();

  G4VisAttributes* visAtt = new G4VisAttributes(*BDSColours::Instance()->GetColour("gdml"));
  visAtt->SetVisibility(false);

  G4VisAttributes* visAtt2 = new G4VisAttributes(*BDSColours::Instance()->GetColour("gdml"));
  visAtt2->SetVisibility(true);

  for (int i=0; i<gdmlWorld->GetNoDaughters(); i++){

    // Set visibility 
    gdmlWorld->GetDaughter(i)->GetLogicalVolume()->SetVisAttributes(visAtt2);

    // Get all daugters and add to logical and sensitive vols
    logicalVols.push_back(gdmlWorld->GetDaughter(i)->GetLogicalVolume());
    sensitiveVols.push_back(gdmlWorld->GetDaughter(i)->GetLogicalVolume());
  }

  gdmlWorld->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());
  
  new G4PVPlacement(nullptr,
                    G4ThreeVector(0.,0.,0.),
                    gdmlWorld,
                    gdmlWorld->GetName()+"_pv",
                    markerVol,
                    false,
                    0,
                    BDSGlobalConstants::Instance()->CheckOverlaps());
}
#endif

std::vector<G4LogicalVolume*> BDSGeometryGDML::GetAllLogicalVolumes() const { 
  return logicalVols;
}
  
std::vector<G4LogicalVolume*> BDSGeometryGDML::GetAllSensitiveVolumes() const {
  return sensitiveVols;
}
