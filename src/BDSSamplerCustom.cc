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
#include "BDSAperture.hh"
#include "BDSApertureFactory.hh"
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSException.hh"
#include "BDSSamplerCustom.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSDSampler.hh"
#include "BDSSDManager.hh"

#include "G4LogicalVolume.hh"
#include "G4String.hh"
#include "G4Tubs.hh"
#include "G4Types.hh"


BDSSamplerCustom::BDSSamplerCustom(const G4String& nameIn,
				   BDSAperture* shape):
  BDSSampler(nameIn)
{
  // Make general samplers thicker to be more tolerant on possible
  // problems with overlapping faces - unlike normal samplers were
  // BDSIM strictly controls the layout.
  BDSApertureFactory fac;
  containerSolid = fac.CreateSolid(name + "_aperture",
				   10*BDSSamplerPlane::chordLength,
				   shape);
  if (!containerSolid)
    {throw BDSException(__METHOD_NAME__, "invalid shape for sampler.");}

  BDSExtent ae = shape->Extent();
  G4double  dz = BDSSamplerPlane::chordLength * 0.5;
  SetExtent(BDSExtent(ae.XNeg(), ae.XPos(), ae.YNeg(), ae.YPos(), -dz, dz));

  CommonConstruction();

  containerLogicalVolume->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());
}
