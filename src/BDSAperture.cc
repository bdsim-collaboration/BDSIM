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
#include "BDSApertureType.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "G4Types.hh"

BDSAperture::BDSAperture(BDSApertureType apertureTypeIn):
  apertureType(apertureTypeIn),
  tiltOffset(BDSTiltOffset())
{;}

BDSAperture::BDSAperture(BDSApertureType      apertureTypeIn,
			 const BDSTiltOffset& tiltOffsetIn):
  apertureType(apertureTypeIn),
  tiltOffset(tiltOffsetIn)
{;}

void BDSAperture::SetTiltOffset(G4double offsetXIn,
				G4double offsetYIn,
				G4double tiltIn)
{
  tiltOffset = BDSTiltOffset(offsetXIn, offsetYIn, tiltIn);
}

void BDSAperture::CheckRequiredParametersSet(G4double aper1, G4bool aper1Set,
					     G4double aper2, G4bool aper2Set,
					     G4double aper3, G4bool aper3Set,
					     G4double aper4, G4bool aper4Set) const
{
  if (aper1Set && !BDS::IsFinite(aper1))
    {throw BDSException("","\"aper1\" not set, but required to be.");}
  if (aper2Set && !BDS::IsFinite(aper2))
    {throw BDSException("","\"aper2\" not set, but required to be.");}
  if (aper3Set && !BDS::IsFinite(aper3))
    {throw BDSException("","\"aper3\" not set, but required to be.");}
  if (aper4Set && !BDS::IsFinite(aper4))
    {throw BDSException("","\"aper4\" not set, but required to be.");}
}
