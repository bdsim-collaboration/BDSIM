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
#include "BDSApertureElliptical.hh"
#include "BDSApertureType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPolygon.hh"
#include "BDSUtilities.hh"

#include "G4TwoVector.hh"
#include "G4Types.hh"

#include <cmath>
#include <vector>

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

void BDSApertureElliptical::CheckInfoOK() const
{
  CheckRequiredParametersSet(a, true, b, true);
  CheckParameterIsPositive(a, "a");
  CheckParameterIsPositive(b, "b");
}

BDSExtent BDSApertureElliptical::Extent() const
{
  BDSExtent simpleExtent(a, b, 0);
  return ExtentOffsetTilt(simpleExtent);
}

BDSApertureElliptical BDSApertureElliptical::operator+ (const G4double number) const
{
  BDSApertureElliptical result = BDSApertureElliptical(*this);
  result += number;
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
  result *= number;
  return result;
}

const BDSApertureElliptical& BDSApertureElliptical::operator*=(const G4double number)
{
  a *= number;
  b *= number;
  return *this;
}

BDSPolygon BDSApertureElliptical::Polygon(G4int nPointsIn) const
{
  G4int np = nPointsIn == 0 ? nPoints : nPointsIn;
  if (np < 3 || np < MinimumNumberOfPoints())
    {throw BDSException(__METHOD_NAME__, "number of points for aperture specified < 3.");}

  std::vector<G4TwoVector> r;
  G4double dTheta = CLHEP::twopi / (G4double)np;
  for (G4int i = 0; i < np; i++)
    {r.push_back(G4TwoVector(a * std::cos(i*dTheta), b * std::sin(i*dTheta)));}
  
  return BDSPolygon(r).ApplyTiltOffset(tiltOffset);
}
