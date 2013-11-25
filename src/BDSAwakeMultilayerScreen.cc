#include "BDSAwakeMultilayerScreen.hh"
#include "G4TwoVector.hh"
#include "BDSLensFacet.hh"
#include "BDSMultiFacetLayer.hh"
#include "BDSGlobalConstants.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

BDSAwakeMultilayerScreen::BDSAwakeMultilayerScreen():
  BDSMultilayerScreen(G4TwoVector(1*m,3*cm),(G4String)"AwakeMultilayerScreen")
{
  layers();
}


BDSAwakeMultilayerScreen::~BDSAwakeMultilayerScreen(){
}

void BDSAwakeMultilayerScreen::layers(){
  _gapWidth=1*um;
  _gapSpacing=100*mm;
  backingLayer();
  scintillatorLayer();
  frontLayer();
}

void BDSAwakeMultilayerScreen::backingLayer(){
  screenLayer(new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),300*um),(G4String)"backingLayer","PET",0,0));
}

void BDSAwakeMultilayerScreen::scintillatorLayer(){
  screenLayer(new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),300*um),(G4String)"scintillatorLayer","lanexScintLayerMaterial",_gapWidth,_gapSpacing));
}

void BDSAwakeMultilayerScreen::frontLayer(){
  
  BDSLensFacet* facet = new BDSLensFacet("afacet",G4TwoVector(1*mm-_gapWidth,3*cm),33.0*BDSGlobalConstants::Instance()->GetPI()/180.0,
					 0,"PET");
  screenLayer(new BDSMultiFacetLayer((G4String)"multiFacetLayer", facet, _gapWidth,10));
}

void BDSAwakeMultilayerScreen::surfaces(){

}

void BDSAwakeMultilayerScreen::reflectiveSurface(){
  G4OpticalSurface* OpSurface=new G4OpticalSurface("OpSurface");
  G4LogicalBorderSurface* Surface = new 
    G4LogicalBorderSurface("phosphor_PET_surface", screenLayer(1)->phys(), screenLayer(0)->phys(), OpSurface);
  G4double sigma_alpha=1.0;
  OpSurface -> SetType(dielectric_metal);
  OpSurface -> SetModel(unified);
  OpSurface -> SetFinish(ground);
  OpSurface -> SetSigmaAlpha(sigma_alpha);
  G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();
  SMPT->AddConstProperty("REFLECTIVITY",0.8);
  SMPT->AddConstProperty("RINDEX",1.5750);
  OpSurface->SetMaterialPropertiesTable(SMPT);
}

