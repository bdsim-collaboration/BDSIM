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
#include "BDSUndulator.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSColours.hh"
#include "BDSMaterials.hh"

#include "BDSDebug.hh"

#include "G4Box.hh"
#include "G4ExtrudedSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include "globals.hh" // geant4 globals / types
#include <vector>

BDSUndulator::BDSUndulator (G4String   nameIn,
			  G4double   lengthIn,
			  G4double   outerDiameterIn,
			  G4int      numberMagnetsIn,
			  G4double   magnetHeightIn,
			  G4double   undulatorGapIn,
			  G4String   materialIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "undulator"),
  outerDiameter(outerDiameterIn),
  numberMagnets(numberMagnetsIn),
  magnetLength(chordLength/numberMagnetsIn),
  magnetHeight(magnetHeightIn),
  magnetGap(undulatorGapIn),
  material(materialIn)
{;}

BDSUndulator::~BDSUndulator()
{;}

void BDSUndulator::BuildContainerLogicalVolume()
{
  
  containerSolid = new G4Box(name + "_container_solid",
			     outerDiameter*0.5,
			     outerDiameter*0.5,
			     chordLength*0.5);
    
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
}

void BDSUndulator::BuildUndulatorMagnet()
{;}

void BDSUndulator::Build()
{
  BDSAcceleratorComponent::Build();
    
  G4Material* undulatorMaterial = BDSMaterials::Instance()->GetMaterial(material);

  // magnet solid and logical Volume
  G4Box* magnet = new G4Box(name, outerDiameter/2.0, outerDiameter/4.0,chordLength/10.0);
  
  RegisterSolid(magnet);
  
  G4LogicalVolume* magnetLv = new G4LogicalVolume(magnet,             // solid
                                                  undulatorMaterial,  // material
                                                  name + "_magnet");  // name
  RegisterLogicalVolume(magnetLv);
    
  //Rotation of magnets.
  G4RotationMatrix* lowerRot = new G4RotationMatrix;
  //lowerRot->rotateY(CLHEP::pi/2.0);
  RegisterRotationMatrix(lowerRot);

  G4RotationMatrix* upperRot = new G4RotationMatrix;
  //upperRot->rotateY(CLHEP::pi/2.0);
  //upperRot->rotateX(CLHEP::pi);
  RegisterRotationMatrix(upperRot);
    
  //Wedge color
  G4VisAttributes* undulatorVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("undulator"));
  magnetLv->SetVisAttributes(undulatorVisAttr);
  RegisterVisAttributes(undulatorVisAttr);

  //Offsets for wedge overlap
  G4double offsetUpper = magnetGap/2.0;
  G4double offsetLower = -1.0 * magnetGap/2.0;

  //Translation of individual wedge components
  G4ThreeVector upperMagnetpos(0, offsetUpper, 0);
  G4ThreeVector lowerMagnetPos(0, offsetLower, 0);
  
    
  //Placement of individual wedge components
  G4PVPlacement* upperPV = new G4PVPlacement(upperRot,               // rotation
                                             upperMagnetpos,         // position
                                             magnetLv,               // its logical volume
                                             name + "_upper_pv",     // its name
                                             containerLogicalVolume, // its mother  volume
                                             false,                  // no boolean operation
                                             0,                      // copy number
                                             checkOverlaps);


    
        
  G4PVPlacement* lowerPV = new G4PVPlacement(lowerRot,               // rotation
                                             lowerMagnetPos,         // position
                                             magnetLv,               // its logical volume
                                             name + "_lower_pv",     // its name
                                             containerLogicalVolume, // its mother  volume
                                             false,                  // no boolean operation
                                             0,                      // copy number
                                             checkOverlaps);
    
  RegisterPhysicalVolume(upperPV);
  RegisterPhysicalVolume(lowerPV);
}
