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
#include "BDSFieldEMInterpolated1D.hh"
#include "BDSFieldModulator.hh"
#include "BDSInterpolator1D.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <utility>

BDSFieldEMInterpolated1D::BDSFieldEMInterpolated1D(BDSInterpolator1D*   eInterpolatorIn,
						   BDSInterpolator1D*   bInterpolatorIn,
						   const G4Transform3D& offset,
						   G4double             eScalingIn,
						   G4double             bScalingIn,
               BDSFieldModulator* modulatorIn):
  BDSFieldEMInterpolated(eInterpolatorIn, bInterpolatorIn, offset, eScalingIn, bScalingIn),
  eInterpolator(eInterpolatorIn),
  bInterpolator(bInterpolatorIn),
  eDimensionIndex((eInterpolatorIn->FirstDimension()).underlying()),
  eTime((eInterpolatorIn->FirstDimension()).underlying() > 2),
  bDimensionIndex((bInterpolatorIn->FirstDimension()).underlying()),
  bTime((bInterpolatorIn->FirstDimension()).underlying() > 2),
  modulator(modulatorIn)
{;}

BDSFieldEMInterpolated1D::~BDSFieldEMInterpolated1D()
{
  delete eInterpolator;
  delete bInterpolator;
  delete modulator;
}

std::pair<G4ThreeVector,G4ThreeVector> BDSFieldEMInterpolated1D::GetField(const G4ThreeVector& position,
									  const G4double       t) const
{
  G4double eCoordinate = 0;
  if (eTime)
    {eCoordinate = t;}
  else
    {eCoordinate = position[eDimensionIndex];}
  G4double bCoordinate = 0;
  if (bTime)
    {bCoordinate = t;}
  else
    {bCoordinate = position[bDimensionIndex];}
  G4double modulation = 1.0;
  if(modulator)
    {modulation = modulator->GetValue(t);}
  G4ThreeVector e = eInterpolator->GetInterpolatedValue(eCoordinate) * EScaling() * modulation;
  G4ThreeVector b = bInterpolator->GetInterpolatedValue(bCoordinate) * BScaling() * modulation;
  return std::make_pair(b,e);
}
