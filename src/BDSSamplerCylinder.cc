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
#include "BDSExtent.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSSDManager.hh"
#include "BDSSDSamplerCylinder.hh"

#include "G4LogicalVolume.hh"
#include "G4String.hh"
#include "G4Tubs.hh"
#include "G4Types.hh"

#include "CLHEP/Units/SystemOfUnits.h"


BDSSamplerCylinder::BDSSamplerCylinder(const G4String& nameIn,
				       G4double        length,
				       G4double        radiusIn,
                                       G4int           filterSetIDIn):
  BDSSampler(nameIn, filterSetIDIn)
{
  G4double thickness = 1e-6 * radiusIn;
  containerSolid = new G4Tubs(nameIn + "_solid",      // name
			      radiusIn,               // inner radius
			      radiusIn + thickness,   // outer radius
			      length*0.5,             // half-length
			      0,                      // start angle
			      CLHEP::twopi);          // sweep angle

  SetExtent(BDSExtent(radiusIn, radiusIn, length*0.5));
  CommonConstruction();
}

void BDSSamplerCylinder::SetSensitivity()
{
  auto sdMan = BDSSDManager::Instance();
  BDSSDSamplerCylinder* sd = filterSetID > -1 ? sdMan->SamplerCylinderWithFilter(filterSetID) : sdMan->SamplerCylinder();
  containerLogicalVolume->SetSensitiveDetector(sd);
}