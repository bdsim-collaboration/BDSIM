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
#include "BDSApertureCircle.hh"
#include "BDSApertureRectangle.hh"
#include "BDSApertureRectCircle.hh"
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

#include <cmath>

BDSApertureRectCircle::BDSApertureRectCircle(G4double aIn,
					     G4double bIn,
					     G4double radiusIn):
  BDSAperture(BDSApertureType::rectcircle),
  a(aIn),
  b(bIn),
  radius(radiusIn)
{;}

G4bool BDSApertureRectCircle::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureRectCircle* oc = dynamic_cast<const BDSApertureRectCircle*>(other);
      return BDS::DoublesAreEqual(oc->a, a) &&
	BDS::DoublesAreEqual(oc->b, b) &&
	BDS::DoublesAreEqual(oc->radius, radius);
    }
}

void BDSApertureRectCircle::CheckInfoOK() const
{
  CheckRequiredParametersSet(a, true, b, true, radius, true);
  CheckParameterIsPositive(a, "a");
  CheckParameterIsPositive(b, "b");
  CheckParameterIsPositive(radius, "radius");
}

BDSExtent BDSApertureRectCircle::Extent() const
{
  
  BDSExtent simpleExtent(std::min(a, radius), std::min(b, radius), 0);
  return ExtentOffsetTilt(simpleExtent);
}

BDSApertureRectCircle BDSApertureRectCircle::operator+ (const G4double number) const
{
  BDSApertureRectCircle result = BDSApertureRectCircle(*this);
  result += number;
  return result;
}

const BDSApertureRectCircle& BDSApertureRectCircle::operator+=(const G4double number)
{
  a += number;
  b += number;
  radius += number;
  return *this;
}

BDSApertureRectCircle BDSApertureRectCircle::operator* (const G4double number) const
{
  BDSApertureRectCircle result = BDSApertureRectCircle(*this);
  result *= number;
  return result;
}

const BDSApertureRectCircle& BDSApertureRectCircle::operator*=(const G4double number)
{
  a *= number;
  b *= number;
  radius *= number;
  return *this;
}

BDSPolygon BDSApertureRectCircle::Polygon(G4int nPointsIn) const
{
  G4int np = nPointsIn == 0 ? MinimumNumberOfPoints() : nPointsIn;
  if (np < MinimumNumberOfPoints())
    {throw BDSException(__METHOD_NAME__, "number of points for aperture specified < " + std::to_string(MinimumNumberOfPoints()) + ".");}

  BDSApertureCircle    ac = BDSApertureCircle(radius, np);
  BDSApertureRectangle ar = BDSApertureRectangle(a, b);
  BDSPolygon pCircle    = ac.Polygon();
  BDSPolygon pRectangle = ar.Polygon();
  BDSPolygon result = pCircle.Intersection(pRectangle);
  return result;

  /*
  std::vector<G4TwoVector> r;

  if (a < radius) // vertical
    {
      // calculate angle between x axis and x,y point where rectangle of width
      // a intercepts circle of radius r where a < r
      G4double thetaToIntersect = std::acos(a/radius);
      G4int    npTopHalf        = (G4int)std::nearbyint(np / 2);
      G4int    npBottomHalf     = np - npTopHalf;
      G4double dThetaTop    = 2*thetaToIntersect / (G4double)npTopHalf;
      G4double dThetaBottom = 2*thetaToIntersect / (G4double)npBottomHalf;
      for (G4int i = 0; i < npTopHalf; i++)
	{
	  r.push_back(G4TwoVector(radius * std::cos(thetaToIntersect + i*dThetaTop),
				  radius * std::sin(thetaToIntersect + i*dThetaTop)));
	}
      for (G4int i = 0; i < npBottomHalf; i++)
	{
	  r.push_back(G4TwoVector(radius * std::cos(thetaToIntersect + CLHEP::pi + i*dThetaBottom),
				  radius * std::sin(thetaToIntersect + CLHEP::pi + i*dThetaBottom)));
	}
    }
  else // horizontal
    {
      // calculate angle between x axis and x,y point where rectangle of width
      // a intercepts circle of radius r where b < r
      G4double thetaToIntersect = std::asin(b/radius);
      G4int    npRightHalf = (G4int)std::nearbyint(np / 2);
      G4int    npLeftHalf  = np - npRightHalf;
      G4double dThetaRight = 2*thetaToIntersect / (G4double)npRightHalf;
      G4double dThetaLeft  = 2*thetaToIntersect / (G4double)npLeftHalf;
      for (G4int i = 0; i < npRightHalf; i++)
	{
	  r.push_back(G4TwoVector(radius * std::cos(-thetaToIntersect + i*dThetaRight),
				  radius * std::sin(-thetaToIntersect + i*dThetaRight)));
	}
      for (G4int i = 0; i < npLeftHalf; i++)
	{
	  r.push_back(G4TwoVector(radius * std::cos(-thetaToIntersect + CLHEP::pi + i*dThetaLeft),
				  radius * std::sin(-thetaToIntersect + CLHEP::pi + i*dThetaLeft)));
	}
    }
  
  return BDSPolygon(r).ApplyTiltOffset(tiltOffset);
  */
}
