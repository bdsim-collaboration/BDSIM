#include "BDSGlobalConstants.hh" 
#include "BDSScreenLayer.hh"
#include "BDSMaterials.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSDManager.hh"
#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

BDSScreenLayer::BDSScreenLayer()
{;}

BDSScreenLayer::BDSScreenLayer (G4ThreeVector size,
				G4String name,
				G4String material, 
				G4double grooveWidth,
				G4double grooveSpatialFrequency):
  _size(size),
  _name(name),
  copyNumber(0),
  _material(material),
  _grooveWidth(grooveWidth),
  _grooveSpatialFrequency(grooveSpatialFrequency)
{
  _nGrooves=0; //Counter for the number of grooves etched into the screen.
  _color=G4Colour(0.1,0.8,0.1,0.3);
  build();
}

void BDSScreenLayer::build(){
  buildGroove();
  buildScreen();
  visAtt();
}

void BDSScreenLayer::buildGroove(){
  if (_grooveWidth==0){ //There may or may not be grooves in the screen layer.
    _grooveSolid=nullptr;
    _grooveLog=nullptr;
    return;
  }
  _grooveSolid = new G4Box((_name+"_grooveSolid").c_str(),_grooveWidth/2.0, _size.y()/2.0, _size.z()/2.0);
  _grooveLog = new G4LogicalVolume(_grooveSolid,BDSMaterials::Instance()->GetMaterial("air"),(_name+"_grooveLog").c_str(),0,0,0);
}

void BDSScreenLayer::buildScreen(){
  _solid  = new G4Box((_name+"_solid").c_str(),_size.x()/2.0,_size.y()/2.0,_size.z()/2.0);
  _log = new G4LogicalVolume(_solid,BDSMaterials::Instance()->GetMaterial(_material),(_name+"_log").c_str(),0,0,0);
  cutGrooves();

#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  G4UserLimits* itsScoringPlaneUserLimits =  new G4UserLimits();
  itsScoringPlaneUserLimits->SetMaxAllowedStep(_size.z()*maxStepFactor);
  _log->SetUserLimits(itsScoringPlaneUserLimits);
#endif

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
  new G4PVPlacement((G4RotationMatrix*)nullptr,  //Create a new physical volume placement for each groove in the screen.
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
  G4VisAttributes* visAtt=new G4VisAttributes(_color);
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

void BDSScreenLayer::color(G4Color col){
  _color=col;
  visAtt();
}

void BDSScreenLayer::backInternalMirror(){
  _internalMirror = new InternalMirror(InternalMirror::_BACK, _size,_material,_log,_phys);
}

void BDSScreenLayer::frontInternalMirror(){
  _internalMirror = new InternalMirror(InternalMirror::_FRONT,_size,_material,_log,_phys);
}

BDSScreenLayer::InternalMirror::~InternalMirror(){
}

BDSScreenLayer::InternalMirror::InternalMirror(G4int varside, G4ThreeVector size, G4String material, G4LogicalVolume* motherLog, G4PVPlacement* motherPhys):_side(varside),_motherSize(size),_motherMaterial(material),_motherLog(motherLog),_motherPhys(motherPhys)
{
  _thickness=1e-9*CLHEP::m; 
  compute();
  geom();
  place();
  optical();
}

void BDSScreenLayer::InternalMirror::geom(){
  _solid  = new G4Box("internalMirrorSolid",_motherSize.x()/2.0,_motherSize.y()/2.0,_thickness);
  _log = new G4LogicalVolume(_solid,BDSMaterials::Instance()->GetMaterial("titanium"),"internalMirrorLog",0,0,0);
}

void BDSScreenLayer::InternalMirror::place(){
  _phys=new G4PVPlacement((G4RotationMatrix*)nullptr,  //Create a new physical volume placement for each groove in the screen.
			  G4ThreeVector(0,0,_pos),
			  _log,
			  "internalMirrorPhys",
			  _motherLog,
			  false,
			  0, // BDSScreenLayer::copyNumber, // nested class prevents data passing
			  true
			  );
}

void BDSScreenLayer::InternalMirror::optical(){
  G4OpticalSurface* OpSurface=new G4OpticalSurface("OpSurface");
  /*G4LogicalBorderSurface* LogSurface = */
  new G4LogicalBorderSurface("LogSurface", _motherPhys, _phys, OpSurface);
  //  G4LogicalSkinSurface* LogSurface  = new G4LogicalSkinSurface("LogSurface",screenLayer(1)->log(),OpSurface);
  OpSurface -> SetType(dielectric_metal);
  OpSurface -> SetModel(unified);
  OpSurface -> SetFinish(polished);
  G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();
  SMPT->AddConstProperty("REFLECTIVITY",0.8);
  //  SMPT->AddConstProperty("RINDEX",1.5750);
  OpSurface->SetMaterialPropertiesTable(SMPT);
}

void BDSScreenLayer::InternalMirror::compute(){
  G4double sign=0;
  try{
    if(_side==_BACK){
      sign = 1;
    }else if(_side==_FRONT){
      sign = -1;
    }else{
      throw 1;
    }
  }catch(int e){
    G4cerr<< "BDSScreenLayer::computInternalMirror - exception number " << e << " occurred. Exiting." << G4endl;
    exit(e);
  }
  
  _pos = sign*(_motherSize.z()/2.0-_thickness/2.0);
}

void BDSScreenLayer::sampler()
{
  G4String samplerName = _name + "_1";

  _log->SetSensitiveDetector(BDSSDManager::Instance()->GetSamplerPlaneSD());
  copyNumber = BDSSamplerRegistry::Instance()->RegisterSampler(samplerName,nullptr);

#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  G4UserLimits* itsScoringPlaneUserLimits =  new G4UserLimits();
  itsScoringPlaneUserLimits->SetMaxAllowedStep(_size.z()*maxStepFactor);
  _log->SetUserLimits(itsScoringPlaneUserLimits);
#endif
}

BDSScreenLayer::~BDSScreenLayer()
{;}
