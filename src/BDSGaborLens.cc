/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"
#include "BDSGaborLens.hh"
#include "BDSExtent.hh"
#include "BDSException.hh"
#include "BDSMaterials.hh"
#include "BDSSDType.hh"

#include "G4Box.hh"
#include "G4ExtrudedSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4TwoVector.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include "globals.hh" // geant4 globals / types
#include <string>
#include <vector>

#include "CLHEP/Units/SystemOfUnits.h"

BDSGaborLens::BDSGaborLens(const G4String& nameIn,
                           G4double lengthIn,
                           G4double horizontalWidthIn,
                           G4Colour* colourIn,
                           BDSBeamPipeInfo* beamPipeInfoIn,
                           BDSFieldInfo* vacuumFieldInfoIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "gaborlens", beamPipeInfoIn),
  horizontalWidth(horizontalWidthIn),
  colour(colourIn),
  vacuumFieldInfo(vacuumFieldInfoIn)
{
  if (vacuumFieldInfo)
    {vacuumFieldInfo->SetBeamPipeRadius(beamPipeInfoIn->IndicativeRadius());}
}

BDSGaborLens::~BDSGaborLens()
{;}

void BDSGaborLens::BuildContainerLogicalVolume()
{
  // Input Checks
  if (horizontalWidth <= 0)
    {throw BDSException(__METHOD_NAME__,"option \"horizontalWidth\" is not defined or must be greater than 0 for element \"" + name + "\"");}

  containerSolid = new G4Tubs(name + "_container_solid",
                              0, horizontalWidth + lengthSafety,
                              chordLength*0.5,
                              0, CLHEP::twopi);

  containerLogicalVolume = new G4LogicalVolume(containerSolid,
                                               beamPipeInfo->vacuumMaterial,
                                               name + "_container_lv");
}

void BDSGaborLens::Build()
{
  BDSAcceleratorComponent::Build();
  BuildBeamPipe();
  BuildOuter();
}

void BDSGaborLens::BuildOuter()
{
  BDSExtent extent = beamPipeInfo->Extent();
  G4double bpMaxExtent = extent.MaximumAbsTransverse();

  G4Tubs* outer = new G4Tubs(name + "_outer_solid",  // name
                             bpMaxExtent + lengthSafety,          // inner radius
                             horizontalWidth*0.5 - lengthSafety,  // outer radius
                             chordLength*0.5,                     // half length
                             0, CLHEP::twopi);                    // start and finish angle
  RegisterSolid(outer);
  G4Material* outerMaterial = BDSMaterials::Instance()->GetMaterial("stainlesssteel");
  G4LogicalVolume* outerLV = new G4LogicalVolume(outer,                // solid
                                                 outerMaterial,        // material
                                                 name + "_outer_lv");  // name
  G4Colour* outercolour = BDSColours::Instance()->GetColour("gaborlens");
  G4VisAttributes* outerVisAttr = new G4VisAttributes(*outercolour);
  outerVisAttr->SetVisibility(true);
  outerLV->SetVisAttributes(outerVisAttr);

  RegisterLogicalVolume(outerLV);

  // placement
  G4PVPlacement* outerPV = new G4PVPlacement(nullptr,             // rotation
                                             G4ThreeVector(0,0,0),// position
                                             outerLV,             // its logical volume
                                             name + "_outer_pv",  // its name
                                             containerLogicalVolume,
                                             false,               // no boolean operation
                                             0,                   // copy number
                                             checkOverlaps);
  RegisterPhysicalVolume(outerPV);
}

void BDSGaborLens::BuildBeamPipe()
{
  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe* pipe = factory->CreateBeamPipe(name + "_beampipe",
                                              chordLength,
                                              beamPipeInfo);
  RegisterDaughter(pipe);

  G4PVPlacement* bpPV = new G4PVPlacement(nullptr,
                                          G4ThreeVector(),
                                          pipe->GetContainerLogicalVolume(),
                                          name+"_beampipe_pv",
                                          containerLogicalVolume,
                                          false,
                                          0,
                                          checkOverlaps);

  RegisterPhysicalVolume(bpPV);

  // register vacuum volume (for biasing)
  SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());

  // update extents
  InheritExtents(pipe);

  BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumFieldInfo,
                                                            pipe->GetVacuumLogicalVolume(),
                                                            true);
}