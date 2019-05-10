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
#include "BDSBLM.hh"
#include "BDSBLMFactory.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"

#include "globals.hh"

BDSBLMFactory::BDSBLMFactory()
{;}

BDSBLMFactory::~BDSBLMFactory()
{;}

BDSBLM* BDSBLMFactory::BuildBLM(G4String name,
				G4String geometryFile,
				G4String geometryType,
				G4String material,
				G4double blm1,
				G4double blm2,
				G4double blm3,
				G4double blm4)
{
  // if geometry file is specified then we load the external file.
  BDSBLM* result = nullptr;
  if (!geometryFile.empty())
    {
      BDSGeometryExternal* blmGeom = BDSGeometryFactory::Instance()->BuildGeometry(name,
										   geometryFile);

      result = new BDSBLM(blmGeom);
    }
  else
    {


    }

  // sensitivity
  // register with output

  return result;
}
