//An accelerator component for diagnostics screens e.g. OTR. Screen inside beam pipe.

#include "BDSScreen.hh"
#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"
#include <list>
#include <string>
#include <sstream>

BDSScreen::~BDSScreen(){
  delete _screenRot;
  delete _mlScreen;
}

BDSScreen::BDSScreen(G4String aName,  
		     G4double aLength,
		     G4bool aperset, 
		     G4double aper, //Aperture of beampipe around screen
		     G4String tunnelMaterial,
		     G4double tunnelOffsetX,
		     G4TwoVector size, //X Y dimensions of screen
		     G4double screenAngle):BDSDrift(aName,aLength, aper,aper,tunnelMaterial,aperset,aper,tunnelOffsetX),
					   _size(size), 
					   _screenAngle(screenAngle){
  _screenRot = new G4RotationMatrix();
  _screenRot->rotateY(_screenAngle);
  _screenPos.setX(0);
  _screenPos.setY(0);
  _screenPos.setZ(0);
  _mlScreen = new BDSMultilayerScreen(size, itsName+"_mlscreen");
  _nLayers=0;
}

void BDSScreen::Build(){
  BDSDrift::Build(); //Build the beam pipe geometry.
  G4VisAttributes* VisAtt1 = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4, 0.3));
  VisAtt1->SetForceWireframe(true);
  VisAtt1->SetVisibility(true);
  itsBeampipeLogicalVolume->SetVisAttributes(VisAtt1);

  PlaceScreen(); //Place the screen in the beam pipe
}

void BDSScreen::BuildFieldAndStepper(){
  G4cout << __METHOD_NAME__ << " - building bmap field and stepper." << G4endl;
  BuildBmapFieldAndStepper();
}

void BDSScreen::screenLayer(G4double thickness, G4String material, G4int isSampler){
  std::stringstream ss;
  ss << _nLayers;
  G4String lNum = ss.str();
  G4String lName = itsName+"_"+lNum;
  _mlScreen->screenLayer(thickness,material,lName, isSampler);
  if(!isSampler) AddSensitiveVolume(_mlScreen->lastLayer()->log());
  _nLayers++;
}

void BDSScreen::PlaceScreen(){
  _mlScreen->build();//Build the screen.
  _mlScreen->place(_screenRot, _screenPos, itsInnerBPLogicalVolume); //Place the screen in the beampipe centre.
}
