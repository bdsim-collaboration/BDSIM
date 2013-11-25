#include "BDSGlobalConstants.hh" 
#include "BDSScreenLayer.hh"
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

#include "G4SDManager.hh"
#include "G4UserLimits.hh"
#include "G4Version.hh"
#include "parser/gmad.h"


//============================================================
BDSScreenLayer::BDSScreenLayer(){
}

BDSScreenLayer::BDSScreenLayer (G4ThreeVector size, G4String name, G4String material, G4double grooveWidth, G4double grooveSpatialFrequency):
  _size(size), _name(name+"_ScreenLayer"),_material(material),_grooveWidth(grooveWidth),_grooveSpatialFrequency(grooveSpatialFrequency)
{
  _nGrooves=0; //Counter for the number of grooves etched into the screen.
  build();
}

void BDSScreenLayer::build(){
  buildGroove();
  buildScreen();
  visAtt();
}

void BDSScreenLayer::buildGroove(){
  if (_grooveWidth==0){ //There may or may not be grooves in the screen layer.
    _grooveSolid=NULL;
    _grooveLog=NULL;
    return;
  }
  _grooveSolid = new G4Box((_name+"_grooveSolid").c_str(),_grooveWidth/2.0, _size.y()/2.0, _size.z()/2.0);
  _grooveLog = new G4LogicalVolume(_grooveSolid,BDSMaterials::Instance()->GetMaterial("air"),(_name+"_grooveLog").c_str(),0,0,0);
}

void BDSScreenLayer::buildScreen(){
  _solid  = new G4Box((_name+"_solid").c_str(),_size.x()/2.0,_size.y()/2.0,_size.z()/2.0);
  _log = new G4LogicalVolume(_solid,BDSMaterials::Instance()->GetMaterial(_material),(_name+"_log").c_str(),0,0,0);
  cutGrooves();
}

void BDSScreenLayer::cutGrooves(){
  if(_grooveWidth>0){
    for(G4double xPosition=-_size.x()/2.0+_grooveWidth/2.0;
	xPosition<((_size.x()/2.0)-_grooveWidth/2.0);
	xPosition+=_grooveSpatialFrequency){
      cutGroove(xPosition);
    }
  }
}

void BDSScreenLayer::cutGroove(G4double xPosition){
  if (!_grooveLog) return;
  G4ThreeVector pos;
  pos.setX(xPosition);
  pos.setY(0);
  pos.setZ(0);
  new G4PVPlacement((G4RotationMatrix*)NULL,  //Create a new physical volume placement for each groove in the screen.
		    pos,
		    _grooveLog,
		    (G4String)(_name+"_groove"),
		    _log,
		    true,
		    _nGrooves,
		    false
		    );
  _nGrooves++; //Increment the counter for the number of grooves in the screen.
}

void BDSScreenLayer::visAtt()
{
  G4VisAttributes* visAtt=new G4VisAttributes(G4Colour(0.8,0.2,0.0,0.3));
  visAtt->SetForceSolid(true);
  G4VisAttributes* visAttGroove=new G4VisAttributes(G4Colour(0.0,0.0,0.0));
  visAttGroove->SetForceSolid(true);
  _log->SetVisAttributes(visAtt);
  if(_grooveLog){
    _grooveLog->SetVisAttributes(visAttGroove);
  }
}

void BDSScreenLayer::phys(G4PVPlacement* phys){
  if(phys->GetLogicalVolume() != log()){
    G4cerr << "BDSScreenLayer - error: physical volume placement does not match logical volume. Exiting." << G4endl;
    exit(1);
  }
  _phys=phys;
}

BDSScreenLayer::~BDSScreenLayer(){
}
