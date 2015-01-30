#include "BDSTunnelCavitySection.hh"
#include "BDSMaterials.hh"

BDSTunnelCavitySection::BDSTunnelCavitySection(){
  G4Exception("Error - BDSTunnelCavitySection must be initialized with an accelerator component as argument!", "-1", FatalErrorInArgument, "");
}

BDSTunnelCavitySection::BDSTunnelCavitySection(BDSAcceleratorComponent* var){
  _acceleratorComponent=var;
  _name=_acceleratorComponent->GetName()+"_tunnel_cavity";
  _radius=_acceleratorComponent->GetTunnelRadius();
  _length=_acceleratorComponent->GetChordLength();
  _angle=_acceleratorComponent->GetAngle();
  G4double posX(_acceleratorComponent->GetTunnelOffsetX());
  G4double posY(BDSGlobalConstants::Instance()->GetTunnelOffsetY());
  _positionOffset=new G4ThreeVector(posX, posY, 0);
  _material=BDSMaterials::Instance()->GetMaterial(_acceleratorComponent->GetTunnelCavityMaterial());
  _nullRotationMatrix=new G4RotationMatrix(0,0,0);
}

BDSTunnelCavitySection::~BDSTunnelCavitySection(){
  delete _positionOffset;
  delete _logicalVolume;
  delete _solid;  
  delete _physi;
  delete _nullRotationMatrix;
}

//Setting functions
void BDSTunnelCavitySection::rotation(G4RotationMatrix* var){ _rotation = var; }
void BDSTunnelCavitySection::logicalVolume(G4LogicalVolume* var){ _logicalVolume = var; }
void BDSTunnelCavitySection::solid(G4VSolid* var){ _solid = var; }  
void BDSTunnelCavitySection::physi(G4VPhysicalVolume* var){ _physi = var; }

//Getting functions
G4ThreeVector* BDSTunnelCavitySection::positionOffset(){return _positionOffset;}
G4RotationMatrix* BDSTunnelCavitySection::rotation(){return _rotation;}
G4double BDSTunnelCavitySection::length(){return _length;}
G4double BDSTunnelCavitySection::angle(){return _angle;}
G4double BDSTunnelCavitySection::radius(){return _radius;}
G4String BDSTunnelCavitySection::name(){return _name;}
G4Material* BDSTunnelCavitySection::material(){return _material;}
G4LogicalVolume* BDSTunnelCavitySection::logicalVolume() {return _logicalVolume;}
G4VSolid* BDSTunnelCavitySection::solid() {return _solid;}
BDSAcceleratorComponent* BDSTunnelCavitySection::acceleratorComponent(){ return _acceleratorComponent; }


