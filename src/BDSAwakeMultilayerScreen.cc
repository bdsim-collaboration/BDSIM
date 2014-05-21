#include "BDSAwakeMultilayerScreen.hh"
#include "G4TwoVector.hh"
#include "BDSLensFacet.hh"
#include "BDSMultiFacetLayer.hh"
#include "BDSGlobalConstants.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

BDSAwakeMultilayerScreen::BDSAwakeMultilayerScreen(G4String material, G4double thickness):
  BDSMultilayerScreen(G4TwoVector(1*m,3*cm),(G4String)"AwakeMultilayerScreen"),_material(material),_thickness(1000*thickness)
{
  _nScintLayers=1;
  _fillFactor=0.5;
  layers();
}


BDSAwakeMultilayerScreen::~BDSAwakeMultilayerScreen(){
}

void BDSAwakeMultilayerScreen::layers(){
  _gapWidth=0*1e-3*mm;
  _gapSpacing=1*mm;
  shieldingLayer();
  backLayer();
  substrateLayer();
  for(int i=0; i<_nScintLayers; i++){
    binderLayer();
    scintillatorLayer();
  }
  binderLayer();
  frontLayer();
  build();
}

void BDSAwakeMultilayerScreen::shieldingLayer(){
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),100*um),(G4String)"shieldingLayer","G4_Al",0,0);
  sl->color(G4Color(0.8,0.8,0.8,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::backLayer(){
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),10*um),(G4String)"backingLayer","cellulose",0,0);
  sl->color(G4Color(0.5,0.0,0.5,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::substrateLayer(){
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),178*um),(G4String)"substrateLayer","PET",0,0);
  sl->color(G4Color(1.0,0.0,0.0,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::binderLayer(){
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),(1-_fillFactor)*_thickness/(_nScintLayers+1.0)),(G4String)"binderLayerBack","pet_lanex",0,0);
  sl->color(G4Color(1.0,0.0,0.0,0.3));
  screenLayer(sl);
}

void BDSAwakeMultilayerScreen::scintillatorLayer(){
  BDSScreenLayer* sl = new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),_fillFactor*_thickness/_nScintLayers),(G4String)"scintillatorLayer","lanex",_gapWidth,_gapSpacing);
  sl->color(G4Color(0.0,1.0,0.0,0.3));
  screenLayer(sl);
  G4cout << "finished." << G4endl;
}

void BDSAwakeMultilayerScreen::frontLayer(){
  BDSScreenLayer* sl =  new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),5*um),(G4String)"frontLayer","cellulose",0,0);
  sl->color(G4Color(0.5,0.0,0.5,0.3));
  screenLayer(sl);
}


/*
void BDSAwakeMultilayerScreen::frontLayer(){
  
  BDSLensFacet* facet = new BDSLensFacet("afacet",G4TwoVector(1*mm-_gapWidth,3*cm),33.0*BDSGlobalConstants::Instance()->GetPI()/180.0,
  					 0,"PET");
  screenLayer(new BDSMultiFacetLayer((G4String)"multiFacetLayer", facet, _gapWidth,1000));
}
*/

void BDSAwakeMultilayerScreen::surfaces(){
  reflectiveSurface();
  //  roughSurface();
}

void BDSAwakeMultilayerScreen::reflectiveSurface(){
  BDSMultilayerScreen::reflectiveSurface(3,2);
}

void BDSAwakeMultilayerScreen::roughSurface(){
  //A rough surface between the scintillator layer and the cellulose layer (due to the phosphor grains)
  //  BDSMultilayerScreen::roughSurface(2,3);
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
