#include "BDSGlobalConstants.hh" 
#include "BDSCCDPixel.hh"
#include "BDSCCDPixelSD.hh"
#include "BDSMaterials.hh"
#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"
#include "BDSDebug.hh"

#include "G4SDManager.hh"
#include "G4UserLimits.hh"

extern BDSCCDPixelSD* BDSCCDPixelSensDet;


//============================================================
BDSCCDPixel::BDSCCDPixel (G4ThreeVector size, G4String name):
  _size(size), _name(name+"_CCDPixel")
{
  G4cout << __METHOD_NAME__ << G4endl;
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  _sensDet=new BDSCCDPixelSD(_name);
  SDMan->AddNewDetector(_sensDet);
  build();
  _log->SetSensitiveDetector(_sensDet);
  G4cout << __METHOD_END__ << G4endl;
}


void BDSCCDPixel::visAtt()
{
  _visAtt=new G4VisAttributes(G4Colour(0.8,0.2,0.0));
  _visAtt->SetForceWireframe(true);
  _log->SetVisAttributes(_visAtt);
}

void BDSCCDPixel::build(){
  G4cout << __METHOD_NAME__ << G4endl;
  _solid  = new G4Box((_name+"_solid").c_str(),_size.x()/2.0,_size.y()/2.0,_size.z()/2.0);
  _log = new G4LogicalVolume(_solid,BDSMaterials::Instance()->GetMaterial("fusedsilica"),(_name+"_log").c_str(),0,0,0);
  visAtt();
  G4cout << __METHOD_END__ << G4endl;
}

G4String BDSCCDPixel::name(){
  return _name;
}

G4LogicalVolume* BDSCCDPixel::log(){
  return _log;
}

G4ThreeVector BDSCCDPixel::size(){
  return _size;
}

BDSCCDPixel::~BDSCCDPixel(){
}
