#include "BDSGlobalConstants.hh" 
#include "BDSMultilayerScreen.hh"
#include "BDSMaterials.hh"
#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "BDSMultilayerScreen.hh"

BDSMultilayerScreen::BDSMultilayerScreen (G4TwoVector xysize, G4String name):
  _xysize(xysize), _name(name+"_MultilayerScreen")
{
  _size.setX(_xysize.x()); 
  _size.setY(_xysize.y());
  _size.setZ(0);
  _solid=nullptr;
  _log=nullptr;
  _phys=nullptr;
}

void BDSMultilayerScreen::screenLayer(G4double thickness, G4String material, G4String name, G4double grooveWidth, G4double grooveSpatialFrequency){
  G4ThreeVector layerSize(_xysize.x(), _xysize.y(), thickness);
  screenLayer(new BDSScreenLayer(layerSize,_name+"_"+name ,material,grooveWidth,grooveSpatialFrequency));
}

void BDSMultilayerScreen::screenLayer(BDSScreenLayer* layer){
  _screenLayers.push_back(layer);
}

G4LogicalVolume* BDSMultilayerScreen::log(){
  return _log;
}

void BDSMultilayerScreen::build(){
  buildMotherVolume();
  placeLayers();
}

void BDSMultilayerScreen::buildMotherVolume(){
  computeDimensions();
  _solid  = new G4Box((_name+"_solid").c_str(),_size.x()/2.0,_size.y()/2.0,_size.z()/2.0);
  _log = new G4LogicalVolume(_solid,BDSMaterials::Instance()->GetMaterial(
          BDSGlobalConstants::Instance()->VacuumMaterial()),(_name+"_log").c_str(),0,0,0);
  G4VisAttributes* visAtt = new G4VisAttributes(G4Color(0.0,0.0,1.0,0.3));
  visAtt->SetForceWireframe(true);
  _log->SetVisAttributes(visAtt);
}

void BDSMultilayerScreen::computeDimensions(){
  G4cout << "Compute dimensions..." << G4endl;
  G4cout << "...z size..." << G4endl;
  G4double temp=0;
  for(unsigned int i=0; i<_screenLayers.size(); i++){
    G4cout << "..adding z size for layer number " << i << G4endl;
    temp += _screenLayers[i]->size().z();
    //Compute the total z thickness.
  }
  _size.setZ(temp);
  //Compute the z positions of all the layers.
  G4cout << "...z positions..." << G4endl;
  G4double pos = _screenLayers[0]->size().z()/2.0 -1.0*_size.z()/2.0; //Position each layer after the previous one.
  _screenLayerZPos.push_back(pos);
  for(unsigned int i=1; i<_screenLayers.size(); i++){
    pos += (_screenLayers[i-1]->size().z()+_screenLayers[i]->size().z())/2.0;
    _screenLayerZPos.push_back(pos);
  }
  G4cout << "...finsished." << G4endl;
}

void BDSMultilayerScreen::placeLayers(){
  G4ThreeVector pos;
  pos.setX(0);
  pos.setY(0);
  pos.setZ(0);

  for(unsigned int i=0; i<_screenLayers.size(); i++){
    pos.setZ(_screenLayerZPos[i]);
    _screenLayers[i]->phys(new G4PVPlacement((G4RotationMatrix*)nullptr,  //Create a new physical volume placement for each groove in the screen.
					     pos,
					     _screenLayers[i]->log(),
					     (G4String)(_screenLayers[i]->name()),
					     _log,
					     false,
					     0,
					     true
					     )
			   );
  }
}


BDSScreenLayer* BDSMultilayerScreen::screenLayer(G4String layer){
  for(unsigned int i=0; i<_screenLayers.size(); i++){
    if(screenLayer(i)->name()==layer){
      screenLayer(i);
    }
  }
  G4cerr << "BDSMultiLayer - error: screen layer \"" << layer << "\" not found. Exiting." << G4endl;
  exit(1);
}


void BDSMultilayerScreen::place(G4RotationMatrix* rot, G4ThreeVector pos, G4LogicalVolume* motherVol){
  phys(new G4PVPlacement(
			 rot,
			 pos,
			 _log,
			 "multilayerScreen",
			 motherVol,
			 false,
			 0,
			 true
			 ));                 
}

void BDSMultilayerScreen::reflectiveSurface(G4int layer1, G4int layer2){
  G4OpticalSurface* OpSurface=new G4OpticalSurface("OpSurface");
  //  G4LogicalBorderSurface* LogSurface =
  new G4LogicalBorderSurface("LogSurface", screenLayer(layer1)->phys(), screenLayer(layer2)->phys(), OpSurface);
  //  G4LogicalSkinSurface* LogSurface  = new G4LogicalSkinSurface("LogSurface",screenLayer(1)->log(),OpSurface);
  OpSurface -> SetType(dielectric_metal);
  OpSurface -> SetModel(unified);
  OpSurface -> SetFinish(polished);

  
  
  G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();
  SMPT->AddConstProperty("REFLECTIVITY",0.8);
  
  /*
  OpSurface -> SetSigmaAlpha(0.7);
  SMPT->AddConstProperty("SPECULARLOBECONSTANT",1.0);
  SMPT->AddConstProperty("SPECULARSPIKECONSTANT",0.0);
  SMPT->AddConstProperty("BACKSCATTERCONSTANT",0.0);
  SMPT->AddConstProperty("EFFICIENCY",0.0);
  //  SMPT->AddConstProperty("RINDEX",1.5750);
  */

  OpSurface->SetMaterialPropertiesTable(SMPT);
}


void BDSMultilayerScreen::roughSurface(G4int layer1, G4int layer2){
  G4OpticalSurface* OpSurface=new G4OpticalSurface("OpSurface");
  OpSurface->SetType(dielectric_dielectric);
  OpSurface->SetFinish(ground);
  OpSurface->SetModel(unified);

  //  G4LogicalBorderSurface* LogSurface = 
  new G4LogicalBorderSurface("LogSurface", screenLayer(layer1)->phys(), screenLayer(layer2)->phys(), OpSurface);

  G4double sigma_alpha=0.7;
  OpSurface -> SetSigmaAlpha(sigma_alpha);
  G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();
  SMPT->AddConstProperty("REFLECTIVITY",0.0);
  OpSurface->SetMaterialPropertiesTable(SMPT);
}

BDSMultilayerScreen::~BDSMultilayerScreen(){
}
