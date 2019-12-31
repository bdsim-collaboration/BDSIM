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
#include "BDSApertureType.hh"
#include "BDSExtent.hh"

#include "G4Types.hh"

BDSApertureCircular::BDSApertureCircular(G4double radiusIn):
  BDSAperture(BDSApertureType::circular),
  radius(radiusIn)
{;}

G4bool BDSApertureCircular::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else
    {
      const BDSApertureCircular* oc = dynamic_cast<const BDSApertureCircular*>(other);
      return oc->radius == radius;
    }
}

G4bool BDSApertureCircular::LessThan(const BDSAperture* other) const
{
  if (!other)
    {return false;}

  G4bool result = false;
  BDSApertureType otherType = other->apertureType;
  switch (otherType.underlying())
    {
    case BDSApertureType::circular:
      {
	const BDSApertureCircular* oc = dynamic_cast<const BDSApertureCircular*>(other);
	result = radius < oc->radius;
	break;
      }
    case BDSApertureType::elliptical:
      {
	const BDSApertureElliptical* oc = dynamic_cast<const BDSApertureElliptical*>(other);
	result = radius < oc->a && radius < oc->b;
	break;
      }
    default:
      {break;}
    }
  return result;
}

void BDSApertureCircular::CheckInfoOK() const
{
  CheckRequiredParametersSet(radius, true);
}

BDSExtent BDSApertureCircular::Extent() const
{
  BDSExtent simpleExtent(radius, radius, 0);
  return ExtentOffsetTilt(simpleExtent);
}

BDSApertureCircular BDSApertureCircular::operator+ (const G4double number) const
{
  BDSApertureCircular result = BDSApertureCircular(*this);
  result.radius += number;
  return result;
}

const BDSApertureCircular& BDSApertureCircular::operator+=(const G4double number)
{
  radius += number;
  return *this;
}

BDSApertureCircular BDSApertureCircular::operator* (const G4double number) const
{
  BDSApertureCircular result = BDSApertureCircular(*this);
  result.radius *= number;
  return result;
}

const BDSApertureCircular& BDSApertureCircular::operator*=(const G4double number)
{
  radius *= number;
  return *this;
}


