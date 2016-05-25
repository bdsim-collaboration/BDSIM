#include "BDSAwakeMultilayerScreen.hh"

#include <cmath>
#include <string>
#include <sstream>

#include "G4TwoVector.hh"
#include "G4ThreeVector.hh"

#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSampler.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSScreenLayer.hh"

BDSAwakeMultilayerScreen::BDSAwakeMultilayerScreen(G4String material, G4double thickness, G4double windowScreenGap, G4double dgrain, G4double windowThickness, G4String windowMaterial, G4double width):
  BDSMultilayerScreen(G4TwoVector(width,8*CLHEP::cm),(G4String)"AwakeMultilayerScreen"),_material(material),_thickness(thickness), _windowScreenGap(windowScreenGap), _dgrain(dgrain), _windowThickness(windowThickness),_windowMaterial(windowMaterial)
{
  _ss.str("");
  _binderLayerCount=0;
  _scintLayerCount=0;
  _fillFactor=0.5;
  _layerThickness=_dgrain;
  _binderLayerThickness=_dgrain*(1-_fillFactor)/_fillFactor;
  _nScintLayers=_thickness/(_layerThickness+_binderLayerThickness);
  G4double modulo = (_nScintLayers-floor(_nScintLayers));
  _firstLayerThickness = _layerThickness*modulo;
  _firstBinderLayerThickness = _binderLayerThickness*modulo;
  layers();
}

BDSAwakeMultilayerScreen::~BDSAwakeMultilayerScreen()
{;}

void BDSAwakeMultilayerScreen::layers()
{
  _gapWidth=0*1e-3*CLHEP::mm;
  _gapSpacing=1*CLHEP::mm;
  thinVacuumLayer();
  preWindowSampler();
  thinVacuumLayer();
  windowLayer();
  thinAirLayer();
  postWindowSampler();
  thinAirLayer();
  windowScreenGap();
  thinAirLayer();
  preScreenSampler();
  thinAirLayer();
  backLayer();
  substrateLayer();
  if(_firstLayerThickness>1e-9){
    backBinderLayer();
    backScintillatorLayer();
  }
  for(int i=0; i<(floor(_nScintLayers)-1); i++){
    binderLayer();
    scintillatorLayer();
  }
  binderLayer();
  frontScintillatorLayer1();
  frontScintillatorLayer2();
  frontLayer();
  thinAirLayer();
  postScreenSampler();
  thinAirLayer();
  G4cout << __METHOD_NAME__ << " - scint layers: " << _scintLayerCount << G4endl;
  G4cout << __METHOD_NAME__ << " - binder layers: " << _binderLayerCount << G4endl;
  build();
}

void BDSAwakeMultilayerScreen::sampler(G4String name, const char* material, G4bool bSampler)
{
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(GetSize().x(),GetSize().y(),1*CLHEP::um),
					   name,
                                           material,
					   0,
					   0);
  sl->SetColour(G4Colour(1.0,0.0,0.0,0.3));
  if(bSampler) sl->sampler();
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::preWindowSampler(){
  sampler((G4String)"preWindowSampler");
}

void BDSAwakeMultilayerScreen::postWindowSampler(){
  sampler((G4String)"postWindowSampler");
}

void BDSAwakeMultilayerScreen::preScreenSampler(){
  sampler((G4String)"preScreenSampler");
}

void BDSAwakeMultilayerScreen::postScreenSampler(){
  sampler((G4String)"postScreenSampler");
}

void BDSAwakeMultilayerScreen::thinAirLayer(){
  sampler((G4String)"thinAirLayer","air",false);
}

void BDSAwakeMultilayerScreen::thinVacuumLayer(){
  sampler((G4String)"thinVacuumLayer","vacuum",false);
}

void BDSAwakeMultilayerScreen::windowLayer(){
  if(_windowThickness>0){
    BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(GetSize().x(),GetSize().y(),_windowThickness),(G4String)"windowLayer",_windowMaterial.data(),0,0);
    sl->SetColour(G4Colour(0.8,0.8,0.8,0.3));
    screenLayer(sl);
  }
}


void BDSAwakeMultilayerScreen::windowScreenGap(){
  if(_windowScreenGap>0){
    BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(GetSize().x(),GetSize().y(),_windowScreenGap),(G4String)"windowScreenGap","air",0,0);
    sl->SetColour(G4Colour(0,0,0,0.3));
    screenLayer(sl);
  }
}


