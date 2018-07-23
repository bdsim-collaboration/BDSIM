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
#include <G4UniformMagField.hh>
#include <G4TransportationManager.hh>
#include <include/BDSFieldBuilder.hh>
#include "globals.hh" // geant4 globals / types

#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSColours.hh"
#include "BDSComponentFactory.hh"
#include "BDSDebug.hh"
#include "BDSFieldType.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorSet.hh"
#include "BDSIntegratorType.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMaterials.hh"
#include "BDSLine.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSUndulator.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"

#include "parser/element.h"
#include "parser/elementtype.h"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"

#include "G4Transform3D.hh"

using namespace GMAD;

BDSUndulator::BDSUndulator (G4String   nameIn,
                            G4double   lengthIn,
                            G4double   outerDiameterIn,
                            G4double   periodIn,
                            G4double   magnetHeightIn,
                            G4double   undulatorGapIn,
                            BDSBeamPipeInfo* beamPipeInfoIn,
                            BDSFieldInfo* vacuumFieldInfoIn,
                            G4String  materialIn ):
        BDSAcceleratorComponent(nameIn, lengthIn, 0, "undulator",beamPipeInfoIn),
        outerDiameter(outerDiameterIn),
        vacuumFieldInfo(vacuumFieldInfoIn),
        undulatorPeriod(periodIn),
        material(materialIn),
        magnetHeight(magnetHeightIn),
        magnetGap(undulatorGapIn)

{;}
BDSUndulator::~BDSUndulator()
{;}

void BDSUndulator::BuildContainerLogicalVolume()
{
  //Input Checks
  if (outerDiameter <= 0)
    {
      G4cerr << __METHOD_NAME__ << "Error: option \"outerDiameter\" is not defined or must be greater than 0" <<  G4endl;
      exit(1);
    }
  if (BDS::IsFinite(fmod(arcLength, undulatorPeriod)))
    {
      G4cerr << __METHOD_NAME__ << "Undulator length \"arcLength\" does not divide into an integer number of "
            "undulator periods (length \"undulatorPeriod\"" <<  G4endl;
      exit(1);
    }
  if (!BDS::IsFinite(undulatorPeriod))
    {
      G4cerr << __METHOD_NAME__ << "Undulator period is 0, period must be finite" <<  G4endl;
      exit(1);
    }

  G4double L = 1500;

  containerSolid = new G4Box(name + "_container_solid",10*L,10*L,chordLength*0.5);

  containerLogicalVolume = new G4LogicalVolume(containerSolid,
                                               emptyMaterial,
                                               name + "_container_lv");

}

void BDSUndulator::Build()
{

  BDSAcceleratorComponent::Build();

  G4double L = 1500;
  G4double x = 100;
  G4double y = 20;
  G4double z = 100;
  G4double q = L/z; //this is the number of blocks

  
//G4Box(const G4String& pName,
//      G4double& pX,
//      G4double& pY,
//      G4double& pZ)

  G4Box* aBox = new G4Box(name, x, y, z);

RegisterSolid(aBox);
 
G4Material *materialBox  = BDSMaterials::Instance()->GetMaterial(material);
G4LogicalVolume *aBoxLV = new G4LogicalVolume(aBox,materialBox, name+"_box_lv");
RegisterLogicalVolume(aBoxLV);

 //Rotation
    G4RotationMatrix *aBoxROT = new G4RotationMatrix;
    aBoxROT->rotateX(0);
    aBoxROT->rotateZ(0);
    aBoxROT->rotateY(0);
    RegisterRotationMatrix(aBoxROT);

    //colour
    G4VisAttributes *aBoxcolour = new G4VisAttributes(G4Colour(0.1,0.7,0.2));
aBoxLV->SetVisAttributes(aBoxcolour);
RegisterVisAttributes(aBoxcolour);

    for (int i = 1; i<2*q; i++){
      G4ThreeVector bBoxpos(0, -60, L - i*z);
      G4PVPlacement *bBoxPV = new G4PVPlacement(aBoxROT,                  // rotation
					      bBoxpos,                  // position
                                              aBoxLV,                   // its logical volume
                                                std::to_string(i) + "_wire_pv_neg",        // its name
					      containerLogicalVolume,   // its mother volume
                                              false,                    // no boolean operation
                                              0,                        // copy number
                                              checkOverlaps);
    RegisterPhysicalVolume(bBoxPV);
    
    }
    

    for (int i =1; i<2*q; i++){
      G4ThreeVector cBoxpos(0,60,L-i*z);
      G4PVPlacement *cBoxPV= new G4PVPlacement(aBoxROT, cBoxpos,aBoxLV,std::to_string(i) +  "_wire_pv_pos",containerLogicalVolume,false,0,checkOverlaps);
      RegisterPhysicalVolume(cBoxPV);


    }


    BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
    BDSBeamPipe* pipe = factory->CreateBeamPipe(name, chordLength ,beamPipeInfo);

    RegisterDaughter(pipe);

    SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());
    InheritExtents(pipe);
    SetInputFaceNormal(pipe->InputFaceNormal());
    SetOutputFaceNormal(pipe->OutputFaceNormal());
    G4PVPlacement* bpPV = new G4PVPlacement(nullptr,
                                            G4ThreeVector(),
                                            pipe->GetContainerLogicalVolume(),
                                            name+"_beampipe_pv",
                                            containerLogicalVolume,
                                            false,
                                            0,
                                            checkOverlaps);

    RegisterPhysicalVolume(bpPV);




    //BDSMagnetOuterInfo* outerInfo = PrepareMagnetOuterInfo(elementName, element, st, bpInfo);
    //vacuumField->SetScalingRadius(outerInfo->innerRadius); // purely for completeness of information - not required
    //BDSFieldInfo* outerField = nullptr;

    //G4Transform3D newFieldTransform = vacuumFieldInfo->Transform();
    //vacuumFieldInfo->SetTransform(newFieldTransform);
    BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumFieldInfo, pipe->GetContainerLogicalVolume(),true);



}

//void BDSUndulator::BuildUndulatorMagnet()
//{;}


//}
