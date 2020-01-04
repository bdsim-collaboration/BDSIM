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
#include "BDSApertureRectangular.hh"
#include "BDSApertureType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSPolygon.hh"
#include "BDSTiltOffset.hh"
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
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureRectangular* oc = dynamic_cast<const BDSApertureRectangular*>(other);
      return BDS::DoublesAreEqual(oc->a, a) && BDS::DoublesAreEqual(oc->b, b);
    }
}

void BDSApertureRectangular::CheckInfoOK() const
{
  CheckRequiredParametersSet(a, true, b, true);
  CheckParameterIsPositive(a, "a");
  CheckParameterIsPositive(b, "b");
}

BDSExtent BDSApertureRectangular::Extent() const
{
  BDSExtent simpleExtent(a, b, 0);
  return ExtentOffsetTilt(simpleExtent);
}

BDSApertureRectangular BDSApertureRectangular::operator+ (const G4double number) const
{
  BDSApertureRectangular result = BDSApertureRectangular(*this);
  result += number;
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
  result *= number;
  return result;
}

const BDSApertureRectangular& BDSApertureRectangular::operator*=(const G4double number)
{
  a *= number;
  b *= number;
  return *this;
}

BDSPolygon BDSApertureRectangular::Polygon(G4int nPointsIn) const
{
  G4int np = nPointsIn == 0 ? MinimumNumberOfPoints() : nPointsIn;
  if (np < MinimumNumberOfPoints())
    {throw BDSException(__METHOD_NAME__, "number of points for aperture specified < " + std::to_string(MinimumNumberOfPoints()) + ".");}

  std::vector<G4TwoVector> r;
  r.push_back(G4TwoVector( a,  b));
  r.push_back(G4TwoVector(-a,  b));
  r.push_back(G4TwoVector(-a, -b));
  r.push_back(G4TwoVector( a, -b));
  
  return BDSPolygon(r).ApplyTiltOffset(tiltOffset);
}
