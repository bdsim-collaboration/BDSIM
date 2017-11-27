/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#include "BDSFieldEMInterpolated1D.hh"
#include "BDSInterpolator1D.hh"

#include "G4ThreeVector.hh"

#include <utility>

BDSFieldEMInterpolated1D::BDSFieldEMInterpolated1D(BDSInterpolator1D* eInterpolatorIn,
						   BDSInterpolator1D* bInterpolatorIn,
						   G4Transform3D      offset,
						   G4double           eScalingIn,
						   G4double           bScalingIn):
  BDSFieldEMInterpolated(offset, eScalingIn, bScalingIn),
  eInterpolator(eInterpolatorIn),
  bInterpolator(bInterpolatorIn)
{;}

BDSFieldEMInterpolated1D::~BDSFieldEMInterpolated1D()
{
  delete eInterpolator;
  delete bInterpolator;
}

std::pair<G4ThreeVector,G4ThreeVector> BDSFieldEMInterpolated1D::GetField(const G4ThreeVector& position,
									  const G4double       /*t*/) const
{
  G4ThreeVector e = eInterpolator->GetInterpolatedValue(position[0]) * EScaling();
  G4ThreeVector b = bInterpolator->GetInterpolatedValue(position[0]) * BScaling();
  return std::make_pair(b,e);
}
