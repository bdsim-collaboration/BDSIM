#ifdef USE_GDML
#include "BDSGlobalConstants.hh"
#include "BDSGeometryGDML.hh"
#include "BDSMaterials.hh"
#include "BDSMagnetColours.hh"
#include "BDSSDManager.hh"

#include "G4Colour.hh"
#include "G4GDMLParser.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include <cstdlib>
#include <cstring>

BDSGeometryGDML::BDSGeometryGDML(G4String GDMLfileIn):markerVol(NULL){
  GDMLfile = GDMLfileIn;
}

BDSGeometryGDML::~BDSGeometryGDML(){
}

void BDSGeometryGDML::Construct(G4LogicalVolume *marker){
  markerVol = marker;
  G4GDMLParser *parser = new G4GDMLParser();
  parser->Read(GDMLfile);
  
  G4LogicalVolume* gdmlWorld = parser->GetWorldVolume()->GetLogicalVolume();

  G4VisAttributes* visAtt = new G4VisAttributes(*BDSMagnetColours::Instance()->GetMagnetColour("gdml"));
  visAtt->SetVisibility(false);

  G4VisAttributes* visAtt2 = new G4VisAttributes(*BDSMagnetColours::Instance()->GetMagnetColour("gdml"));
  visAtt2->SetVisibility(true);

  for (int i=0; i<gdmlWorld->GetNoDaughters(); i++){

    // Set visibility 
    gdmlWorld->GetDaughter(i)->GetLogicalVolume()->SetVisAttributes(visAtt2);

    // Get all daugters and add to logical and sensitive vols
    logicalVols.push_back(gdmlWorld->GetDaughter(i)->GetLogicalVolume());
    sensitiveVols.push_back(gdmlWorld->GetDaughter(i)->GetLogicalVolume());
  }
  gdmlWorld->SetVisAttributes(visAtt);
  
  new G4PVPlacement(NULL,
                    G4ThreeVector(0.,0.,0.),
                    gdmlWorld,
                    gdmlWorld->GetName()+"_PhysiComp",
                    markerVol,
                    false,
                    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
}
#endif

std::vector<G4LogicalVolume*> BDSGeometryGDML::GetAllLogicalVolumes() const { 
  return logicalVols;
}
  
std::vector<G4LogicalVolume*> BDSGeometryGDML::GetAllSensitiveVolumes() const {
  return sensitiveVols;
}
