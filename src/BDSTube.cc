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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSTube.hh"

#include "G4TessellatedSolid.hh"
#include "G4ThreeVector.hh"
#include "G4TriangularFacet.hh"
#include "G4TwoVector.hh"
#include "G4Types.hh"
#include "G4VFacet.hh" // defines G4FacetVertexType

#include <string>
#include <vector>

BDSTube::BDSTube(const G4String& nameIn,
		 G4double        fullLengthIn,
		 const std::vector<G4TwoVector>& startingPoints):
  G4TessellatedSolid(nameIn)
{
  G4double l = fullLengthIn*0.5;
  const std::vector<G4TwoVector>& sp = startingPoints; // shortcut
  typedef G4ThreeVector G4TV; // shortcut
  
  // full length must be > 0
  if (fullLengthIn <= 0)
    {throw BDSException(__METHOD_NAME__, "length <= 0");}
  
  if (startingPoints.size() < 3)
    {throw BDSException(__METHOD_NAME__, "too few points (" + std::to_string(startingPoints.size()) + ")");}
  
  // check for self-intersecting - TODO

  // make and add facets
  G4int limit = (G4int)sp.size();
  for (G4int i = 0; i < limit; i++)
    {
      G4int j = i + 1;
      if (j == limit)
	{j = 0;}
      G4TriangularFacet* facet1 = new G4TriangularFacet(G4TV(sp[i].x(), sp[i].y(), -l),
							G4TV(sp[j].x(), sp[j].y(), -l),
							G4TV(sp[i].x(), sp[i].y(),  l),
							G4FacetVertexType::ABSOLUTE);
      G4TriangularFacet* facet2 = new G4TriangularFacet(G4TV(sp[j].x(), sp[j].y(), -l),
							G4TV(sp[j].x(), sp[j].y(),  l),
							G4TV(sp[i].x(), sp[i].y(),  l),
							G4FacetVertexType::ABSOLUTE);
      G4TriangularFacet* facetF = new G4TriangularFacet(G4TV(0,         0,         -l),
							G4TV(sp[j].x(), sp[j].y(), -l),
							G4TV(sp[i].x(), sp[i].y(), -l),
							G4FacetVertexType::ABSOLUTE);
      G4TriangularFacet* facetB = new G4TriangularFacet(G4TV(0,         0,          l),
							G4TV(sp[i].x(), sp[i].y(),  l),
							G4TV(sp[j].x(), sp[j].y(),  l),
							G4FacetVertexType::ABSOLUTE);
      AddFacet((G4VFacet*)facet1);
      AddFacet((G4VFacet*)facet2);
      AddFacet((G4VFacet*)facetF);
      AddFacet((G4VFacet*)facetB);
    }
  SetSolidClosed(true);
}

BDSTube::BDSTube(const G4String& nameIn,
		 G4double        dZNegative,
		 G4double        dZPositive,
		 const std::vector<G4TwoVector>& startingPoints,
		 const std::vector<G4TwoVector>& finishingPoints):
  G4TessellatedSolid(nameIn)
{
  const std::vector<G4TwoVector>& sp = startingPoints; // shortcut
  const std::vector<G4TwoVector>& fp = finishingPoints; // shortcut
  typedef G4ThreeVector G4TV; // shortcut

  // full length must be > 0
  if (dZPositive <= dZNegative)
    {throw BDSException(__METHOD_NAME__, "length <= 0");}
  if (startingPoints.size() != finishingPoints.size())
    {throw BDSException(__METHOD_NAME__, "mismatched sizes of points.");}
  if (startingPoints.size() < 3)
    {throw BDSException(__METHOD_NAME__, "too few points (" + std::to_string(startingPoints.size()) + ")");}
  if (finishingPoints.size() < 3)
    {throw BDSException(__METHOD_NAME__, "too few points (" + std::to_string(startingPoints.size()) + ")");}
  
  // if rotation between points > 90deg, add 1 z plane per 90deg - TODO
  std::vector<G4double> z = {dZNegative, dZPositive};
  
  for (G4int zi = 0; zi < (G4int)z.size() - 1; zi++)
    {
      G4double za = z[zi];
      G4double zb = z[zi+1];

      G4int limit = (G4int)sp.size();
      for (G4int i = 0; i < limit; i++)
	{
	  G4int j = i + 1;
	  if (j == limit)
	    {j = 0;}
	  G4TriangularFacet* facet1 = new G4TriangularFacet(G4TV(sp[i].x(), sp[i].y(), za),
							    G4TV(sp[j].x(), sp[j].y(), za),
							    G4TV(fp[i].x(), fp[i].y(), zb),
							    G4FacetVertexType::ABSOLUTE);
	  G4TriangularFacet* facet2 = new G4TriangularFacet(G4TV(sp[j].x(), sp[j].y(), za),
							    G4TV(fp[j].x(), fp[j].y(), zb),
							    G4TV(fp[i].x(), fp[i].y(), zb),
							    G4FacetVertexType::ABSOLUTE);
	  G4TriangularFacet* facetF = new G4TriangularFacet(G4TV(0,         0,         za),
							    G4TV(sp[j].x(), sp[j].y(), za),
							    G4TV(sp[i].x(), sp[i].y(), za),
							    G4FacetVertexType::ABSOLUTE);
	  G4TriangularFacet* facetB = new G4TriangularFacet(G4TV(0,         0,         zb),
							    G4TV(fp[i].x(), fp[i].y(), zb),
							    G4TV(fp[j].x(), fp[j].y(), zb),
							    G4FacetVertexType::ABSOLUTE);
	  AddFacet((G4VFacet*)facet1);
	  AddFacet((G4VFacet*)facet2);
          AddFacet((G4VFacet*)facetF);
          AddFacet((G4VFacet*)facetB);
	}
    }
  SetSolidClosed(true);
}
