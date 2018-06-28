/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSScreen.hh"

#include "BDSMultilayerScreen.hh"
#include "BDSScreenLayer.hh"

#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <list>
#include <sstream>

BDSScreen::BDSScreen(G4String         nameIn,  
		     G4double         chordLengthIn,
             G4double         screenAnglexIn,
             G4double         screenAngleyIn,
             G4double         screenAnglezIn,
		     G4double         screenPosxIn,
		     G4double         screenPosyIn,
		     G4double         screenPoszIn,
		     BDSBeamPipeInfo* beamPipeInfoIn,
		     G4TwoVector      sizeIn): //X Y dimensions of screen

  BDSDrift(nameIn,chordLengthIn,beamPipeInfoIn),
  size(sizeIn), 
  screenAnglex(screenAnglexIn),
  screenAngley(screenAngleyIn),
  screenAnglez(screenAnglezIn),
  screenPosx(screenPosxIn),
  screenPosy(screenPosyIn),
  screenPosz(screenPoszIn),
  screenPos(G4ThreeVector(screenPosx, screenPosy, screenPosz)),
  nLayers(0)
{
  mlScreen = new BDSMultilayerScreen(size, nameIn+"_mlscreen");
  
  screenRot = new G4RotationMatrix();
  screenRot->rotateX(screenAnglex);
  screenRot->rotateY(screenAngley);
  screenRot->rotateZ(screenAnglez);

}

BDSScreen::~BDSScreen()
{
  delete screenRot;
  delete mlScreen;
}

void BDSScreen::Build()
{
  BDSDrift::Build(); //Build the beam pipe geometry.
  
  // Make the beam pipe wireframe
  G4VisAttributes* VisAtt1 = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4, 0.3));
  VisAtt1->SetForceWireframe(true);
  VisAtt1->SetVisibility(true);
  containerLogicalVolume->SetVisAttributes(VisAtt1);

  PlaceScreen(); //Place the screen in the beam pipe
}

void BDSScreen::AddScreenLayer(G4double thickness, G4String material, G4int isSampler)
{
  std::stringstream ss;
  ss << nLayers;
  G4String lNum = ss.str();
  G4String lName = name+"_"+lNum;
  mlScreen->AddScreenLayer(thickness,material,lName, isSampler);
  if(!isSampler)
    {RegisterSensitiveVolume(mlScreen->LastLayer()->GetLog());}
  nLayers++;
}

void BDSScreen::PlaceScreen()
{
  mlScreen->Build();//Build the screen.
  G4LogicalVolume* vacuumLV = GetAcceleratorVacuumLogicalVolume();
  mlScreen->Place(screenRot, screenPos, vacuumLV); //Place the screen in the beampipe centre.
}
