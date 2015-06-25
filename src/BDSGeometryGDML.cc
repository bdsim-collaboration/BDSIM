#ifdef USE_GDML
#include "BDSGlobalConstants.hh"
#include "BDSGeometryGDML.hh"
#include "BDSMaterials.hh"
#include "G4Colour.hh"
#include "G4GDMLParser.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include <cstdlib>
#include <cstring>

BDSGeometryGDML::BDSGeometryGDML(G4String GDMLfile):itsMarkerVol(NULL){
  itsGDMLfile = GDMLfile;
}

BDSGeometryGDML::~BDSGeometryGDML(){
}

void BDSGeometryGDML::Construct(G4LogicalVolume *marker){
  itsMarkerVol = marker;
  G4GDMLParser *parser = new G4GDMLParser();
  parser->Read(itsGDMLfile);
  
  G4LogicalVolume* topvol = parser->GetWorldVolume()->GetLogicalVolume();

  G4VisAttributes* VisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
  VisAtt->SetVisibility(false);

  G4VisAttributes* VisAtt2 = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
  VisAtt2->SetVisibility(false);
  
  for (int i=0; i<topvol->GetNoDaughters(); i++){
    topvol->GetDaughter(i)->GetLogicalVolume()->SetVisAttributes(VisAtt);
  }
  
  topvol->SetVisAttributes(VisAtt2);
  
  new G4PVPlacement(NULL,
                    G4ThreeVector(0.,0.,0.),
                    topvol,
                    topvol->GetName()+"_PhysiComp",
                    itsMarkerVol,
                    false,
                    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
}
#endif
