/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPolygon.hh"
#include "BDSTiltOffset.hh"

#include "G4TwoVector.hh"
#include "G4Types.hh"

#include <algorithm>
#include <limits>
#include <list>
#include <vector>

BDSPolygon::BDSPolygon(const std::vector<G4TwoVector>& pointsIn):
  points(pointsIn),
  extent(nullptr),
  interpolation(BDSInterpolatorType::linear1d)
{
  if (points.size() < 3)
    {throw BDSException(__METHOD_NAME__, "polygon must have at least 3 x,y points to be valid.");}
}

BDSPolygon::~BDSPolygon()
{
  delete extent;
}

BDSPolygon::BDSPolygon(const BDSPolygon& other):
  points(other.points),
  extent(nullptr),
  interpolation(other.interpolation)
{
  if (other.extent)
    {extent = new BDSExtent(*other.extent);}
}

G4bool BDSPolygon::Inside(const G4TwoVector& point) const
{
  G4bool result = true;
  for (G4int i = 0; i < (G4int)size() - 1; i++)
    {
      G4TwoVector test = points[i] - point;
      G4TwoVector norm = (points[i+1] - points[i]).orthogonal();
      result &= test.dot(norm) > 0;
    }
  return result;
}

BDSPolygon::PointIs BDSPolygon::InsideLabelled(const G4TwoVector& point) const
{
  return Inside(point) ? BDSPolygon::PointIs::in : BDSPolygon::PointIs::out;
}


G4bool BDSPolygon::Inside(const BDSPolygon& other) const
{
  // test all segments don't intersect -> doesn't overlap
  // test one point is inside
  G4bool result = true;
  for (G4int i = 0; i < (G4int)size() - 1; i++)
    {
      for (G4int j = 0; j < (G4int)other.size() - 1; j++)
	{result &= !SegmentsIntersect(points[i], points[i+1], other.points[j], other.points[j+1]);}
    }
  if (!result) // don't bother with inside point check
    {return result;}

  // test one point is inside.
  result &= other.Inside(points[0]);
  return result;
}

G4bool BDSPolygon::SelfIntersecting(G4int* const indexOfIntersectionA,
				    G4int* const indexOfIntersectionB) const
{
  G4bool result = false;
  for (G4int i = 0; i < (G4int)size() - 1; i++)
    {
      for (G4int j = i + 1; j < (G4int)size() - 1; j++)
	{
	  result |= SegmentsIntersect(points[i], points[i+1], points[j], points[j+1]);
	  if (result)
	    {// for optional feedback
	      if (indexOfIntersectionA)
		{*indexOfIntersectionA = i;}
	      if (indexOfIntersectionB)
		{*indexOfIntersectionB = j;}
	      return result;
	    }
	}
    }
  return result;
}

BDSPolygon BDSPolygon::ApplyTiltOffset(const BDSTiltOffset& to) const
{
  BDSPolygon r(*this);
  G4double    t      = to.Tilt();
  G4TwoVector offset = to.GetOffset2D();
  for (auto& p : r)
    {
      p.rotate(t);
      p += offset;
    }
  return r;
}

BDSExtent BDSPolygon::Extent() const
{
  if (extent)
    {return BDSExtent(*extent);}

  G4double extXNeg = 0;
  G4double extXPos = 0;
  G4double extYNeg = 0;
  G4double extYPos = 0;
  for (const auto& p : points)
    {
      extXNeg = std::min(extXNeg, p.x());
      extXPos = std::max(extXPos, p.x());
      extYNeg = std::min(extYNeg, p.y());
      extYPos = std::max(extYPos, p.y());
    }

  G4double lsl = BDSGlobalConstants::Instance()->LengthSafetyLarge();
  extent = new BDSExtent(extXNeg, extXPos,
			 extYNeg, extYPos,
			 -lsl,    lsl);
  return BDSExtent(*extent);
}

G4int BDSPolygon::SegmentsIntersect(const G4TwoVector& p1,
				    const G4TwoVector& p2,
				    const G4TwoVector& q1,
				    const G4TwoVector& q2,
				    G4TwoVector* intersectionPoint)
{
  // Use a relative error test to test for parallelism.  This effectively
  // is a threshold on the angle between D0 and D1.  The threshold
  // parameter ’sqrEpsilon’ can be defined in this function or be
  // available globally.
  const G4double eps = std::numeric_limits<double>::epsilon() * std::abs(std::max(p1.x(), p1.y()));
  const G4double sqrEpsilon = eps*eps;
  G4TwoVector E        = q1 - p1;
  G4double    kross    = p2.x() * q2.y() - p2.y() * q2.x();
  G4double    sqrKross = kross * kross;
  G4double    sqrLen0  = p2.x() * p2.x() + p2.y() * p2.y();
  G4double    sqrLen1  = q2.x() * q2.x() + q2.y() * q2.y();
  if (sqrKross > sqrEpsilon * sqrLen0 * sqrLen1)
    {
      // lines are not parallel
      G4double s = (E.x() * q2.y() - E.y() *q2.x()) / kross;
      if (intersectionPoint)
	{*intersectionPoint = p1 + s * p2;}
      return 1;
    }
  
  // lines are parallel
  G4double sqrLenE = E.x() * E.x() + E.y() * E.y();
  kross = E.x() * p2.y() - E.y() * p2.x();
  sqrKross = kross * kross;
  if (sqrKross > sqrEpsilon * sqrLen0 * sqrLenE)
    {// lines are different
      return 0;
    }
  
  // lines are the same
  return 2;
}

std::list<BDSPolygon::LabelledPoint> BDSPolygon::GenerateLabelled(const BDSPolygon& reference,
                                           const BDSPolygon& test,
                                           G4int* nInside)
{
  std::list<BDSPolygon::LabelledPoint> testLabelled;
  G4int nIn = 0;
  for (const auto& p : test)
    {
      auto inOut = reference.InsideLabelled(p);
      nIn += inOut == BDSPolygon::PointIs::in ? 1 : 0;
      testLabelled.emplace_back(BDSPolygon::LabelledPoint{&p, inOut});
    }
  if (nInside)
    {*nInside = nIn;}
  return testLabelled;
}

BDSPolygon BDSPolygon::Union(const BDSPolygon& other) const
{
  // loop over points in other and mark as inside or outside this polygon
  G4int nOtherInThis = 0;
  std::list<BDSPolygon::LabelledPoint> otherLabelled = GenerateLabelled(*this, other, &nOtherInThis);
  G4int nThisInOther = 0;
  std::list<BDSPolygon::LabelledPoint> thisLabelled = GenerateLabelled(other, *this, &nOtherInThis);

  // if all outside search for intersections between all segments
  if (nOtherInThis == 0)
    {
      // check if any segments intersect

      // if no intersections and all outside, then throw exception -> no disjoint unions
    }
  else
    {
      // generate intersection points between this and other polygon

    }
  
  return BDSPolygon(*this);
}

std::vector<BDSPolygon*> BDSPolygon::Subtraction(const BDSPolygon& other) const
{return std::vector<BDSPolygon*>();}

BDSPolygon BDSPolygon::Intersection(const BDSPolygon& other) const
{
  return BDSPolygon(*this);
}
