/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSApertureRhombus.hh"
//#include "BDSBeamPipeFactoryPoints.hh"

#include "G4TwoVector.hh"
#include "G4Types.hh"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

#include "BDSAperture.hh"
#include "BDSApertureOctagon.hh"
#include "BDSApertureType.hh"
#include "BDSExtent.hh"
#include "BDSPolygon.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

BDSApertureRhombus::BDSApertureRhombus(G4double xIn,
                                       G4double yIn,
                                       G4double cornerRadiusIn):
  BDSAperture(BDSApertureType::rhombus),
  x(xIn),
  y(yIn),
  cornerRadius(cornerRadiusIn)
{;}

G4bool BDSApertureRhombus::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureRhombus* oc = dynamic_cast<const BDSApertureRhombus*>(other);
      return BDS::DoublesAreEqual(oc->x, x) &&
        BDS::DoublesAreEqual(oc->y, y) &&
        BDS::DoublesAreEqual(oc->cornerRadius, cornerRadius);
    }
}

void BDSApertureRhombus::CheckInfoOK() const
{
  CheckRequiredParametersSet(x, true, y, true, cornerRadius, true);
  CheckParameterIsPositive(x, "x");
  CheckParameterIsPositive(y, "y");
  CheckParameterIsPositive(cornerRadius, "cornerRadius");
}

G4double BDSApertureRhombus::RadiusToEncompass() const
{
  return 0; //TBC
  //return std::max(std::hypot(x,yEdge), std::hypot(xEdge,y)) + tiltOffset.Radius();
}

BDSExtent BDSApertureRhombus::Extent() const
{
  BDSExtent simpleExtent(x, y, 0);
  return ExtentOffsetTilt(simpleExtent);
}

BDSApertureRhombus BDSApertureRhombus::operator+ (G4double number) const
{
  BDSApertureRhombus result = BDSApertureRhombus(*this);
  result += number;
  return result;
}

const BDSApertureRhombus& BDSApertureRhombus::operator+=(G4double number)
{
  x += number;
  y += number;
  cornerRadius += number;
  return *this;
}

BDSApertureRhombus BDSApertureRhombus::operator* (G4double number) const
{
  BDSApertureRhombus result = BDSApertureRhombus(*this);
  result *= number;
  return result;
}

const BDSApertureRhombus& BDSApertureRhombus::operator*=(G4double number)
{
  x *= number;
  y *= number;
  cornerRadius *= number;
  return *this;
}

BDSAperture* BDSApertureRhombus::Plus(G4double number) const
{
  BDSApertureRhombus result = (*this) + number;
  return new BDSApertureRhombus(result);
}

BDSAperture* BDSApertureRhombus::Times(G4double number) const
{
  BDSApertureRhombus result = (*this) * number;
  return new BDSApertureRhombus(result);
}

BDSAperture* BDSApertureRhombus::Clone() const
{
  return new BDSApertureRhombus(*this);
}

std::array<G4double,7> BDSApertureRhombus::ApertureNumbers() const
{
  return {x,y,cornerRadius,0,tiltOffset.OffsetX(),tiltOffset.OffsetY(),tiltOffset.Tilt()};
}

