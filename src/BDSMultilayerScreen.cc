#include "BDSMultilayerScreen.hh"

#include "BDSColourWheel.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSMaterials.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSScreenLayer.hh"
#include "BDSDebug.hh"

#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4OpticalSurface.hh"
#include "G4PVPlacement.hh"               
#include "G4VSolid.hh"

BDSMultilayerScreen::BDSMultilayerScreen (G4TwoVector xysize, G4String name):
  xysize(xysize), name(name), colourWheel(new BDSColourWheel())
{
  size.setX(xysize.x()); 
  size.setY(xysize.y());
  size.setZ(0);
  solid=nullptr;
  log=nullptr;
  phys=nullptr;
}

void BDSMultilayerScreen::screenLayer(G4double thickness, G4String material, G4String name, G4int isSampler, G4double grooveWidth, G4double grooveSpatialFrequency)
{
  G4String layerName = name;
  if(isSampler){
    G4int nThisSampler = BDSSamplerRegistry::Instance()->NumberOfExistingSamplers() + 1;
    G4String tempString = "Sampler_" + std::to_string(nThisSampler);
    layerName = tempString + "_" + layerName;
  } else {
      layerName=name;
  }
  G4ThreeVector layerSize(xysize.x(), xysize.y(), thickness);
  screenLayer(new BDSScreenLayer(layerSize, layerName, material, grooveWidth,grooveSpatialFrequency), isSampler);
}

void BDSMultilayerScreen::screenLayer(BDSScreenLayer* layer, G4int isSampler)
{
  colourWheel->spin();
  layer->SetColour(colourWheel->colour());
  if(isSampler) layer->sampler();
  screenLayers.push_back(layer);
}

BDSScreenLayer* BDSMultilayerScreen::lastLayer(){
  return screenLayer(nLayers()-1);
}

void BDSMultilayerScreen::build()
{
  buildMotherVolume();
  placeLayers();
}

void BDSMultilayerScreen::buildMotherVolume()
{
  computeDimensions();
  solid  = new G4Box((name+"_solid").c_str(),size.x()/2.0,size.y()/2.0,size.z()/2.0);
  log = new G4LogicalVolume(solid,BDSMaterials::Instance()->GetMaterial(
          BDSGlobalConstants::Instance()->VacuumMaterial()),(name+"_log").c_str(),0,0,0);
  G4VisAttributes* visAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0,0.3));
  visAtt->SetForceWireframe(true);
  log->SetVisAttributes(visAtt);
}

void BDSMultilayerScreen::computeDimensions(){
  G4cout << "Compute dimensions..." << G4endl;
  G4cout << "...z size..." << G4endl;
  G4double temp=0;
  if(screenLayers.size()==0){
    G4Exception("Screen has no layers.", "-1", FatalException, "");
  }
  for(unsigned int i=0; i<screenLayers.size(); i++){
    G4cout << "..adding z size for layer number " << i << G4endl;
    temp += screenLayers[i]->GetSize().z();
    //Compute the total z thickness.
  }
  size.setZ(temp);
  //Compute the z positions of all the layers.
  G4cout << "...z positions..." << G4endl;
  G4double pos = screenLayers[0]->GetSize().z()/2.0 -1.0*size.z()/2.0; //Position each layer after the previous one.
  screenLayerZPos.push_back(pos);
  for(unsigned int i=1; i<screenLayers.size(); i++){
    pos += (screenLayers[i-1]->GetSize().z()+screenLayers[i]->GetSize().z())/2.0;
    screenLayerZPos.push_back(pos);
  }
  G4cout << "...finsished." << G4endl;
}

void BDSMultilayerScreen::placeLayers(){
  G4ThreeVector pos;
  pos.setX(0);
  pos.setY(0);
  pos.setZ(0);

  for(unsigned int i=0; i<screenLayers.size(); i++){
    pos.setZ(screenLayerZPos[i]);
    G4cout << __METHOD_NAME__ <<": placing screen layer with ID: " << screenLayers[i]->GetSamplerID() << G4endl;
    screenLayers[i]->SetPhys(new G4PVPlacement((G4RotationMatrix*)nullptr,  //Create a new physical volume placement for each groove in the screen.
					       pos,
					       screenLayers[i]->GetLog(),
					       (G4String)(screenLayers[i]->GetName()),
					       log,
					       false,
					       screenLayers[i]->GetSamplerID(),
					       true
					     )
			   );
  }
}


BDSScreenLayer* BDSMultilayerScreen::screenLayer(G4String layer){
  for(unsigned int i=0; i<screenLayers.size(); i++){
    if(screenLayer(i)->GetName()==layer){
      screenLayer(i);
    }
  }
  G4cerr << "BDSMultiLayer - error: screen layer \"" << layer << "\" not found. Exiting." << G4endl;
  exit(1);
}


void BDSMultilayerScreen::place(G4RotationMatrix* rot, G4ThreeVector pos, G4LogicalVolume* motherVol){
  SetPhys(new G4PVPlacement(
			    rot,
			    pos,
			    log,
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
  new G4LogicalBorderSurface("LogSurface", screenLayer(layer1)->GetPhys(), screenLayer(layer2)->GetPhys(), OpSurface);
  //  G4LogicalSkinSurface* LogSurface  = new G4LogicalSkinSurface("LogSurface",screenLayer(1)->GetLog(),OpSurface);
  OpSurface->SetType(dielectric_metal);
  OpSurface->SetModel(unified);
  OpSurface->SetFinish(polished);

  
  
  G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();
  SMPT->AddConstProperty("REFLECTIVITY",0.8);
  
  /*
  OpSurface->SetSigmaAlpha(0.7);
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
  new G4LogicalBorderSurface("LogSurface", screenLayer(layer1)->GetPhys(), screenLayer(layer2)->GetPhys(), OpSurface);

  G4double sigma_alpha=0.7;
  OpSurface->SetSigmaAlpha(sigma_alpha);
  G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();
  SMPT->AddConstProperty("REFLECTIVITY",0.0);
  OpSurface->SetMaterialPropertiesTable(SMPT);
}

BDSMultilayerScreen::~BDSMultilayerScreen()
{
  delete colourWheel;
}
