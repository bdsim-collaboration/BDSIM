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
#include "BDSArray4DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator4DLinearMag.hh"
#include "BDSInterpolatorRoutines.hh"

#include "globals.hh"

#include <cmath>

BDSInterpolator4DLinearMag::BDSInterpolator4DLinearMag(BDSArray4DCoords* arrayIn):
  BDSInterpolator4D(arrayIn)
{;}

BDSInterpolator4DLinearMag::~BDSInterpolator4DLinearMag()
{;}

BDSFieldValue BDSInterpolator4DLinearMag::GetInterpolatedValueT(G4double x,
								G4double y,
								G4double z,
								G4double t) const
{
  G4double xarr = array->ArrayCoordsFromX(x);
  G4double yarr = array->ArrayCoordsFromY(y);
  G4double zarr = array->ArrayCoordsFromZ(z);
  G4double tarr = array->ArrayCoordsFromT(t);
  
  G4int x0 = (G4int)std::floor(xarr);
  G4int y0 = (G4int)std::floor(yarr);
  G4int z0 = (G4int)std::floor(zarr);
  G4int t0 = (G4int)std::floor(tarr);

  BDSFieldValue localData[2][2][2][2];

  for (G4int i = 0; i < 2; i++)
    {
      for (G4int j = 0; j < 2; j++)
	{
	  for (G4int k = 0; k < 2; k++)
	    {
	      for (G4int l = 0; l < 2; l++)
		{
		  localData[i][j][k][l] = array->GetConst(x0+i, y0+j, z0+k, t0+l);
		}
	    }
	}
    }

  BDSFieldValue result = BDS::Linear4DMag(localData, xarr-x0, yarr-y0, zarr-z0, tarr-t0);

  return result;
}