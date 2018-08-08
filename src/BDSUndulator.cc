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
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSFieldBuilder.hh"
#include "BDSMaterials.hh"
#include "BDSUndulator.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"

#include <cmath>

BDSUndulator::BDSUndulator(G4String   nameIn,
			   G4double   lengthIn,
			   G4double   periodIn,
			   G4double   magnetHeightIn,
			   G4double   magnetWidthIn,
			   G4double   undulatorGapIn,
			   BDSBeamPipeInfo* beamPipeInfoIn,
			   BDSFieldInfo* vacuumFieldInfoIn,
			   G4String   materialIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "undulator", beamPipeInfoIn),
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
  // input Checks
  BDSExtent bp = beamPipeInfo->Extent();
  if (BDS::IsFinite(std::fmod(chordLength, undulatorPeriod)))
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
  if (!BDS::IsFinite(undulatorGap))
     {
       G4cout << __METHOD_NAME__ << "\"undulatorGap\" = 0 -> using 2x beam pipe height" << G4endl;
       undulatorGap = bp.DY() * 4;
     }
  if (0.5 * undulatorGap <= bp.DY())
    {
      G4cerr << __METHOD_NAME__ << "undulatorGap smaller then aperture " <<  G4endl;
      exit(1);
    }

  if (!BDS::IsFinite(magnetWidth))
    {
      G4cout << __METHOD_NAME__ << "\"magnetWidth\" = 0 -> using 100 mm" << G4endl;
      magnetWidth = 100;
    }
  if (!BDS::IsFinite(magnetHeight))
    {
      G4cout << __METHOD_NAME__ << "\"magnetWidth\" = 0 -> using 20 mm" << G4endl;
      magnetHeight = 20;
    }

  G4double halfWidth  = magnetWidth + lengthSafetyLarge;
  G4double halfHeight = undulatorGap*0.5 + magnetHeight + lengthSafetyLarge;
  containerSolid = new G4Box(name + "_container_solid",
			     halfWidth,
			     halfHeight,
			     chordLength*0.5);

  containerLogicalVolume = new G4LogicalVolume(containerSolid,
                                               emptyMaterial,
                                               name + "_container_lv");

  BDSExtent ext = BDSExtent(2 * halfWidth, 2 * halfHeight, chordLength);
  SetExtent(ext);
}

void BDSUndulator::Build()
{
  BDSAcceleratorComponent::Build();

  G4double numMagnets = 2*chordLength/undulatorPeriod; //number of magnets (undulator period is 2 magnets)

  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe* pipe = factory->CreateBeamPipe(name, chordLength, beamPipeInfo);
  RegisterDaughter(pipe);

  // magnet geometry
  G4Box* aBox = new G4Box(name + "_single_magnet_solid",
			  magnetWidth,
			  magnetHeight,
			  undulatorPeriod*0.25);
  RegisterSolid(aBox);
 
  G4Material* materialBox  = BDSMaterials::Instance()->GetMaterial(material);

  G4LogicalVolume* lowerBoxLV = new G4LogicalVolume(aBox,
						    materialBox,
						    name + "_lower_box_lv");
  RegisterLogicalVolume(lowerBoxLV);

  
  G4LogicalVolume* upperBoxLV = new G4LogicalVolume(aBox,
						    materialBox,
						    name + "_upper_box_lv");
  RegisterLogicalVolume(upperBoxLV);

  // colour
  G4VisAttributes* lowerBoxcolour = new G4VisAttributes(G4Colour(0.8,0.1,0.1));
  lowerBoxLV->SetVisAttributes(lowerBoxcolour);
  RegisterVisAttributes(lowerBoxcolour);

  G4VisAttributes* upperBoxcolour = new G4VisAttributes(G4Colour(0.1,0.1,0.8));
  upperBoxLV->SetVisAttributes(upperBoxcolour);
  RegisterVisAttributes(upperBoxcolour);

  // place upper and lower magnets in a loop
  // note numMagnets may not be an integer value (it's also a G4double)
  for (G4int i = 1; i <= numMagnets; i++)
    {
      G4bool sign = BDS::IsFinite(std::fmod(i, 2));
      G4LogicalVolume* uVol = sign ? upperBoxLV : lowerBoxLV;
      G4LogicalVolume* lVol = !sign ? upperBoxLV : lowerBoxLV;

      G4ThreeVector upperBoxPos(0, undulatorGap / 2.0,  (0.5*chordLength - undulatorPeriod/4.0) -  ((i-1) *undulatorPeriod/2.0));
      G4ThreeVector lowerBoxPos(0, undulatorGap / -2.0, (0.5*chordLength - undulatorPeriod/4.0) -  ((i-1) *undulatorPeriod/2.0));
      
      G4PVPlacement* upperBoxPV = new G4PVPlacement(nullptr,                  // rotation
						    upperBoxPos,              // position
						    uVol,                     // logical volume
						    name + "_upper_pos_" + std::to_string(i) +  "_pv", // name
						    containerLogicalVolume,   // mother volume
						    false,                    // no boolean operation
						    i,                        // copy number
						    checkOverlaps);
      
      G4PVPlacement* lowerBoxPV= new G4PVPlacement(nullptr,
						   lowerBoxPos,
						   lVol,
						   name + "_lower_pos_" + std::to_string(i) +  "_pv",
						   containerLogicalVolume,
						   false,
						   i,
						   checkOverlaps);
      RegisterPhysicalVolume(upperBoxPV);
      RegisterPhysicalVolume(lowerBoxPV);
    }

  // place beam pipe volume
  SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());
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
