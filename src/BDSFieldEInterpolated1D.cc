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
#include "BDSDimensionType.hh"
#include "BDSFieldEInterpolated1D.hh"
#include "BDSFieldModulator.hh"
#include "BDSInterpolator1D.hh"

#include "G4ThreeVector.hh"

BDSFieldEInterpolated1D::BDSFieldEInterpolated1D(BDSInterpolator1D*   interpolatorIn,
						 const G4Transform3D& offset,
						 G4double             eScalingIn,
             BDSFieldModulator* modulatorIn):
  BDSFieldEInterpolated(interpolatorIn, offset, eScalingIn),
  interpolator(interpolatorIn),
  dimensionIndex((interpolatorIn->FirstDimension()).underlying()),
  time((interpolatorIn->FirstDimension()).underlying() > 2),
  modulator(modulatorIn)
{;}

BDSFieldEInterpolated1D::~BDSFieldEInterpolated1D()
{
  delete interpolator;
  delete modulator;
}

G4ThreeVector BDSFieldEInterpolated1D::GetField(const G4ThreeVector& position,
						const G4double       t) const
{
  G4double coordinate = 0;
  if (time)
    {coordinate = t;}
  else
    {coordinate = position[dimensionIndex];}
  G4double modulation = 1.0;
  if(modulator)
    {modulation = modulator->GetValue(t);}
  return interpolator->GetInterpolatedValue(coordinate) * EScaling() * modulation;
}
