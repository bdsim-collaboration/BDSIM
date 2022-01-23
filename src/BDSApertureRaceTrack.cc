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
#include "BDSAperture.hh"
#include "BDSApertureCurved.hh"
#include "BDSApertureRaceTrack.hh"
#include "BDSApertureType.hh"
#include "BDSExtent.hh"
#include "BDSPolygon.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "G4TwoVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <array>
#include <vector>

BDSApertureRaceTrack::BDSApertureRaceTrack(G4double xIn,
					                       G4double yIn,
					                       G4double radiusIn,
                                           unsigned int nPointsIn):
  BDSApertureCurved(BDSApertureType::racetrack, nPointsIn),
  x(xIn),
  y(yIn),
  radius(radiusIn)
{;}

G4bool BDSApertureRaceTrack::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureRaceTrack* oc = dynamic_cast<const BDSApertureRaceTrack*>(other);
      return BDS::DoublesAreEqual(oc->x, x) &&
	  BDS::DoublesAreEqual(oc->y, y) &&
	  BDS::DoublesAreEqual(oc->radius, radius);
    }
}

void BDSApertureRaceTrack::CheckInfoOK() const
{
  CheckRequiredParametersSet(x, true, y, true, radius, true);
  CheckParameterIsPositive(x, "x");
  CheckParameterIsPositive(y, "y");
  CheckParameterIsPositive(radius, "radius");
}

BDSExtent BDSApertureRaceTrack::Extent() const
{
  BDSExtent simpleExtent(x + radius, y + radius, 0);
  return ExtentOffsetTilt(simpleExtent);
}

BDSApertureRaceTrack BDSApertureRaceTrack::operator+ (G4double number) const
{
  BDSApertureRaceTrack result = BDSApertureRaceTrack(*this);
  result += number;
  return result;
}

const BDSApertureRaceTrack& BDSApertureRaceTrack::operator+=(G4double number)
{
  x += number;
  y += number;
  radius += number;
  return *this;
}

BDSApertureRaceTrack BDSApertureRaceTrack::operator* (G4double number) const
{
  BDSApertureRaceTrack result = BDSApertureRaceTrack(*this);
  result *= number;
  return result;
}

const BDSApertureRaceTrack& BDSApertureRaceTrack::operator*=(G4double number)
{
  x *= number;
  y *= number;
  radius *= number;
  return *this;
}

BDSAperture* BDSApertureRaceTrack::Plus(G4double number) const
{
  BDSApertureRaceTrack result = (*this) + number;
  return new BDSApertureRaceTrack(result);
}

BDSAperture* BDSApertureRaceTrack::Times(G4double number) const
{
  BDSApertureRaceTrack result = (*this) * number;
  return new BDSApertureRaceTrack(result);
}

BDSAperture* BDSApertureRaceTrack::Clone() const
{
  return new BDSApertureRaceTrack(*this);
}

std::array<G4double,7> BDSApertureRaceTrack::ApertureNumbers() const
{
  return {x,y,radius,0,tiltOffset.OffsetX(),tiltOffset.OffsetY(),tiltOffset.Tilt()};
}

BDSPolygon BDSApertureRaceTrack::PolygonNPoints(unsigned int nPointsIn) const
{
  nPointsIn = BDS::NextMultiple(nPointsIn, 4); // ensure multiple of 4
  std::vector<G4TwoVector> r;
  r.reserve(nPointsIn);
  G4int pointsPerCurve = nPointsIn / 4;
  AppendAngleEllipse(r, 0, 0.5 * CLHEP::pi, radius, radius, pointsPerCurve, x, y);
  r.emplace_back(x + radius, y);
  AppendAngleEllipse(r, 0.5 * CLHEP::pi, CLHEP::pi, radius, radius, pointsPerCurve, x, -y);
  r.emplace_back(x, -y - radius);
  AppendAngleEllipse(r, CLHEP::pi, (3./2.) * CLHEP::pi, radius, radius, pointsPerCurve, -x, -y);
  r.emplace_back(-x - radius, -y);
  AppendAngleEllipse(r, (3. / 2) * CLHEP::pi, CLHEP::twopi, radius, radius, pointsPerCurve, -x, y);
  r.emplace_back(-x, y + radius);
  return BDSPolygon(r);
}
