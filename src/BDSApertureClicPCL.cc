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
#include "BDSApertureClicPCL.hh"
#include "BDSApertureType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSPolygon.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "G4TwoVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <vector>

BDSApertureClicPCL::BDSApertureClicPCL(G4double xIn,
                                       G4double yTopIn,
                                       G4double yBottomIn,
                                       G4double ySepIn):
  BDSAperture(BDSApertureType::clicpcl),
  x(xIn),
  yTop(yTopIn),
  yBottom(yBottomIn),
  ySep(ySepIn)
{;}

G4bool BDSApertureClicPCL::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureClicPCL* oc = dynamic_cast<const BDSApertureClicPCL*>(other);
      return BDS::DoublesAreEqual(oc->x, x) &&
	         BDS::DoublesAreEqual(oc->yTop, yTop) &&
	         BDS::DoublesAreEqual(oc->yBottom, yBottom) &&
             BDS::DoublesAreEqual(oc->ySep, ySep);
    }
}

void BDSApertureClicPCL::CheckInfoOK() const
{
  CheckRequiredParametersSet(x, true, yTop, true, yBottom, true, ySep, true);
  CheckParameterIsPositive(x, "x");
  CheckParameterIsPositive(yTop, "yTop");
  CheckParameterIsPositive(yBottom, "yBottom");
  CheckParameterIsPositive(ySep, "ySep");
}

BDSExtent BDSApertureClicPCL::Extent() const
{
  BDSExtent simpleExtent(-x, x,
                         -yBottom, ySep + yTop,
                          0, 0);
  return ExtentOffsetTilt(simpleExtent);
}

BDSApertureClicPCL BDSApertureClicPCL::operator+ (G4double number) const
{
  BDSApertureClicPCL result = BDSApertureClicPCL(*this);
  result += number;
  return result;
}

const BDSApertureClicPCL& BDSApertureClicPCL::operator+=(G4double number)
{
  x += number;
  yTop += number;
  yBottom += number;
  return *this;
}

BDSApertureClicPCL BDSApertureClicPCL::operator* (G4double number) const
{
  BDSApertureClicPCL result = BDSApertureClicPCL(*this);
  result *= number;
  return result;
}

const BDSApertureClicPCL& BDSApertureClicPCL::operator*=(G4double number)
{
  x *= number;
  yTop *= number;
  yBottom *= number;
  ySep *= number;
  return *this;
}

BDSPolygon BDSApertureClicPCL::Polygon(G4int nPointsIn) const
{
  G4int np = nPointsIn == 0 ? MinimumNumberOfPoints() : nPointsIn;
  if (np < MinimumNumberOfPoints())
    {throw BDSException(__METHOD_NAME__, "number of points for aperture specified < " + std::to_string(MinimumNumberOfPoints()) + ".");}

  std::vector<G4TwoVector> r;
  np = BDS::NextMultiple(np, 2); // ensure multiple of 2
  G4int nPoints = np / 2;
  AppendAngleEllipse(r, -CLHEP::halfpi, CLHEP::halfpi, x, yTop, nPoints, 0, ySep);
  r.emplace_back(G4TwoVector(x, ySep));
  AppendAngleEllipse(r, CLHEP::halfpi, CLHEP::halfpi + CLHEP::pi, x, yBottom, nPoints, 0, 0);
  r.emplace_back(G4TwoVector(-x, 0));

  return BDSPolygon(r).ApplyTiltOffset(tiltOffset);
}
