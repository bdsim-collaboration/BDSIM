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
#include "BDSApertureCircular.hh"
#include "BDSApertureElliptical.hh"
#include "BDSApertureRectangular.hh"
#include "BDSApertureType.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSUtilities.hh"

#include "G4Types.hh"

BDSApertureElliptical::BDSApertureElliptical(G4double aIn,
					     G4double bIn,
					     G4int    nPointsIn):
  BDSAperture(BDSApertureType::elliptical),
  a(aIn),
  b(bIn),
  nPoints(nPointsIn)
{
  if (nPoints == 0)
    {nPoints = BDSGlobalConstants::Instance()->NSegmentsPerCircle();}
}

G4bool BDSApertureElliptical::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureElliptical* oc = dynamic_cast<const BDSApertureElliptical*>(other);
      return BDS::DoublesAreEqual(oc->a, a) && BDS::DoublesAreEqual(oc->b, b);
    }
}

G4bool BDSApertureElliptical::LessThan(const BDSAperture* other) const
{
  if (!other)
    {return false;}

  G4bool result = false;
  BDSApertureType otherType = other->apertureType;
  switch (otherType.underlying())
    {
    case BDSApertureType::elliptical:
      {
	const BDSApertureElliptical* oc = dynamic_cast<const BDSApertureElliptical*>(other);
	result = a < oc->a && b < oc->b;
	break;
      }
    case BDSApertureType::circular:
      {
	const BDSApertureCircular* oc = dynamic_cast<const BDSApertureCircular*>(other);
	result = a < oc->radius && b < oc->radius;
	break;
      }
    case BDSApertureType::rectangular:
      {
	const BDSApertureRectangular* oc = dynamic_cast<const BDSApertureRectangular*>(other);
	result = a < oc->a && b < oc->b;
	break;
      }
    default:
      {break;}
    }
  return result;
}

void BDSApertureElliptical::CheckInfoOK() const
{
  CheckRequiredParametersSet(a, true, b, true);
}

BDSExtent BDSApertureElliptical::Extent() const
{
  BDSExtent simpleExtent(a, b, 0);
  return ExtentOffsetTilt(simpleExtent);
}

BDSApertureElliptical BDSApertureElliptical::operator+ (const G4double number) const
{
  BDSApertureElliptical result = BDSApertureElliptical(*this);
  result.a += number;
  result.b += number;
  return result;
}

const BDSApertureElliptical& BDSApertureElliptical::operator+=(const G4double number)
{
  a += number;
  b += number;
  return *this;
}

BDSApertureElliptical BDSApertureElliptical::operator* (const G4double number) const
{
  BDSApertureElliptical result = BDSApertureElliptical(*this);
  result.a *= number;
  result.b *= number;
  return result;
}

const BDSApertureElliptical& BDSApertureElliptical::operator*=(const G4double number)
{
  a *= number;
  b *= number;
  return *this;
}


