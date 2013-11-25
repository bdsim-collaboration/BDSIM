#include "BDSGlobalConstants.hh" 
#include "BDSMultilayerScreen.hh"
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
#include "BDSMultilayerScreen.hh"

//============================================================
BDSMultilayerScreen::BDSMultilayerScreen (G4TwoVector xysize, G4String name):
  _xysize(xysize), _name(name+"_MultilayerScreen")
{
  _size.setX(_xysize.x()); 
  _size.setY(_xysize.y());
  _size.setZ(0);
  _solid=NULL;
  _log=NULL;
}

void BDSMultilayerScreen::screenLayer(G4double thickness, G4String material, G4String name, G4double grooveWidth, G4double grooveSpatialFrequency){
  G4ThreeVector layerSize(_xysize.x(), _xysize.y(), thickness);
  screenLayer(new BDSScreenLayer(layerSize,_name+"_"+name ,material,grooveWidth,grooveSpatialFrequency));
}

void BDSMultilayerScreen::screenLayer(BDSScreenLayer* layer){
  _screenLayers.push_back(layer);
}

G4LogicalVolume* BDSMultilayerScreen::log(){
  build();
  return _log;
}

void BDSMultilayerScreen::build(){
  buildMotherVolume();
  placeLayers();
}

void BDSMultilayerScreen::buildMotherVolume(){
  delete _solid; //Delete these if they already exist.
  delete _log;
  computeDimensions();
  _solid  = new G4Box((_name+"_solid").c_str(),_size.x()/2.0,_size.y()/2.0,_size.z()/2.0);
  _log = new G4LogicalVolume(_solid,BDSMaterials::Instance()->GetMaterial("vacuum"),(_name+"_log").c_str(),0,0,0);
}

void BDSMultilayerScreen::computeDimensions(){
  G4double temp=0;
  for(int i=0; i<_screenLayers.size(); i++){
    temp += _screenLayers[i]->size().z();
    //Compute the total z thickness.
  }
  _size.setZ(temp);
  //Compute the z positions of all the layers.
  G4double pos;
  for(int i=0; i<_screenLayers.size(); i++){
    pos = -_size.z()/2.0 + _screenLayers[i]->size().z()/2.0; //Position each layer after the previous one.
    for(int j=0; j<i; j++){
      pos += _screenLayers[i]->size().z();
    }
    _screenLayerZPos.push_back(pos);
  }
}

void BDSMultilayerScreen::placeLayers(){
  G4ThreeVector pos;
  pos.setX(0);
  pos.setY(0);
  pos.setZ(0);

  for(int i=0; i<_screenLayers.size(); i++){
    pos.setZ(_screenLayerZPos[i]);
    _screenLayers[i]->phys(new G4PVPlacement((G4RotationMatrix*)NULL,  //Create a new physical volume placement for each groove in the screen.
					     pos,
					     _screenLayers[i]->log(),
					     (G4String)(_name+"_"+_screenLayers[i]->name()),
					     _log,
					     false,
					     0,
					     true
					     )
			   );
  }
}


BDSScreenLayer* BDSMultilayerScreen::screenLayer(G4String layer){
  for(int i=0; i<_screenLayers.size(); i++){
    if(_screenLayers[i]->name()==layer){
      return _screenLayers[i];
    }
  }
  G4cerr << "BDSMultiLayer - error: screen layer \"" << layer << "\" not found. Exiting." << G4endl;
  exit(1);
}


BDSMultilayerScreen::~BDSMultilayerScreen(){
}
