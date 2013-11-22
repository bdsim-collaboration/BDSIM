#include "BDSAwakeMultilayerScreen.hh"
#include "G4TwoVector.hh"
#include "BDSLensFacet.hh"
#include "BDSMultiFacetLayer.hh"
#include "BDSGlobalConstants.hh"

BDSAwakeMultilayerScreen::BDSAwakeMultilayerScreen():
  BDSMultilayerScreen(G4TwoVector(1*m,3*cm),(G4String)"AwakeMultilayerScreen")
{
  addLayers();
}


BDSAwakeMultilayerScreen::~BDSAwakeMultilayerScreen(){
}

void BDSAwakeMultilayerScreen::addLayers(){
  _gapWidth=1*um;
  _gapSpacing=1*mm;
  addBackingLayer();
  addScintillatorLayer();
  addFrontLayer();
}


void BDSAwakeMultilayerScreen::addBackingLayer(){
  addScreenLayer(new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),300*um),(G4String)"backingLayer",(G4String)"PET",0,0));
}

void BDSAwakeMultilayerScreen::addScintillatorLayer(){
  addScreenLayer(new BDSScreenLayer(G4ThreeVector(size().x(),size().y(),300*um),(G4String)"scintillatorLayer","lanex",_gapWidth,_gapSpacing));
}

void BDSAwakeMultilayerScreen::addFrontLayer(){
  
  BDSLensFacet* facet = new BDSLensFacet("afacet",G4TwoVector(1*mm-_gapWidth,3*cm),33.0*BDSGlobalConstants::Instance()->GetPI()/180.0,
					 0,(G4String)"PET");
  addScreenLayer(new BDSMultiFacetLayer((G4String)"multiFacetLayer", facet, _gapWidth,1000));
}
