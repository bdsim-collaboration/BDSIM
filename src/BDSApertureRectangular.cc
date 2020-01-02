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
#include "BDSUtilities.hh"

#include "G4Types.hh"

BDSApertureRectangular::BDSApertureRectangular(G4double aIn,
					       G4double bIn):
  BDSAperture(BDSApertureType::rectangular),
  a(aIn),
  b(bIn)
{;}

G4bool BDSApertureRectangular::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else
    {
      const BDSApertureRectangular* oc = dynamic_cast<const BDSApertureRectangular*>(other);
      return BDS::DoublesAreEqual(oc->a, a) && BDS::DoublesAreEqual(oc->b, b);
    }
}

G4bool BDSApertureRectangular::LessThan(const BDSAperture* other) const
{
  if (!other)
    {return false;}

  G4bool result = false;
  BDSApertureType otherType = other->apertureType;
  switch (otherType.underlying())
    {
    case BDSApertureType::rectangular:
      {
	const BDSApertureRectangular* oc = dynamic_cast<const BDSApertureRectangular*>(other);
	result = a < oc->a && b < oc->b;
	break;
      }
    case BDSApertureType::circular:
      {
	const BDSApertureCircular* oc = dynamic_cast<const BDSApertureCircular*>(other);
	result = a < oc->radius && b < oc->radius;
	break;
      }
    case BDSApertureType::elliptical:
      {
	const BDSApertureElliptical* oc = dynamic_cast<const BDSApertureElliptical*>(other);
	result = a < oc->a && b < oc->b;
	break;
      }
    default:
      {break;}
    }
  return result;
}

void BDSApertureRectangular::CheckInfoOK() const
{
  CheckRequiredParametersSet(a, true, b, true);
}

BDSExtent BDSApertureRectangular::Extent() const
{
  BDSExtent simpleExtent(a, b, 0);
  return ExtentOffsetTilt(simpleExtent);
}

BDSApertureRectangular BDSApertureRectangular::operator+ (const G4double number) const
{
  BDSApertureRectangular result = BDSApertureRectangular(*this);
  result.a += number;
  result.b += number;
  return result;
}

const BDSApertureRectangular& BDSApertureRectangular::operator+=(const G4double number)
{
  a += number;
  b += number;
  return *this;
}

BDSApertureRectangular BDSApertureRectangular::operator* (const G4double number) const
{
  BDSApertureRectangular result = BDSApertureRectangular(*this);
  result.a *= number;
  result.b *= number;
  return result;
}

const BDSApertureRectangular& BDSApertureRectangular::operator*=(const G4double number)
{
  a *= number;
  b *= number;
  return *this;
}


