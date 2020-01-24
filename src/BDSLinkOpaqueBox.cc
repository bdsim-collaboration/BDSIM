/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "BDSLinkOpaqueBox.hh"

LinkOpaqueBox::LinkOpaqueBox(BDSAcceleratorComponent* componentIn,
			     G4int indexIn):
  BDSGeometryComponent(nullptr, nullptr),
  component(componentIn),
  index(indexIn)
{
  // build container geometry.
  // One box minus smaller box to make room for collimator.

  // Make a container of vacuum

  // place component in box.
  // place sampler / output plane in box with index.

  // Update containerSolid and containerLV of BDSGeometryComponent.
  // Update extents of BDSGeometryComponent.
  // register daughters BDSGeometryComponent.


}

BDSExtent LinkOpaqueBox::GetExtent()
{
  return component->GetExtent();
}
