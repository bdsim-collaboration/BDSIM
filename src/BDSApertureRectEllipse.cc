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
#include "BDSApertureEllpse.hh"
#include "BDSApertureRectangle.hh"
#include "BDSApertureRectEllipse.hh"
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

BDSApertureRectEllipse::BDSApertureRectEllipse(G4double rectangleAIn,
					       G4double rectangleBIn,
					       G4double ellipseAIn,
					       G4double ellipseBIn):
  BDSAperture(BDSApertureType::rectellipse),
  rectangleA(rectangleAIn),
  rectangleB(rectangleBIn),
  ellipseA(ellipseAIn),
  ellipseB(ellipseBIn)
{;}

G4bool BDSApertureRectEllipse::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureRectEllipse* oc = dynamic_cast<const BDSApertureRectEllipse*>(other);
      return BDS::DoublesAreEqual(oc->rectangleA, rectangleA) &&
	BDS::DoublesAreEqual(oc->rectangleB, rectangleB) &&
	BDS::DoublesAreEqual(oc->ellipseA, ellipseA) &&
      	BDS::DoublesAreEqual(oc->ellipseB, ellipseB);
    }
}

void BDSApertureRectEllipse::CheckInfoOK() const
{
  CheckRequiredParametersSet(rectangleA, true, rectangleB, true, ellipseA, true, ellipseB, true);
  CheckParameterIsPositive(rectangleA, "rectangleA");
  CheckParameterIsPositive(rectangleB, "rectangleB");
  CheckParameterIsPositive(ellipseA,   "ellipseA");
  CheckParameterIsPositive(ellipseB,   "ellipseB");
}

BDSExtent BDSApertureRectEllipse::Extent() const
{
  
  BDSExtent simpleExtent(std::min(rectangleA, ellipseA), std::min(rectangleB, ellipseB), 0);
  return ExtentOffsetTilt(simpleExtent);
}

BDSApertureRectEllipse BDSApertureRectEllipse::operator+ (const G4double number) const
{
  BDSApertureRectEllipse result = BDSApertureRectEllipse(*this);
  result += number;
  return result;
}

const BDSApertureRectEllipse& BDSApertureRectEllipse::operator+=(const G4double number)
{
  rectangleA += number;
  rectangleB += number;
  ellipseA   += number;
  ellipseB   += number;
  return *this;
}

BDSApertureRectEllipse BDSApertureRectEllipse::operator* (const G4double number) const
{
  BDSApertureRectEllipse result = BDSApertureRectEllipse(*this);
  result *= number;
  return result;
}

const BDSApertureRectEllipse& BDSApertureRectEllipse::operator*=(const G4double number)
{
  rectangleA *= number;
  rectangleB *= number;
  ellipseA   *= number;
  ellipseB   *= number;
  return *this;
}

BDSPolygon BDSApertureRectEllipse::Polygon(G4int nPointsIn) const
{
  G4int np = nPointsIn == 0 ? MinimumNumberOfPoints() : nPointsIn;
  if (np < MinimumNumberOfPoints())
    {throw BDSException(__METHOD_NAME__, "number of points for aperture specified < " + std::to_string(MinimumNumberOfPoints()) + ".");}

  BDSApertureEllpse  ae = BDSApertureEllpse(ellipseA, ellipseB, np);
  BDSApertureRectangle ar = BDSApertureRectangle(rectangleA, rectangleB);
  BDSPolygon pEllipse = ae.Polygon();
  BDSPolygon pRectangle = ar.Polygon();
  BDSPolygon result = pEllipse.Intersection(pRectangle);
  return result;
  

  /*
  std::vector<G4TwoVector> r;

  if (rectangleA < ellipseA) // vertical
    {
      // calculate angle between x axis and x,y point where rectangle of width
      // a intercepts circle of radius r where a < r
      G4double thetaToIntersect = std::acos(rectangleA/ellipseA);
      G4int    npTopHalf        = (G4int)std::nearbyint(np / 2);
      G4int    npBottomHalf     = np - npTopHalf;
      G4double dThetaTop    = 2*thetaToIntersect / (G4double)npTopHalf;
      G4double dThetaBottom = 2*thetaToIntersect / (G4double)npBottomHalf;
      for (G4int i = 0; i < npTopHalf; i++)
	{
	  r.push_back(G4TwoVector(ellipseA * std::cos(thetaToIntersect + i*dThetaTop),
				  ellipseB * std::sin(thetaToIntersect + i*dThetaTop)));
	}
      for (G4int i = 0; i < npBottomHalf; i++)
	{
	  r.push_back(G4TwoVector(ellipseA * std::cos(thetaToIntersect + CLHEP::pi + i*dThetaBottom),
				  ellipseB * std::sin(thetaToIntersect + CLHEP::pi + i*dThetaBottom)));
	}
    }
  else // horizontal
    {
      // calculate angle between x axis and x,y point where rectangle of width
      // a intercepts circle of radius r where b < r
      G4double thetaToIntersect = std::asin(rectangleB/ellipseB);
      G4int    npRightHalf = (G4int)std::nearbyint(np / 2);
      G4int    npLeftHalf  = np - npRightHalf;
      G4double dThetaRight = 2*thetaToIntersect / (G4double)npRightHalf;
      G4double dThetaLeft  = 2*thetaToIntersect / (G4double)npLeftHalf;
      for (G4int i = 0; i < npRightHalf; i++)
	{
	  r.push_back(G4TwoVector(ellipseA * std::cos(-thetaToIntersect + i*dThetaRight),
				  ellipseB * std::sin(-thetaToIntersect + i*dThetaRight)));
	}
      for (G4int i = 0; i < npLeftHalf; i++)
	{
	  r.push_back(G4TwoVector(ellipseA * std::cos(-thetaToIntersect + CLHEP::pi + i*dThetaLeft),
				  ellipseB * std::sin(-thetaToIntersect + CLHEP::pi + i*dThetaLeft)));
	}
    }
  
  return BDSPolygon(r).ApplyTiltOffset(tiltOffset);
  */
}
