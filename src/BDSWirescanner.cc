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
#include "BDSWirescanner.hh"

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
#include "G4Tubs.hh"

#include "globals.hh" // geant4 globals / types
#include <vector>

BDSWirescanner::BDSWirescanner(G4String   nameIn,
			  G4double   lengthIn,
			  G4double   outerDiameterIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "wirescanner"),
  outerDiameter(outerDiameterIn)
{;}

BDSWirescanner::~BDSWirescanner()
{;}

void BDSWirescanner::BuildContainerLogicalVolume()
{
  //Input Checks
  if (outerDiameter <= 0)
    {
      G4cerr << __METHOD_NAME__ << "Error: option \"outerDiameter\" is not defined or must be greater than 0" <<  G4endl;
      exit(1);
    }

  
  containerSolid = new G4Box(name + "_container_solid",
			     outerDiameter*0.5,
			     outerDiameter*0.5,
			     arcLength*0.5);
    
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
}

int wireradius = 10;

void BDSWirescanner::Build() {
    BDSAcceleratorComponent::Build();

    G4Material *material = BDSMaterials::Instance()->GetMaterial("carbon");

    // Wire Solid and logical Volume
    G4Tubs *wire = new G4Tubs(name, 0, wireradius, arcLength*0.5, 0, 2 * CLHEP::pi);


    RegisterSolid(wire);

    G4LogicalVolume *wireLV = new G4LogicalVolume(wire,                // solid
                                                  material,            // material
                                                  name + "_wire_lv");  // name
    RegisterLogicalVolume(wireLV);


    //Offsets for wedge overlap
    G4double xoffsetLeft = 0;
    G4double xoffsetRight = 0;

    //Rotation
    G4RotationMatrix *wireRot = new G4RotationMatrix;
    wireRot->rotateX(CLHEP::pi/2.0);
    wireRot->rotateZ(CLHEP::pi);
    wireRot->rotateY(0);
    RegisterRotationMatrix(wireRot);

    //colour
    G4VisAttributes *wireVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("wirescanner"));
    wireLV->SetVisAttributes(wireVisAttr);

    RegisterVisAttributes(wireVisAttr);

    //position
    G4ThreeVector wirescannerpos(0, 0, 0);


    //Placement
    G4PVPlacement *wirePV = new G4PVPlacement(wireRot,           // rotation
                                                   wirescannerpos,           // position
                                                   wireLV,            // its logical volume
                                                   name + "_wire_pv", // its name
                                                   containerLogicalVolume, // its mother  volume
                                                   false,                  // no boolean operation
                                                   0,                      // copy number
                                                   checkOverlaps);


    RegisterPhysicalVolume(wirePV);
}
