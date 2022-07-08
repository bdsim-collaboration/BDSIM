/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2022.

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

#include <regex>
#include "BDSSDFilterLogicalVolume.hh"

#include "G4Step.hh"

BDSSDFilterLogicalVolume::BDSSDFilterLogicalVolume(const G4String& name,
                                                   const std::vector<G4String>& referenceLVIn):
        G4VSDFilter(name),
        referenceLV(referenceLVIn)
{}

BDSSDFilterLogicalVolume::~BDSSDFilterLogicalVolume()
{}

G4bool BDSSDFilterLogicalVolume::Accept(const G4Step* aStep) const
{
  // get the step in the mass world
  const G4Step* realWorldStep = aStep->GetTrack()->GetStep();

  // get the material of the logical volume
  G4LogicalVolume* stepLV = realWorldStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume();
  G4String nameLV = stepLV->GetName();

  return std::any_of(referenceLV.begin(), referenceLV.end(), [&nameLV](const G4String& rlv){
        return regex_match (nameLV, std::regex(rlv) );
    });

}
