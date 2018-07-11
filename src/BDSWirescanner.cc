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
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSUtilities.hh"


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
			  G4double   outerDiameterIn,
			  G4double   wireDiameterIn,
              G4double   wireLengthIn,
              G4double   wirescannerOffsetIn,
              G4double   wirescannerRotxIn,
              G4double   wirescannerRotyIn,
              G4double   wirescannerRotzIn,
              BDSBeamPipeInfo*  beamPipeInfoIn,
              G4String   wireMaterialIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "wirescanner", beamPipeInfoIn),
  outerDiameter(outerDiameterIn),
  wireDiameter(wireDiameterIn),
  wireLength(wireLengthIn),
  wirescannerOffset(wirescannerOffsetIn),
  wirescannerRotx(wirescannerRotxIn),
  wirescannerRoty(wirescannerRotyIn),
  wirescannerRotz(wirescannerRotzIn),
  wireMaterial(wireMaterialIn)
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
  if (wireDiameter <= 0)
    {
        G4cerr << __METHOD_NAME__ << "Error: option \"wireDiameter\" is not defined or must be greater than 0" <<  G4endl;
        exit(1);
    }
//  if (wireLength <= 0)
//    {
//       G4cerr << __METHOD_NAME__ << "Error: option \"wireLength\" is not defined or must be greater than 0" <<  G4endl;
//        exit(1);
//    }
  if (-outerDiameter >= wirescannerOffset)
    {
       G4cerr << __METHOD_NAME__ << "Error: option \"wirescannerOffset\" is not within container boundaries" <<  G4endl;
       exit(1);
    }
  if (wirescannerOffset >= outerDiameter)
    {
       G4cerr << __METHOD_NAME__ << "Error: option \"wirescannerOffset\" is not within container boundaries" << G4endl;
       exit(1);
    }
  if (0.5*wireLength >= sqrt(pow(beamPipeInfo->aper1,2)-pow(wirescannerOffset,2)))
  {
      G4cerr << __METHOD_NAME__ << "Error: option \"wirescannerLength\" is larger than beam pipe" << G4endl;
      exit(1);
  }

    if (wireMaterial == "")
    {
        wireMaterial = "carbon";
    }
    if (wireLength == 0)
    {
        wireLength = beamPipeInfo->aper1;
    }
    G4cout << beamPipeInfo->aper1 << G4endl;
    G4cout << wireDiameter << G4endl;
    G4cout << wireLength << G4endl;
    G4cout << wirescannerOffset << G4endl;
    G4cout << wireMaterial << G4endl;

  //build container
//  containerSolid = new G4Box(name + "_container_solid",
//			     outerDiameter*0.5,
//			     outerDiameter*0.5,
//			     chordLength*0.5);

//  containerLogicalVolume = new G4LogicalVolume(containerSolid,
//  	  			       emptyMaterial,
//                           name + "_container_lv");
}


void BDSWirescanner::Build() {
    BDSAcceleratorComponent::Build();

    BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
    BDSBeamPipe* pipe = factory->CreateBeamPipe(name,
                                                chordLength,
                                                beamPipeInfo);

    RegisterDaughter(pipe);

 //make the beam pipe container, this object's container
    containerLogicalVolume = pipe->GetContainerLogicalVolume();
    containerSolid         = pipe->GetContainerSolid();

 //register vacuum volume (for biasing)
    SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());

// update extents
    InheritExtents(pipe);

// update faces
    SetInputFaceNormal(pipe->InputFaceNormal());
   SetOutputFaceNormal(pipe->OutputFaceNormal());


    G4Material *material = BDSMaterials::Instance()->GetMaterial(wireMaterial);

    // Wire Solid and logical Volume
    G4Tubs *wire = new G4Tubs(name, 0, wireDiameter*0.5, wireLength*0.5, 0, 2 * CLHEP::pi);


    RegisterSolid(wire);

    G4LogicalVolume *wireLV = new G4LogicalVolume(wire,                // solid
                                                  material,            // material
                                                  name + "_wire_lv");  // name
    RegisterLogicalVolume(wireLV);


    //Rotation
    G4RotationMatrix *wireRot = new G4RotationMatrix;
    wireRot->rotateX(wirescannerRotx + CLHEP::pi/2.0);
    wireRot->rotateZ(wirescannerRotz);
    wireRot->rotateY(wirescannerRoty);
    RegisterRotationMatrix(wireRot);

    //colour
    G4VisAttributes *wireVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("wirescanner"));
    wireLV->SetVisAttributes(wireVisAttr);
    RegisterVisAttributes(wireVisAttr);

    //position
    G4ThreeVector wirescannerpos(wirescannerOffset, 0, 0);


    //Placement
    G4PVPlacement *wirePV = new G4PVPlacement(wireRot,           // rotation
                                                   wirescannerpos,           // position
                                                   wireLV,            // its logical volume
                                                   name + "_wire_pv", // its name
                                              GetAcceleratorVacuumLogicalVolume(),
//                                                   containerLogicalVolume, // its mother  volume
                                                   false,                  // no boolean operation
                                                   0,                      // copy number
                                                   checkOverlaps);
    RegisterPhysicalVolume(wirePV);
}
