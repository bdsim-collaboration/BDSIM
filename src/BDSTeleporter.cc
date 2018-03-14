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
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSTeleporter.hh"

#include "globals.hh" //G4 global constants & types
#include "G4Box.hh" 
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"

#include <cmath>


BDSTeleporter::BDSTeleporter(const G4double length,
			     BDSFieldInfo*  vacuumFieldInfoIn):
  BDSAcceleratorComponent("teleporter", length, 0, "teleporter"),
  vacuumFieldInfo(vacuumFieldInfoIn)
{;}

void BDSTeleporter::Build()
{
  BDSAcceleratorComponent::Build(); // create container
  
  BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumFieldInfo,
							    containerLogicalVolume,
							    true);
}

void BDSTeleporter::BuildContainerLogicalVolume()
{
  G4double radius = BDSGlobalConstants::Instance()->SamplerDiameter() * 0.5;
  containerSolid = new G4Box(name+"_container_solid",
			     radius,
			     radius,
			     chordLength*0.5);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");

  // register extents with BDSGeometryComponent base class
  SetExtent(BDSExtent(radius, radius, chordLength*0.5));
}

G4ThreeVector BDS::CalculateTeleporterDelta(BDSBeamline* thebeamline)
{
  if (thebeamline->empty())
    {return G4ThreeVector();}
  // get position of last item in beamline
  // and then calculate necessary offset teleporter should apply
  G4ThreeVector lastitemposition  = thebeamline->back()->GetReferencePositionEnd();
  G4ThreeVector firstitemposition = thebeamline->front()->GetReferencePositionStart();
  G4ThreeVector delta             = lastitemposition - firstitemposition;
  
  G4cout << "Calculating Teleporter delta"   << G4endl;
  G4cout << "Last item end position:       " << lastitemposition  << " mm" << G4endl;
  G4cout << "First item start position:    " << firstitemposition << " mm" << G4endl;
  G4cout << "Teleport delta:               " << delta << " mm" << G4endl;

  return delta;
}

BDSTeleporter::~BDSTeleporter()
{
  delete vacuumFieldInfo;
}