BDSPolygon BDSApertureRhombus::PolygonNPoints(unsigned int nPointsIn) const
{
  nPointsIn = BDS::NextMultiple(nPointsIn, 8); // ensure multiple of 8
  std::vector<G4TwoVector> r;
  /*
  /// TODO deal with nPoints
  r.emplace_back(G4TwoVector( x,      yEdge));
  r.emplace_back(G4TwoVector( x,     -yEdge));
  r.emplace_back(G4TwoVector( xEdge, -y));
  r.emplace_back(G4TwoVector(-xEdge, -y));
  r.emplace_back(G4TwoVector(-x,     -yEdge));
  r.emplace_back(G4TwoVector(-x,      yEdge));
  r.emplace_back(G4TwoVector(-xEdge,  y));
  r.emplace_back(G4TwoVector( xEdge,  y));
  */
  return BDSPolygon(r);
}
/*
BDSBeamPipeFactoryRhombus::BDSBeamPipeFactoryRhombus()
{;}

void BDSBeamPipeFactoryRhombus::GenerateRhombus(std::vector<G4TwoVector>& vec,
                                                G4double x1,
                                                G4double y1,
                                                G4double cornerRadius,
                                                G4int pointsPerTwoPi)
{
  G4double frac = cornerRadius / std::max(std::abs(x1), std::abs(y1));
  if (frac > 1e-3)
    {// build with corner radius
      // The rhombus may be asymmetric, in which case building a pi/2 range of curve
      // won't result in a smooth transition to the straight sections. We need to work
      // out what range of angle to cover to come to the right tangent.
      // consider top curved point -> alpha = angle between y axis and (0,y1) -> (x1,0) line
      G4double alpha = std::atan2(std::abs(x1), std::abs(y1));
      G4double halfAngle = CLHEP::halfpi - alpha;
      std::vector<G4TwoVector> topBit;
      G4double nPointsTopDouble = (2*halfAngle/CLHEP::twopi) * pointsPerTwoPi;
      G4int nPointsTop = std::max(4, (G4int)std::ceil(nPointsTopDouble)); // ensure at least 4 points
      G4double currentAngle = -halfAngle;
      G4double dAngle = 2*halfAngle / (G4double)nPointsTop;
      G4TwoVector rotationPointTop(0, y1-cornerRadius);
      for (G4int i = 0; i <= nPointsTop; i++)
        {
          G4TwoVector r(0, cornerRadius);
          r.rotate(-currentAngle);
          topBit.push_back(rotationPointTop + r);
          currentAngle += dAngle;
        }

      std::vector<G4TwoVector> rightBit;
      G4double nPointsRightDouble = (2*alpha/CLHEP::twopi) * pointsPerTwoPi;
      G4int nPointsRight = std::max(4, (G4int)std::ceil(nPointsRightDouble)); // ensure at least 4 points
      currentAngle = -alpha;
      dAngle = 2*alpha / (G4double)nPointsRight;
      G4TwoVector rotationPointRight(x1-cornerRadius, 0);
      for (G4int i = 0; i <= nPointsRight; i++)
        {
          G4TwoVector r(cornerRadius, 0);
          r.rotate(-currentAngle);
          topBit.push_back(rotationPointRight + r);
          currentAngle += dAngle;
        }

      for (const auto& p : topBit)
        {vec.push_back(p);}
      for (const auto& p : rightBit)
        {vec.push_back(p);}
      for (const auto& p : topBit)
        {vec.emplace_back(-p.x(), -p.y());}
      for (const auto& p : rightBit)
        {vec.emplace_back(-p.x(), -p.y());}
      std::reverse(vec.begin(), vec.end());
    }
  else
    {// build just as a rhombus
      AppendPoint(vec,  x1,    0);
      AppendPoint(vec,   0,  -y1);
      AppendPoint(vec, -x1,    0);
      AppendPoint(vec,   0,   y1);
    }
}

BDS::ThreePoints BDSBeamPipeFactoryRhombus::ExpandRhombus(G4double aper1,
                                                          G4double aper2,
                                                          G4double aper3,
                                                          G4double distance)
{ 
  G4double a1 = aper1 + distance;
  G4double a2 = aper2 + distance;
  G4double a3 = aper3 > 0 ? aper3 + distance : aper3;
  
  BDS::ThreePoints result = {a1,a2,a3};
  return result;
}

void BDSBeamPipeFactoryRhombus::GeneratePoints(G4double aper1,
                                               G4double aper2,
                                               G4double aper3,
                                               G4double aper4,
                                               G4double beamPipeThickness,
                                               G4int    pointsPerTwoPi)
{
  GenerateRhombus(vacuumEdge, aper1, aper2, aper3, pointsPerTwoPi);
  BDS::ThreePoints ai = ExpandRhombus(aper1, aper2, aper3, lengthSafetyLarge);
  GenerateRhombus(beamPipeInnerEdge, ai.aper1, ai.aper2, ai.aper3, pointsPerTwoPi);
  BDS::ThreePoints ao = ExpandRhombus(aper1, aper2, aper3, lengthSafetyLarge + beamPipeThickness);
  GenerateRhombus(beamPipeOuterEdge, ao.aper1, ao.aper2, ao.aper3, pointsPerTwoPi);
  BDS::ThreePoints ac = ExpandRhombus(aper1, aper2, aper3, beamPipeThickness + 2*lengthSafetyLarge);
  GenerateRhombus(containerEdge, ac.aper1, ac.aper2, ac.aper3, pointsPerTwoPi);
  BDS::ThreePoints acs = ExpandRhombus(aper1, aper2, aper3, beamPipeThickness + 3*lengthSafetyLarge);
  GenerateRhombus(containerSubtractionEdge, acs.aper1, acs.aper2, acs.aper3, pointsPerTwoPi);

  extentX = acs.aper1;
  extentY = acs.aper2;
}

G4double BDSBeamPipeFactoryRhombus::CalculateIntersectionRadius(G4double aper1,
                                                                G4double aper2,
                                                                G4double aper3,
                                                                G4double aper4,
                                                                G4double beamPipeThickness)
{
  G4double dThickness = beamPipeThickness + 2*lengthSafetyLarge;
  
  G4double result = std::max(aper1 + dThickness, aper2 + dThickness);
  result += lengthSafetyLarge;
  return result;
}
*/
