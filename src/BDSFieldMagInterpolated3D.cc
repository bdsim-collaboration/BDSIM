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
#include "BDSFieldMagInterpolated3D.hh"
#include "BDSInterpolator3D.hh"

#include "G4ThreeVector.hh"

BDSFieldMagInterpolated3D::BDSFieldMagInterpolated3D(BDSInterpolator3D* interpolatorIn,
						     G4Transform3D      offset,
						     G4double           scalingIn):
  BDSFieldMagInterpolated(offset, scalingIn),
  interpolator(interpolatorIn)
{;}

BDSFieldMagInterpolated3D::~BDSFieldMagInterpolated3D()
{
  delete interpolator;
}

G4ThreeVector BDSFieldMagInterpolated3D::GetField(const G4ThreeVector& position,
						  const G4double       /*t*/) const
{return interpolator->GetInterpolatedValue(position[0], position[1], position[2]) * Scaling();}
