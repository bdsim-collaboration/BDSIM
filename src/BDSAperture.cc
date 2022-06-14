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
#include "BDSApertureType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSPolygon.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "G4String.hh"
#include "G4Types.hh"

BDSAperture::BDSAperture(BDSApertureType apertureTypeIn):
  apertureType(apertureTypeIn),
  tiltOffset(BDSTiltOffset())
{;}

BDSAperture::BDSAperture(BDSApertureType      apertureTypeIn,
			 const BDSTiltOffset& tiltOffsetIn):
  apertureType(apertureTypeIn),
  tiltOffset(tiltOffsetIn)
{;}

void BDSAperture::CheckRequiredParametersSet(G4double aper1, G4bool aper1Set,
					     G4double aper2, G4bool aper2Set,
					     G4double aper3, G4bool aper3Set,
					     G4double aper4, G4bool aper4Set) const
{
  if (aper1Set && !BDS::IsFinite(aper1))
    {throw BDSException("","\"aper1\" not set, but required to be.");}
  if (aper2Set && !BDS::IsFinite(aper2))
    {throw BDSException("","\"aper2\" not set, but required to be.");}
  if (aper3Set && !BDS::IsFinite(aper3))
    {throw BDSException("","\"aper3\" not set, but required to be.");}
  if (aper4Set && !BDS::IsFinite(aper4))
    {throw BDSException("","\"aper4\" not set, but required to be.");}
}

void BDSAperture::CheckParameterIsPositive(G4double        parameter,
					   const G4String& nameToWarn) const
{
  if (parameter <= 0)
    {throw BDSException(__METHOD_NAME__, "parameter \"" + nameToWarn + "\" is not >= 0");}
}

G4bool BDSAperture::LessThan(const BDSAperture* other) const
{
  if (!other)
    {return false;}

  return Polygon().Inside(other->Polygon());
}

void BDSAperture::AppendAngleEllipse(std::vector<G4TwoVector>& vec,
				     G4double startAngle,
				     G4double finishAngle,
				     G4double radiusA,
				     G4double radiusB,
                                     unsigned int nPoints,
				     G4double xOffset,
				     G4double yOffset)
{
  G4double diff = finishAngle - startAngle;
  G4double delta = diff / (G4double)nPoints;
  for (G4double ang = startAngle; ang < finishAngle;)
    {// l for local
      G4double xl = xOffset + radiusA*std::sin(ang);
      G4double yl = yOffset + radiusB*std::cos(ang);
      vec.emplace_back(xl, yl);
      ang += delta;
    }
}

BDSPolygon BDSAperture::Polygon(unsigned int nPointsIn) const
{
  unsigned int mnp = MinimumNumberOfPoints();
  unsigned int rnp = RecommendedNumberOfPoints();
  unsigned int np = nPointsIn == 0 ? rnp : nPointsIn;
  if (np < mnp)
    {throw BDSException(__METHOD_NAME__, "number of points for aperture specified < " + std::to_string(mnp) + ".");}
  BDSPolygon r = PolygonNPoints(MinimumNumberOfPoints());
  return r.ApplyTiltOffset(tiltOffset);
}
