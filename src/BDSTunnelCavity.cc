#include "BDSTunnelCavity.hh"
#include "BDSBeamline.hh"
#include "BDSMaterials.hh"
#include "G4UnionSolid.hh"


BDSTunnelCavity* BDSTunnelCavity::_instance = 0;

BDSTunnelCavity* BDSTunnelCavity::Instance(){
  if(_instance==0){
    _instance = new BDSTunnelCavity();
  }
  return _instance;
}

BDSTunnelCavity::BDSTunnelCavity(){
  _first=true;
  _straightSectionBuilder = new BDSStraightTunnelCavitySectionBuilder();
  _angleSectionBuilder = new BDSAngleTunnelCavitySectionBuilder();
  _widthMax = new G4ThreeVector(0,0,0);
  build();
}

BDSTunnelCavity::~BDSTunnelCavity(){
  delete _straightSectionBuilder;
  delete _angleSectionBuilder;
  delete _widthMax;
  delete _solidVolume;
  delete _logicalVolume;
  delete _physicalVolume;
  _instance = 0;
}

void BDSTunnelCavity::build(){
  inspectBeamline();
  for(BDSBeamline::Instance()->first();!BDSBeamline::Instance()->isDone();BDSBeamline::Instance()->next()){
    _acceleratorComponent = BDSBeamline::Instance()->currentItem();
    G4cout << "BDSTunnelCavity::build() - making tunnel cavity for " << _acceleratorComponent->GetName() << G4endl;
    
    constructSection();
    calcWidthMax();
    appendSection();    
  }
  G4cout << "BDSTunnelCavity::build() - constructing logical" << G4endl;
  constructLogical();
  G4cout << "BDSTunnelCavity::build() - finished" << G4endl;
}


void BDSTunnelCavity::constructSection(){
  //Select the appropriate builder
  if (_acceleratorComponent->GetAngle() != 0){
    _sectionGenerator.tunnelCavitySectionBuilder(_angleSectionBuilder);
  } else {
    _sectionGenerator.tunnelCavitySectionBuilder(_straightSectionBuilder);
  }
  //Build the tunnel section
  _sectionGenerator.constructTunnelCavitySection(_acceleratorComponent);
}

void BDSTunnelCavity::calcWidthMax(){
  _widthMax -> setX(2*(_acceleratorComponent->GetTunnelRadius() + std::abs(_acceleratorComponent->GetTunnelOffsetX() + BDSBeamline::Instance()->positionEnd()->x())));
  _widthMax -> setY(2*(_acceleratorComponent->GetTunnelRadius() + std::abs(BDSGlobalConstants::Instance()->GetTunnelOffsetY() + BDSBeamline::Instance()->positionEnd()->y())));
  _widthMax -> setZ(BDSBeamline::Instance()->positionEnd()->z());
}

G4ThreeVector* BDSTunnelCavity::widthMax(){
  return _widthMax;
}

void BDSTunnelCavity::appendSection(){ 
  G4cout << "BDSTunnelCavity::appendSection()" << G4endl;
  if(_first){
    _first=false;
    G4cout << "BDSTunnelCavity::appendSection() - making new solid volume" << G4endl;
    _solidVolume = _sectionGenerator.tunnelCavitySection()->solid();
  } else {
    G4cout << "BDSTunnelCavity::appendSection() - appending to solid volume" << G4endl;
    const G4String tunnelCavityName = "tunnelCavitySol";
    G4VSolid* solidVolumeCopy = _solidVolume->Clone();
    (*_solidVolume) = 
      G4UnionSolid(tunnelCavityName,
		   solidVolumeCopy,
		   _sectionGenerator.tunnelCavitySection()->solid(),
		   BDSBeamline::Instance()->rotation(),
		   *BDSBeamline::Instance()->positionFromCurrentCenter());
    G4cout << "BDSTunnelCavity::appendSection() - deleting solid volume copy" << G4endl;
    delete solidVolumeCopy;
  }
  G4cout << "BDSTunnelCavity::appendSection() - finished" << G4endl;
}

  void BDSTunnelCavity::constructLogical(){
    G4cout << "BDSTunnelCavity::constructLogical() - making logical volume" << G4endl;
  const G4String logicalVolumeName = "tunnelCavityLog";
  _logicalVolume = new G4LogicalVolume(_solidVolume,
				       //				       BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetTunnelCavityMaterialName()),
				       BDSMaterials::Instance()->GetMaterial("air"),
				       logicalVolumeName);
  G4cout << "BDSTunnelCavity::constructLogical() - made logical volume" << G4endl;
}

void BDSTunnelCavity::inspectBeamline(){
  if (BDSBeamline::Instance()->size() == 0){
    G4Exception("BDSTunnelCavity: beam line is empty!", "-1", FatalException, "");
  }
}
