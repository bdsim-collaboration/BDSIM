#include "BDSWorld.hh"
#include "BDSBeamlineNavigator.hh"
#include "BDSMaterials.hh"
#include "BDSTunnelCavity.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"

/*
BDSWorld* BDSWorld::_instance = 0;

BDSWorld* BDSWorld::Instance(){
  if(_instance==0) {
    _instance = new BDSWorld();
  }
  return _instance;
}
*/

BDSWorld::BDSWorld(){
  _size = new G4ThreeVector(0.,0.,0.);
  _material = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetSoilMaterialName()),
  build();
  G4cout << "BDSWorld - placing" << G4endl;
  place();
}

BDSWorld::~BDSWorld(){
  delete _size;
  /*
  delete _solidVolume;
		  //		    gFlashRegion.back()->SetUserLimits(new G4UserLimits(var->GetLength()/10.0));
		  //		    MultipleSensVols[i]->SetUserLimits(new G4UserLimits(var->GetLength()/10.0));
b
  delete _logicalVolume;
  delete _physicalVolume;
  _instance = 0;
  */
}

void BDSWorld::build(){
  G4cout << "BDSWorld::build() - constructSolid()" << G4endl;
  constructSolid();
  G4cout << "BDSWorld::build() - constructLogical()" << G4endl;
  constructLogical();
  G4cout << "BDSWorld::build() - setVisAttributes()" << G4endl;
  setVisAttributes();
  G4cout << "BDSWorld::build() - setUseLimits()" << G4endl;
  setUserLimits();
  G4cout << "BDSWorld::build() - finished()" << G4endl;
}


void BDSWorld::constructSolid(){
  calcSize();
  G4cout << "BDSWorld::constructSolid - making solid volume..." << G4endl;
  _solidVolume = new G4Box("World", _size->x(), _size->y(), _size->z());
  _solidVolume->DumpInfo();
}

void BDSWorld::calcSize(){
  G4cout << "BDSWorld::calcSize() - " << G4endl;
  _size->setX((BDSTunnelCavity::Instance()->widthMax()->x()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness())/2.0);
  _size->setY((BDSTunnelCavity::Instance()->widthMax()->y()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness())/2.0);
  _size->setZ((BDSTunnelCavity::Instance()->widthMax()->z())/2.0);
  G4cout << "BDSWorld::calcSize() - size x, y, z = " << _size->x()/CLHEP::m << " " << _size->y()/CLHEP::m << " " << _size->z()/CLHEP::m << " m" << G4endl;
}

void BDSWorld::constructLogical(){
  G4cout << "BDSWorld::constructLogical - making logical volume..." << G4endl;
  _logicalVolume = new G4LogicalVolume(_solidVolume,
				       _material,
				       "world_logical");
  G4cout << "BDSWorld::constructLogical - finished." << G4endl;
}

void BDSWorld::setVisAttributes(){
  _visAttributes = new G4VisAttributes(true);
  _logicalVolume->SetVisAttributes(_visAttributes);
}

void BDSWorld::setUserLimits(){
#ifndef NOUSERLIMITS
  _userLimits =new G4UserLimits();
  _userLimits->SetMaxAllowedStep(10*CLHEP::m);
  _userLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  _userLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  _logicalVolume->SetUserLimits(_userLimits);
#endif
}

void BDSWorld::place(){
  G4cout << "BDSWorld::place - " << G4endl;
  G4ThreeVector zeroThreeVector = G4ThreeVector(0,0,0);
  _physicalVolume = new G4PVPlacement(0, // no rotation
				      zeroThreeVector,     // at (0,0,0)
				      _logicalVolume,      
				      "world_physical_volume",
				      NULL,                // its mother  volume
				      false,               // no boolean operation
				      0,                   // copy number
				      false
				      );
  
}

G4VPhysicalVolume* BDSWorld::physicalVolume(){
  return _physicalVolume;
}

G4LogicalVolume* BDSWorld::logicalVolume(){
  return _logicalVolume;

}

G4UserLimits* BDSWorld::userLimits(){
  return _userLimits;
}
