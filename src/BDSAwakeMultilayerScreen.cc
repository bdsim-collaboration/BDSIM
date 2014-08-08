#include <math.h>
#include "BDSAwakeMultilayerScreen.hh"
#include "G4TwoVector.hh"
#include "BDSLensFacet.hh"
#include "BDSMultiFacetLayer.hh"
#include "BDSGlobalConstants.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "BDSSampler.hh"

BDSAwakeMultilayerScreen::BDSAwakeMultilayerScreen(G4String material, G4double thickness, G4double dgrain, G4double windowThickness, G4String windowMaterial):
BDSMultilayerScreen(G4TwoVector(1*CLHEP::m,3*CLHEP::cm),(G4String)"AwakeMultilayerScreen"),_material(material),_thickness(thickness),  _dgrain(dgrain), _windowThickness(windowThickness),_windowMaterial(windowMaterial)
{
  _fillFactor=0.5;
  _layerThickness=_dgrain;
  _binderLayerThickness=_dgrain*(1-_fillFactor)/_fillFactor;
  _nScintLayers=_thickness/(_layerThickness+_binderLayerThickness);
  G4double modulo = (_nScintLayers-floor(_nScintLayers));
  _firstLayerThickness = _layerThickness*modulo;
  _firstBinderLayerThickness = _binderLayerThickness*modulo;
  layers();
}


BDSAwakeMultilayerScreen::~BDSAwakeMultilayerScreen(){
}

void BDSAwakeMultilayerScreen::layers(){
  _gapWidth=0*1e-3*CLHEP::mm;
  _gapSpacing=1*CLHEP::mm;
  preWindowSampler();
  windowLayer();
  preScreenSampler();
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
  postScreenSampler();
  build();
}

void BDSAwakeMultilayerScreen::sampler(G4String name){
  G4int nThisSampler = BDSSampler::GetNSamplers()+1;
  G4String samplerName = "Sampler_" + BDSGlobalConstants::Instance()->StringFromInt(nThisSampler) + "_" + name;
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),1*CLHEP::um),samplerName,"vacuum",0,0);
  sl->color(G4Color(1.0,0.0,0.0,0.3));
  sl->sampler();
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::preWindowSampler(){
  sampler((G4String)"preWindowSampler");
}

void BDSAwakeMultilayerScreen::preScreenSampler(){
  sampler((G4String)"preScreenSampler");
}

void BDSAwakeMultilayerScreen::postScreenSampler(){
  sampler((G4String)"postScreenSampler");
}

void BDSAwakeMultilayerScreen::windowLayer(){
  if(_windowThickness>0){
    BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),_windowThickness),(G4String)"windowLayer",_windowMaterial.data(),0,0);
    sl->color(G4Color(0.8,0.8,0.8,0.3));
    screenLayer(sl);
  }
}


void BDSAwakeMultilayerScreen::backLayer(){
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),10*CLHEP::um),(G4String)"backingLayer","cellulose",0,0);
  sl->color(G4Color(0.5,0.0,0.5,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::substrateLayer(){
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),178*CLHEP::um),(G4String)"substrateLayer","PET",0,0);
  sl->color(G4Color(1.0,0.0,0.0,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::binderLayer(){
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),_binderLayerThickness),(G4String)"binderLayerBack","pet_lanex",0,0);
  sl->color(G4Color(1.0,0.0,0.0,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::backBinderLayer(){
    BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),_firstBinderLayerThickness),(G4String)"binderLayerBack","pet_lanex",0,0);
  sl->color(G4Color(1.0,0.0,0.0,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::scintillatorLayer(){
  BDSScreenLayer* sl = new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),_layerThickness),(G4String)"scintillatorLayer","gos_lanex",_gapWidth,_gapSpacing);
  sl->color(G4Color(0.0,1.0,0.0,0.3));
  screenLayer(sl);
  G4cout << "finished." << G4endl;
}

void BDSAwakeMultilayerScreen::frontScintillatorLayer1(){
  BDSScreenLayer* sl = new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),0.5*_layerThickness),(G4String)"scintillatorLayer","gos_lanex",_gapWidth,_gapSpacing);
  sl->color(G4Color(0.0,1.0,0.0,0.3));
  screenLayer(sl);
  G4cout << "finished." << G4endl;
}

void BDSAwakeMultilayerScreen::frontScintillatorLayer2(){
  BDSScreenLayer* sl = new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),0.5*_layerThickness),(G4String)"scintillatorLayer","gos_ri1",_gapWidth,_gapSpacing);
  sl->color(G4Color(0.0,1.0,0.0,0.3));
  screenLayer(sl);
  G4cout << "finished." << G4endl;
}

void BDSAwakeMultilayerScreen::backScintillatorLayer(){
  BDSScreenLayer* sl = new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),_firstLayerThickness),(G4String)"scintillatorLayer","gos_lanex",_gapWidth,_gapSpacing);
  sl->color(G4Color(0.0,1.0,0.0,0.3));
  screenLayer(sl);
  G4cout << "finished." << G4endl;
}


void BDSAwakeMultilayerScreen::frontLayer(){
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),5*CLHEP::um),(G4String)"frontLayer","cellulose",0,0);
  sl->color(G4Color(0.5,0.0,0.5,0.3));
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
