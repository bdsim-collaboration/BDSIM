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
#include "BDSArray1DCoords.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSFieldValue.hh"
#include "BDSGlobalConstants.hh"
#include "BDSInterpolatorRoutines.hh"
#include "BDSInterpolatorType.hh"
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
  vertexNormals(nullptr),
  extent(nullptr),
  radiusToEncompass(0),
  recalculateRadiusToEncompass(true),
  interpolation(BDSInterpolatorType::linear1d)
{
  if (points.size() < 3)
    {throw BDSException(__METHOD_NAME__, "polygon must have at least 3 x,y points to be valid.");}
  
  // determine the winding order and if wrong, flip it
  // we rely on this order, so we calculate the outward facing normals correctly
  // further down - the sign of the pi/2
  auto detV1 = points[1] - points[0];
  auto detV2 = points[2] - points[1];
  G4double det = Determinant(detV1, detV2);
  if (det < 0)
    {std::reverse(points.begin(), points.end());}
}

BDSPolygon::~BDSPolygon() noexcept
{
  delete vertexNormals;
  delete extent;
}

BDSPolygon::BDSPolygon(const BDSPolygon& other):
  points(other.points),
  vertexNormals(nullptr),
  extent(nullptr),
  radiusToEncompass(other.radiusToEncompass),
  recalculateRadiusToEncompass(other.recalculateRadiusToEncompass),
  interpolation(other.interpolation)
{
  if (other.vertexNormals)
    {vertexNormals = new std::vector<G4TwoVector>(*(other.vertexNormals));}
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
  if (!to.HasFiniteOffset() || !to.HasFiniteTilt())
    {return r;}
  G4double t = to.Tilt();
  G4TwoVector offset = to.GetOffset2D();
  for (auto& p : r)
    {
      p.rotate(t);
      p += offset;
    }
  return r;
}

BDSPolygon BDSPolygon::ExpandByValueUsingVertexNormals(G4double value) const
{
  if (!vertexNormals)
    {CalculateVertexNormals();}
  
  std::vector<G4TwoVector> newPoints;
  newPoints.reserve(points.size());
  for (G4int i = 0; i < (G4int)points.size(); i++)
    {
      auto ur = vertexNormals[i];
      newPoints.emplace_back(points[i] + (*vertexNormals)[i] * value);
    }
  return BDSPolygon(newPoints);
}

void BDSPolygon::ExpandByValueUsingVertexNormalsInPlace(G4double value)
{
  if (!vertexNormals)
    {CalculateVertexNormals();}
  
  for (G4int i = 0; i < (G4int)points.size(); i++)
    {
      auto ur = vertexNormals[i];
      G4TwoVector newValue = points[i] + (*vertexNormals)[i] * value;
      points[i] = newValue;
    }
  recalculateRadiusToEncompass = true;
}

BDSPolygon BDSPolygon::ScaleByValue(G4double scale) const
{
  std::vector<G4TwoVector> newPoints;
  newPoints.reserve(points.size());
  for (G4int i = 0; i < (G4int)points.size(); i++)
    {newPoints[i] = points[i] * scale;}
  return BDSPolygon(newPoints);
}

void BDSPolygon::ScaleByValueInPlace(G4double scale)
{
  for (G4int i = 0; i < (G4int)points.size(); i++)
    {
      G4TwoVector newValue = points[i] * scale;
      points[i] = newValue;
    }
  recalculateRadiusToEncompass = true;
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

G4double BDSPolygon::RadiusToEncompass() const
{
  if (recalculateRadiusToEncompass)
    {
      auto max = std::max_element(points.begin(), points.end(), [](const G4TwoVector& a, const G4TwoVector& b) {return a.mag() < b.mag();});
      radiusToEncompass = max->mag();
    }
  recalculateRadiusToEncompass = false;
  return radiusToEncompass;
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

BDSPolygon BDSPolygon::InterpolateWithNPoints(unsigned int nPointsNew) const
{
  BDSArray1DCoords array = BDSArray1DCoords((G4int)points.size() + 2,
                                            -1.0,
                                            (G4double)points.size() + 1.0);
  auto Convert = [](const G4TwoVector& a) { return BDSFieldValue((FIELDTYPET)a.x(), (FIELDTYPET)a.y(), 0); };
  auto ConvertBack = [](const BDSFieldValue& a) { return G4TwoVector((FIELDTYPET)a.x(), (FIELDTYPET)a.y()); };
  array(0) = Convert(points.back());
  for (G4int i = 0; i < (G4int)points.size(); i++)
    {array(i+1) = Convert(points[i]);}
  array((G4int)points.size()+1) = Convert(points[0]);

  std::vector<G4TwoVector> result;
  result.reserve(nPointsNew);
  
  G4double ratio = (G4double)points.size() / (G4double)nPointsNew;
  switch (interpolation.underlying())
    {
    case BDSInterpolatorType::linear1d:
      {
	for (unsigned int i = 0; i < nPointsNew; i++)
	  {
	    G4double newIndex = (G4double)i * ratio;
	    BDSFieldValue localData[2];
	    G4double xFrac;
	    array.ExtractSection2(newIndex, localData, xFrac);
	    result[i] = ConvertBack(BDS::Linear1D(localData, xFrac));
	  }
	break;
      }
    case BDSInterpolatorType::cubic1d:
      {
	for (unsigned int i = 0; i < nPointsNew; i++)
	  {
	    G4double newIndex = (G4double)i * ratio;
	    BDSFieldValue localData[4];
	    G4double xFrac;
	    array.ExtractSection4(newIndex, localData, xFrac);
	    result[i] = ConvertBack(BDS::Cubic1D(localData, xFrac));
	  }
	break;
      }
    default:
      {throw BDSException(__METHOD_NAME__, "invalid interpolation type \"" + interpolation.ToString() + "\"");}
    }
  
  auto resultPolygon = BDSPolygon(result);
  return resultPolygon;
}

void BDSPolygon::CalculateVertexNormals() const
{
  delete vertexNormals;
  vertexNormals = new std::vector<G4TwoVector>();
  vertexNormals->reserve(points.size());

  // This relies on a winding order established in the constructor.
  
  // Loop round the polygon and calculate the normal of each segment
  // sum the normals of two segments and normalise to give an outwards
  // pointing unit vector at each vertex. We have a bit of if else for
  // the boundaries of the loop for back / front.
  G4TwoVector v1;
  G4TwoVector v2;
  G4TwoVector v3;
  for (G4int i = 0; i < (G4int)points.size(); i++)
    {
      if (i == 0)
	{
	  v1 = points.back();
	  v3 = points[i+1];
	}
      else if (i == (G4int)points.size() - 1)
	{
	  v1 = points[i-1];
	  v3 = points[0];
	}
      else
	{
	  v1 = points[i-1];
	  v3 = points[i+1];
	}
      v2 = points[i];
      
      auto normal1 = v2 - v1;
      auto normal2 = v3 - v2;
      normal1.rotate(-CLHEP::halfpi); // now it's a normal
      normal2.rotate(-CLHEP::halfpi);
      
      auto vertexNormal = (normal1 + normal2).unit();
      
      vertexNormals->emplace_back(vertexNormal); // outward pointing direction for expansion
    }
}

G4double BDSPolygon::Determinant(const G4TwoVector& v1,
                                 const G4TwoVector& v2) const
{
  G4double det = v1.x()*v2.y() - v2.x()*v1.y();
  return det;
}
