#include "BDSGlobalConstants.hh" 
#include "BDSLens.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4SubtractionSolid.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "BDSDebug.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4Orb.hh"

#include "G4SDManager.hh"
#include "G4UserLimits.hh"
#include "G4Version.hh"
#include "parser/gmad.h"
#include "BDSDebug.hh"

//============================================================
BDSLens::BDSLens (G4String name, G4double diameter, G4double rho, G4double centreThickness):
_name(name+"_Lens"), _diameter(diameter), _rho(rho), _centreThickness(centreThickness)
{
  build();
}


void BDSLens::visAtt()
{
  _visAtt=new G4VisAttributes(G4Colour(0.8,0.8,0.8));
  _visAtt->SetForceWireframe(true);
  _log->SetVisAttributes(_visAtt);
}

void BDSLens::build(){
  G4Tubs* tube = new G4Tubs(_name+"_tempTube", _diameter, _rho,_rho,0,twopi*radian);
  G4SubtractionSolid* sideA = new G4SubtractionSolid(_name+"_sideA",new G4Orb(_name+"_tempOrbA",_rho),tube);
  G4SubtractionSolid* sideB = new G4SubtractionSolid(_name+"_sideB",new G4Orb(_name+"_tempOrbB",_rho),tube);
  G4ThreeVector transVec;
  transVec.setZ(2*_rho-_centreThickness);
  _solid = new G4IntersectionSolid(_name+"_solid", sideA, sideB, (G4RotationMatrix*)NULL,transVec);
  _log = new G4LogicalVolume(_solid,BDSMaterials::Instance()->GetMaterial("yag"),(_name+"_log").c_str(),0,0,0);
  visAtt();
}

G4String BDSLens::name(){
  return _name;
}

G4LogicalVolume* BDSLens::log(){
  return _log;
}

BDSLens::~BDSLens(){
}
