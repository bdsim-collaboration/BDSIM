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
#include "BDSBeamPipeFactoryOctagonal.hh"
#include "BDSBeamPipeFactoryPoints.hh"

#include "G4TwoVector.hh"
#include "G4Types.hh"

#include <cmath>

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
      G4int npoints = pointsPerTwoPi / 4;
      npoints = npoints < 4 ? 4 : npoints;
      G4TwoVector r(x1-cornerRadius, 0);
      for (G4int i = 0; i < npoints; i++)
        {


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
  G4double a3 = aper3 + distance;
  
  BDS::FourPoints result = {a1,a2,a3};
  return result;
}

void BDSBeamPipeFactoryRhombus::GeneratePoints(G4double aper1,
                                               G4double aper2,
                                               G4double aper3,
                                               G4double /*aper4*/,
                                               G4double beamPipeThickness,
                                               G4int    pointsPerTwoPi)
{
  GenerateRhombus(vacuumEdge, aper1, aper2, aper3);
  BDS::ThreePoints ai = ExpandRhombus(aper1, aper2, aper3, lengthSafetyLarge);
  GenerateRhombus(beamPipeInnerEdge, ai.aper1, ai.aper2, ai.aper3);
  BDS::ThreePoints ao = ExpandRhombus(aper1, aper2, aper3, lengthSafetyLarge + beamPipeThickness);
  GenerateRhombus(beamPipeOuterEdge, ao.aper1, ao.aper2, ao.aper3);
  BDS::ThreePoints ac = ExpandRhombus(aper1, aper2, aper3, beamPipeThickness + 2*lengthSafetyLarge);
  GenerateRhombus(containerEdge, ac.aper1, ac.aper2, ac.aper3);
  BDS::ThreePoints acs = ExpandRhombus(aper1, aper2, aper3, beamPipeThickness + 3*lengthSafetyLarge);
  GenerateRhombus(containerSubtractionEdge, acs.aper1, acs.aper2, acs.aper3);

  extentX = acs.aper1;
  extentY = acs.aper2;
}

G4double BDSBeamPipeFactoryRhombus::CalculateIntersectionRadius(G4double aper1,
                                                                G4double aper2,
                                                                G4double aper3,
                                                                G4double /*aper4*/,
                                                                G4double beamPipeThickness)
{
  // We calculate the expanded aper123 parameters corresponding to the outermost rhombus
  // this has to be the same as the last ExpandRhombus in GeneratePoints()
  BDS::FourPoints acs = ExpandRhombus(aper1, aper2, aper3, beamPipeThickness + 2*lengthSafetyLarge);
  
  G4double result = std::max(acs.aper1, acs.aper2);
  result += lengthSafetyLarge;
  return result;
}
