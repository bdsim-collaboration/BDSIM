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
#include "BDSAperturePointsLoader.hh"
#include "BDSBeamPipeFactoryPointsFile.hh"

#include "G4Types.hh"
#include "G4TwoVector.hh"

#include <algorithm>
#include <vector>

BDSBeamPipeFactoryPointsFile::BDSBeamPipeFactoryPointsFile():
  maximumRadius(0)
{;}

void BDSBeamPipeFactoryPointsFile::GeneratePoints(G4double, G4double, G4double, G4double,
						  G4double beamPipeThickness, G4int)
{
  maximumRadius = 0;
  
  // the product will not own this points vector pointer - the cache does inside this function
  std::vector<G4TwoVector>* rawPoints = BDS::LoadAperturePoints(pointsFile, pointsUnit);
  
  vacuumEdge = std::vector<G4TwoVector>(rawPoints->begin(), rawPoints->end());

  // calculate the unit vector for each point - so a direction from the 0,0 point to that point
  std::vector<G4TwoVector> unitR;
  unitR.reserve(vacuumEdge.size());
  for (const auto& v : vacuumEdge)
    {unitR.emplace_back(v.unit());}
  
  for (G4int i = 0; i < (G4int)vacuumEdge.size(); i++)
    {
      auto ur = unitR[i];
      beamPipeInnerEdge.emplace_back(       vacuumEdge[i] + ur*lengthSafetyLarge);
      beamPipeOuterEdge.emplace_back(       vacuumEdge[i] + ur*(lengthSafetyLarge   + beamPipeThickness));
      containerEdge.emplace_back(           vacuumEdge[i] + ur*(2*lengthSafetyLarge + beamPipeThickness));
      containerSubtractionEdge.emplace_back(vacuumEdge[i] + ur*(3*lengthSafetyLarge + beamPipeThickness));
      maximumRadius = std::max(maximumRadius, containerSubtractionEdge.back().mag());
    }
}

G4double BDSBeamPipeFactoryPointsFile::CalculateIntersectionRadius(G4double, G4double, G4double, G4double, G4double)
{
  return maximumRadius;
}
