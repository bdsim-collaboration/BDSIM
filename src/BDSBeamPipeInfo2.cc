/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#include "BDSAperture.hh"
#include "BDSBeamPipeInfo2.hh"
#include "BDSBeamPipeType.hh"
#include "BDSExtent.hh"

#include "G4Types.hh"

#include <algorithm>

BDSBeamPipeInfo2::BDSBeamPipeInfo2(BDSBeamPipeType beamPipeTypeIn,
				   BDSAperture*    apertureIn,
				   G4Material*     vacuumMaterialIn,
				   G4double        beamPipeThicknessIn,
				   G4Material*     beamPipeMaterialIn,
				   G4bool          vacuumOnlyIn,
				   BDSAperture*    apertureOutIn,
				   G4ThreeVector*  inputFaceNormalIn,
				   G4ThreeVector*  outputFaceNormalIn):
  beamPipeType(beamPipeTypeIn),
  aperture(apertureIn),
  vacuumMaterial(vacuumMaterialIn),
  beamPipeThickness(beamPipeThicknessIn),
  beamPipeMaterial(beamPipeMaterialIn),
  vacuumOnly(vacuumOnlyIn),
  apertureOut(apertureOutIn),
  inputFaceNormal(inputFaceNormalIn),
  outputFaceNormal(outputFaceNormalIn)
{;}

BDSBeamPipeInfo2::BDSBeamPipeInfo2(const BDSBeamPipeInfo2& other):
  beamPipeType(other.beamPipeType),
  vacuumMaterial(other.vacuumMaterial),
  beamPipeThickness(other.beamPipeThickness),
  beamPipeMaterial(other.beamPipeMaterial),
  vacuumOnly(other.vacuumOnly)
{
  //if (other.aperture)
  //  {aperture = other.aperture->Clone();}
}

BDSBeamPipeInfo2::~BDSBeamPipeInfo2()
{
  // permit the use of two apertures to be specified but ar degenerate
  if (apertureOut != aperture)
    {delete apertureOut;}
  delete aperture;
  delete inputFaceNormal;
  delete outputFaceNormal;
}

BDSExtent BDSBeamPipeInfo2::Extent() const
{
  BDSExtent result = aperture->Extent();
  result.ExpandBy(beamPipeThickness); // a slight underestimate as no lengthSafety
  if (apertureOut && (apertureOut != aperture))
    {
      BDSExtent apOutExt = apertureOut->Extent();
      apOutExt.ExpandBy(beamPipeThickness);
      return std::max(result, apOutExt);
    }
  else
    {return result;}
}
