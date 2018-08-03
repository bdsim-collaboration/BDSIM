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
                            G4double   magnetWidthIn,
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
        magnetWidth(magnetWidthIn),
        undulatorGap(undulatorGapIn)

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
   if (undulatorGap == 0)
   {
       undulatorGap = beamPipeInfo->aper1*4;
   }

  if (undulatorGap/2 <= beamPipeInfo->aper1)
    {
      G4cerr << __METHOD_NAME__ << "undulatorGap smaller then aperture " <<  G4endl;
      exit(1);
    }


  if (magnetWidth == 0)
  {
    magnetWidth = 100;
  }
  if (magnetHeight == 0)
  {
    magnetHeight = 20;
  }


//    G4double L = 1500;
//    G4double x = 100;
//    G4double y = 20;

  containerSolid = new G4Box(name + "_container_solid",magnetWidth,undulatorGap/2.0 + magnetHeight,chordLength*0.5);

  containerLogicalVolume = new G4LogicalVolume(containerSolid,
                                               emptyMaterial,
                                               name + "_container_lv");

}

void BDSUndulator::Build()
{
  BDSAcceleratorComponent::Build();
  
  G4cout << "para" << G4endl;
  G4cout << undulatorGap << G4endl;
  G4cout << outerDiameter << G4endl;
  G4cout << magnetHeight << G4endl;
  G4cout << magnetWidth << G4endl;

 // undulatorGap = 200;

  G4double L = 1500;
//  G4double x = 100;
//  G4double y = 20;
  G4double numMagnets = 0.5*chordLength/undulatorPeriod; //number of magnets (undulator period is 2 magnets)

  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe* pipe = factory->CreateBeamPipe(name, chordLength ,beamPipeInfo);
  RegisterDaughter(pipe);

  // magnet geometry
  G4Box* aBox = new G4Box(name, magnetWidth, magnetHeight, undulatorPeriod/2.0);

  RegisterSolid(aBox);
 
  G4Material *materialBox  = BDSMaterials::Instance()->GetMaterial(material);

  G4LogicalVolume *lowerBoxLV = new G4LogicalVolume(aBox,materialBox, name+"_lower_box_lv");
  RegisterLogicalVolume(lowerBoxLV);


  G4LogicalVolume *upperBoxLV = new G4LogicalVolume(aBox,materialBox, name+"_upper_box_lv");
  RegisterLogicalVolume(upperBoxLV);

  //Rotation
  G4RotationMatrix *aBoxROT = new G4RotationMatrix;
  aBoxROT->rotateX(0);
  aBoxROT->rotateZ(0);
  aBoxROT->rotateY(0);
  RegisterRotationMatrix(aBoxROT);

  //colour
  G4VisAttributes *lowerBoxcolour = new G4VisAttributes(G4Colour(0.8,0.1,0.1));
  lowerBoxLV->SetVisAttributes(lowerBoxcolour);
  RegisterVisAttributes(lowerBoxcolour);

  G4VisAttributes *upperBoxcolour = new G4VisAttributes(G4Colour(0.1,0.1,0.8));
  upperBoxLV->SetVisAttributes(upperBoxcolour);
  RegisterVisAttributes(upperBoxcolour);

  // place upper and lower magnets in a loop
  for (int i = 1; i<2*numMagnets; i++)
    {

      if (BDS::IsFinite(fmod(i, 2)))
      {
        //G4cout << i << G4endl;
        // upper magnet
        G4ThreeVector bBoxpos(0, undulatorGap / 2.0, L - i * undulatorPeriod);
        G4PVPlacement *upperBoxPV = new G4PVPlacement(aBoxROT,      // rotation
                                                  bBoxpos,                  // position
                                                  upperBoxLV,                   // its logical volume
                                                  std::to_string(i) + "_upper_pv_neg",        // its name
                                                  containerLogicalVolume,   // its mother volume
                                                  false,                    // no boolean operation
                                                  0,                        // copy number
                                                  checkOverlaps);
        RegisterPhysicalVolume(upperBoxPV);

        G4ThreeVector cBoxpos(0,undulatorGap / -2.0, L - i*undulatorPeriod);
        G4PVPlacement *lowerBoxPV= new G4PVPlacement(aBoxROT,
                                                 cBoxpos,
                                                 lowerBoxLV,
                                                 std::to_string(i) +  "_lower_pv_pos",
                                                 containerLogicalVolume,
                                                 false,
                                                 0,
                                                 checkOverlaps);
        RegisterPhysicalVolume(lowerBoxPV);
      }
        else

      {
        G4ThreeVector bBoxpos(0, undulatorGap / 2.0, L - i * undulatorPeriod);
        G4PVPlacement *upperBoxPV = new G4PVPlacement(aBoxROT,      // rotation
                                                      bBoxpos,                  // position
                                                      lowerBoxLV,                   // its logical volume
                                                      std::to_string(i) + "_upper_pv_neg",        // its name
                                                      containerLogicalVolume,   // its mother volume
                                                      false,                    // no boolean operation
                                                      0,                        // copy number
                                                      checkOverlaps);
        RegisterPhysicalVolume(upperBoxPV);

        G4ThreeVector cBoxpos(0,undulatorGap / -2.0, L - i*undulatorPeriod);
        G4PVPlacement *lowerBoxPV= new G4PVPlacement(aBoxROT,
                                                     cBoxpos,
                                                     upperBoxLV,
                                                     std::to_string(i) +  "_lower_pv_pos",
                                                     containerLogicalVolume,
                                                     false,
                                                     0,
                                                     checkOverlaps);
        RegisterPhysicalVolume(lowerBoxPV);

      }

/*
      // upper magnet
      G4ThreeVector bBoxpos(0, pipe->GetExtentY().second * 2, L - i * undulatorPeriod);
      G4PVPlacement *bBoxPV = new G4PVPlacement(aBoxROT,      // rotation
                                              bBoxpos,                  // position
                                              aBoxLV,                   // its logical volume
                                              std::to_string(i) + "_upper_pv_neg",        // its name
                                              containerLogicalVolume,   // its mother volume
                                              false,                    // no boolean operation
                                              0,                        // copy number
                                              checkOverlaps);
      RegisterPhysicalVolume(bBoxPV);

      //lower magnet
      G4ThreeVector cBoxpos(0,pipe->GetExtentY().first*2, L - i*undulatorPeriod);
      G4PVPlacement *cBoxPV= new G4PVPlacement(aBoxROT,
                                    cBoxpos,
                                    aBoxLV,
                                    std::to_string(i) +  "_lower_pv_pos",
                                    containerLogicalVolume,
                                    false,
                                    0,
                                    checkOverlaps);
      RegisterPhysicalVolume(cBoxPV);
      */
    }

  // place beam pipe volume
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

  BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumFieldInfo,
                                                            pipe->GetContainerLogicalVolume(),
                                                            true);
}

//void BDSUndulator::BuildUndulatorMagnet()
//{;}


//}
