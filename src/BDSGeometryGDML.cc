#ifdef USE_GDML
#include "BDSGlobalConstants.hh"
#include "BDSGeometryGDML.hh"
#include "G4SDManager.hh"
#include "BDSMaterials.hh"
#include "BDSSamplerSD.hh"
#include "G4SDManager.hh"
#include "BDSSamplerSD.hh"
#include "BDSOutput.hh"
#include "G4GDMLParser.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"
#include <vector>
#include <map>
#include <cstdlib>
#include "G4ClassicalRK4.hh"
#include <cstring>

using namespace std;

extern G4RotationMatrix* RotY90;
extern BDSOutput* bdsOutput;
//extern BDSGlobalConstants* BDSGlobalConstants::Instance();

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
  VisAtt->SetForceSolid(true);

  G4VisAttributes* VisAtt2 = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
  VisAtt2->SetVisibility(false);
  VisAtt2->SetForceSolid(true);

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