void BDSAwakeMultilayerScreen::backLayer(){
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(GetSize().x(),GetSize().y(),10*CLHEP::um),(G4String)"backingLayer","cellulose",0,0);
  sl->SetColour(G4Colour(0.5,0.0,0.5,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::substrateLayer(){
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(GetSize().x(),GetSize().y(),178*CLHEP::um),(G4String)"substrateLayer","PET",0,0);
  sl->SetColour(G4Colour(1.0,0.0,0.0,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::binderLayer(){
  incBinderLayer();
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(GetSize().x(),GetSize().y(),_binderLayerThickness),_binderLayerName,"pet_lanex",0,0);
  sl->SetColour(G4Colour(1.0,0.0,0.0,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::incBinderLayer(){
  _binderLayerCount++;
  _ss.str("");
  _ss << "binderLayer_" << _binderLayerCount; 
  _binderLayerName = _ss.str();
}
void BDSAwakeMultilayerScreen::incScintLayer(){
  _scintLayerCount++;
  _ss.str("");
  _ss << "scintLayer_" << _scintLayerCount; 
  _scintLayerName = _ss.str();
}

void BDSAwakeMultilayerScreen::backBinderLayer(){
  incBinderLayer();
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(GetSize().x(),GetSize().y(),_firstBinderLayerThickness),_binderLayerName,"pet_lanex",0,0);
  sl->SetColour(G4Colour(1.0,0.0,0.0,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::scintillatorLayer(){
  incScintLayer();
  BDSScreenLayer* sl = new BDSScreenLayer(G4ThreeVector(GetSize().x(),GetSize().y(),_layerThickness),_scintLayerName,"gos_lanex",_gapWidth,_gapSpacing);
  sl->SetColour(G4Colour(0.0,1.0,0.0,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::frontScintillatorLayer1(){
  incScintLayer();
  BDSScreenLayer* sl = new BDSScreenLayer(G4ThreeVector(GetSize().x(),GetSize().y(),0.5*_layerThickness),_scintLayerName,"gos_lanex",_gapWidth,_gapSpacing);
  sl->SetColour(G4Colour(0.0,1.0,0.0,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::frontScintillatorLayer2(){
  incScintLayer();
  BDSScreenLayer* sl = new BDSScreenLayer(G4ThreeVector(GetSize().x(),GetSize().y(),0.5*_layerThickness),_scintLayerName,"gos_ri1",_gapWidth,_gapSpacing);
  sl->SetColour(G4Colour(0.0,1.0,0.0,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::backScintillatorLayer(){
  incScintLayer();
  BDSScreenLayer* sl = new BDSScreenLayer(G4ThreeVector(GetSize().x(),GetSize().y(),_firstLayerThickness),_scintLayerName,"gos_lanex",_gapWidth,_gapSpacing);
  sl->SetColour(G4Colour(0.0,1.0,0.0,0.3));
  screenLayer(sl);
}


void BDSAwakeMultilayerScreen::frontLayer(){
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(GetSize().x(),GetSize().y(),5*CLHEP::um),(G4String)"frontLayer","cellulose",0,0);
  sl->SetColour(G4Colour(0.5,0.0,0.5,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::surfaces(){
  reflectiveSurface();
  //  roughSurface();
}

void BDSAwakeMultilayerScreen::reflectiveSurface(){
  BDSMultilayerScreen::reflectiveSurface(3,2);
}

void BDSAwakeMultilayerScreen::roughSurface(){
  //A rough surface between the scintillator layer and the cellulose layer (due to the phosphor grains)
  BDSMultilayerScreen::roughSurface(23,24);
}




  /*
  G4OpticalSurface* OpSurface=new G4OpticalSurface("OpSurface");
  G4LogicalBorderSurface* Surface = new 
    G4LogicalBorderSurface("phosphor_PET_surface", screenLayer(0)->phys(), screenLayer(1)->phys(), OpSurface);
  //  G4LogicalSkinSurface* Surface  = new G4LogicalSkinSurface("PET_skin_surface",screenLayer(0)->log(),OpSurface);
  //  G4double sigma_alpha=0.7;
  OpSurface -> SetType(dielectric_metal);
  OpSurface -> SetModel(glisur);
  OpSurface -> SetFinish(polished);
  //  G4double polish = 0.8;

  //  OpSurface -> SetSigmaAlpha(sigma_alpha);
  const G4int NUM = 2;
  G4double pp[NUM] = {2.038*eV, 4.144*eV};
  //  G4double rindex[NUM] = {1.5750, 1.5750};
  G4double reflectivity[NUM] = {1.0, 1.0};
  G4double efficiency[NUM] = {0.5, 0.5};
  //  G4double specularlobe[NUM] = {0.3, 0.3};
  //  G4double specularspike[NUM] = {0.2, 0.2};
  //  G4double backscatter[NUM] = {0.1, 0.1};
  G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();
  SMPT->AddProperty("REFLECTIVITY",pp, reflectivity,NUM);
  SMPT->AddProperty("EFFICIENCY",pp,efficiency,NUM);

  //  SMPT->AddProperty("RINDEX",pp,rindex,NUM);

  OpSurface->SetMaterialPropertiesTable(SMPT);

  */


void BDSAwakeMultilayerScreen::place(G4RotationMatrix* rot, G4ThreeVector pos, G4LogicalVolume* motherVol)
{
  super::place(rot,pos,motherVol);
  surfaces();
}
