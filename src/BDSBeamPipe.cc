/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#include "BDSBeamPipe.hh"
#include "BDSUtilities.hh"

#include "globals.hh"         // geant4 globals / types
#include "G4VSolid.hh"

BDSBeamPipe::BDSBeamPipe(G4VSolid*        containerSolidIn,
			 G4LogicalVolume* containerLVIn,
			 BDSExtent        extentIn,
			 G4VSolid*        containerSubtractionSolidIn,
			 G4LogicalVolume* vacuumLVIn,
			 G4bool           containerIsCircularIn,
			 G4double         containerRadiusIn,
			 G4ThreeVector    inputFaceNormalIn,
			 G4ThreeVector    outputFaceNormalIn):
  BDSGeometryComponent(containerSolidIn, containerLVIn, extentIn),
  containerSubtractionSolid(containerSubtractionSolidIn),
  vacuumLogicalVolume(vacuumLVIn),
  containerIsCircular(containerIsCircularIn),
  containerRadius(containerRadiusIn),
  inputFaceNormal(inputFaceNormalIn.unit()),
  outputFaceNormal(outputFaceNormalIn.unit())
{;}

BDSBeamPipe::~BDSBeamPipe()
{
  delete containerSubtractionSolid;
}

std::set<G4LogicalVolume*> BDSBeamPipe::GetVolumesForField() const
{
  std::set<G4LogicalVolume*> result = GetAllLogicalVolumes(); // from base class
  // a magnet can defined without geometry but the field may still be desired which requires an LV.
  // only remove the containerLV if there's at least one other LV in the set that the field can be
  // attached to.
  // only if empty of daughter LVs should the set then contain the container LV.
   if (result.size() > 1)
    {result.erase(GetContainerLogicalVolume());}
   else if (!BDS::IsFinite(result.size()))
    {result = {GetContainerLogicalVolume()};}

  return result;
}
