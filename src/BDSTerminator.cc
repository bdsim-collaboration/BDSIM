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
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSDManager.hh"
#include "BDSTerminator.hh"
#include "BDSTerminatorSD.hh"
#include "BDSTerminatorUserLimits.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"

#include <limits>

BDSTerminator::BDSTerminator():
  BDSAcceleratorComponent("terminator", BDSSamplerPlane::ChordLength(), 0, "terminator")
{;}

BDSTerminator::~BDSTerminator()
{;}

void BDSTerminator::Build()
{
  BuildContainerLogicalVolume();
}

void BDSTerminator::BuildContainerLogicalVolume()
{
  // Bascially a copy of BDSSampler but with different sensitive detector added
  G4double radius = BDSGlobalConstants::Instance()->SamplerDiameter() * 0.5;
  containerSolid = new G4Box(name + "_container_solid",
			     radius,
			     radius,
			     chordLength * 0.5);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
  
  // Make the terminator sensitive to count the turns of the primary particle
  containerLogicalVolume->SetSensitiveDetector(BDSSDManager::Instance()->GetTerminatorSD());
  
  // Dynamic user limits - the logic of killing particles on last turn.
  // The numerical values are the default G4UserLimit values so everything will
  // normally be tracked. BDSTerminatorUserLimits has the logic inside it to
  // respond to turn number.
  BDSTerminatorUserLimits* userLimits = new BDSTerminatorUserLimits(std::numeric_limits<double>::max(),
								    std::numeric_limits<double>::max(),
								    std::numeric_limits<double>::max(),
								    0.,0.);
  RegisterUserLimits(userLimits);
  containerLogicalVolume->SetUserLimits(userLimits);

  // visual attributes
  containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());
      
  // register extents with BDSGeometryComponent base class
  SetExtent(BDSExtent(radius, radius, chordLength*0.5));
}

