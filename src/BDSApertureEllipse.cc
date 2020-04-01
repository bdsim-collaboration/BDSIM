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
#include "BDSApertureEllipse.hh"
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

BDSApertureEllipse::BDSApertureEllipse(G4double aIn,
                                      G4double bIn,
                                      G4int    nPointsIn):
  BDSAperture(BDSApertureType::ellipse),
  a(aIn),
  b(bIn),
  nPoints(nPointsIn)
{
  if (nPoints == 0)
    {nPoints = BDSGlobalConstants::Instance()->NSegmentsPerCircle();}
}

G4bool BDSApertureEllipse::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureEllipse* oc = dynamic_cast<const BDSApertureEllipse*>(other);
      return BDS::DoublesAreEqual(oc->a, a) && BDS::DoublesAreEqual(oc->b, b);
    }
}

void BDSApertureEllipse::CheckInfoOK() const
{
  CheckRequiredParametersSet(a, true, b, true);
  CheckParameterIsPositive(a, "a");
  CheckParameterIsPositive(b, "b");
}

BDSExtent BDSApertureEllipse::Extent() const
{
  BDSExtent simpleExtent(a, b, 0);
  return ExtentOffsetTilt(simpleExtent);
}

BDSApertureEllipse BDSApertureEllipse::operator+ (G4double number) const
{
  BDSApertureEllipse result = BDSApertureEllipse(*this);
  result += number;
  return result;
}

const BDSApertureEllipse& BDSApertureEllipse::operator+=(G4double number)
{
  a += number;
  b += number;
  return *this;
}

BDSApertureEllipse BDSApertureEllipse::operator* (G4double number) const
{
  BDSApertureEllipse result = BDSApertureEllipse(*this);
  result *= number;
  return result;
}

const BDSApertureEllipse& BDSApertureEllipse::operator*=(G4double number)
{
  a *= number;
  b *= number;
  return *this;
}

BDSPolygon BDSApertureEllipse::Polygon(G4int nPointsIn) const
{
  G4int np = nPointsIn == 0 ? nPoints : nPointsIn;
  if (np < 3 || np < MinimumNumberOfPoints())
    {throw BDSException(__METHOD_NAME__, "number of points for aperture specified < 3.");}

  std::vector<G4TwoVector> r;
  r.reserve(np);
  G4double dTheta = CLHEP::twopi / (G4double)np;
  for (G4int i = 0; i < np; i++)
    {r.emplace_back(G4TwoVector(a * std::cos(i*dTheta), b * std::sin(i*dTheta)));}
  
  return BDSPolygon(r).ApplyTiltOffset(tiltOffset);
}
