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
			  G4double   outerDiameterIn ):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "degrader"),
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
			     chordLength*0.5);
    
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
}

void BDSWirescanner::Build() {
    BDSAcceleratorComponent::Build();

    G4Material *material = BDSMaterials::Instance()->GetMaterial("carbon");

    /*
     // Case for even number of wedges
     if (isEven(numberWedges)){
         for(G4int i=0; i < (numberWedges+1); i++){
             if(isEven(i)){
                 if(i==0){                     //First half wedge
                     rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                     rightWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                     rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i+0.5)*wedgeBasewidth) );
                 }
                 else if(i==numberWedges){     //Last half Wedge
                     rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i-0.5)*wedgeBasewidth) );
                     rightWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                     rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                 }
                 else{                         //RHS full wedge(s)
                     rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i-0.5)*wedgeBasewidth) );
                     rightWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                     rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i+0.5)*wedgeBasewidth) );
                 }
             }
             else if(isOdd(i)){                //LHS full wedge(s)
                 leftWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i-0.5)*wedgeBasewidth) );
                 leftWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                 leftWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i+0.5)*wedgeBasewidth) );
             }
         }
         //Vertices of base part of RHS component for connecting all RHS wedges
         rightWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength, maxzoffset) );
         rightWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength,-1.0*maxzoffset));

         //Vertices of base part of LHS component for connecting all LHS wedges
         leftWedgeSide.push_back( G4TwoVector(0, maxzoffset));
         leftWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength, maxzoffset) );
         leftWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength, -1.0*maxzoffset) );
         leftWedgeSide.push_back( G4TwoVector(0, -1.0*maxzoffset));
     }
     // Case for odd number of wedges.
     else if (isOdd(numberWedges)){
         for(G4int i=0; i < (numberWedges+1); i++){
             if(isEven(i)){
                 if(i==0){     //RHS half wedge
                     rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                     rightWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                     rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i+0.5)*wedgeBasewidth) );
                 }
                 else{         //RHS full wedge(s)
                     rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i-0.5)*wedgeBasewidth) );
                     rightWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                     rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i+0.5)*wedgeBasewidth) );
                 }
             }
             else if(isOdd(i)){
                 if(i==numberWedges){      //LHS half wedge
                     leftWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i-0.5)*wedgeBasewidth) );
                     leftWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                     leftWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                 }
                 else{                     //LHS full wedge(s)
                     leftWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i-0.5)*wedgeBasewidth) );
                     leftWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                     leftWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i+0.5)*wedgeBasewidth) );
                 }
             }
         }
          //Vertices of base part of RHS component for connecting all RHS wedges
         rightWedgeSide.push_back( G4TwoVector(0, maxzoffset));
         rightWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength, maxzoffset) );
         rightWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength,-1.0*maxzoffset));

         //Vertices of base part of LHS component for connecting all LHS wedges
         leftWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength, maxzoffset) );
         leftWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength, -1.0*maxzoffset) );
         leftWedgeSide.push_back( G4TwoVector(0, -1.0*maxzoffset));
     }
    */
    // Wire Solid and logical Volume
    G4Tubs *wire = new G4Tubs(name, 0, outerDiameter, arcLength, 0, 2 * CLHEP::pi);


    RegisterSolid(wire);

    G4LogicalVolume *wireLV = new G4LogicalVolume(wire,               // solid
                                                  material,                 // material
                                                  name + "_wire_lv");  // name
    RegisterLogicalVolume(wireLV);


    //Offsets for wedge overlap
    G4double xoffsetLeft = 0;
    G4double xoffsetRight = 0;

    //Rotation  of wedges. Left taken to be +VE x direction, right is -VE x direction.
    G4RotationMatrix *leftRot = new G4RotationMatrix;
    leftRot->rotateX(CLHEP::pi / -2.0);
    leftRot->rotateZ(CLHEP::pi);
    RegisterRotationMatrix(leftRot);

    //Wedge color
    G4VisAttributes *wireVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("degrader"));
    wireLV->SetVisAttributes(wireVisAttr);

    RegisterVisAttributes(wireVisAttr);

    //Translation of individual wedge components
    G4ThreeVector rightwedgepos(0, 0, 0);
    G4ThreeVector leftwedgepos(0, 0, 0);


    //Placement of individual wedge components
    G4PVPlacement *leftwedgePV = new G4PVPlacement(leftRot,           // rotation
                                                   leftwedgepos,           // position
                                                   wireLV,            // its logical volume
                                                   name + "_wire_pv", // its name
                                                   containerLogicalVolume, // its mother  volume
                                                   false,                  // no boolean operation
                                                   0,                      // copy number
                                                   checkOverlaps);


    RegisterPhysicalVolume(leftwedgePV);
}
