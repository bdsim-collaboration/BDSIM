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
  BDSTube(nameIn, fullLengthIn, startingPoints, startingPoints)
{;}

BDSTube::BDSTube(const G4String& nameIn,
		 G4double        fullLengthIn,
		 const std::vector<G4TwoVector>& startingPoints,
		 const std::vector<G4TwoVector>& finishingPoints):
  BDSTube(nameIn, -0.5*fullLengthIn, 0.5*fullLengthIn, startingPoints, finishingPoints)
{;}

BDSTube::BDSTube(const G4String& nameIn,
		 G4double        dZNegative,
		 G4double        dZPositive,
		 const std::vector<G4TwoVector>& startingPoints,
		 const std::vector<G4TwoVector>& finishingPoints):
  G4TessellatedSolid(nameIn)
{
  // checks
  // full length must be > 0
  if (dZPositive <= dZNegative)
    {throw BDSException(__METHOD_NAME__, "length <= 0");}
  if (startingPoints.size() < 3)
    {throw BDSException(__METHOD_NAME__, "too few points (" + std::to_string(startingPoints.size()) + ")");}
  if (finishingPoints.size() < 3)
    {throw BDSException(__METHOD_NAME__, "too few points (" + std::to_string(finishingPoints.size()) + ")");}

  unsigned int spSize = (unsigned int)startingPoints.size();
  unsigned int fpSize = (unsigned int)finishingPoints.size();
  if (spSize != fpSize)
    {
      if (spSize % fpSize == 0 || fpSize % spSize == 0)
	{SubMultipleConstruction(dZNegative, dZPositive, startingPoints, finishingPoints);}
      else
	{throw BDSException(__METHOD_NAME__, "mismatched sizes of points.");}
    }
  else
    {
      // if rotation between points > 90deg, add 1 z plane per 90deg - TODO
      std::vector<G4double> z = {dZNegative, dZPositive};
      RegularConstruction(z, startingPoints, finishingPoints);   
    }
}

void BDSTube::RegularConstruction(const std::vector<G4double>&    z,
				  const std::vector<G4TwoVector>& startingPoints,
				  const std::vector<G4TwoVector>& finishingPoints)
{
  if (z.size() < 2)
    {throw BDSException(__METHOD_NAME__, "must be at least 2 z positions.");}
  
  // shortcuts / aliases
  const std::vector<G4TwoVector>& sp = startingPoints;
  const std::vector<G4TwoVector>& fp = finishingPoints;
  typedef G4ThreeVector G4TV;
  
  for (G4int zi = 0; zi < (G4int)z.size() - 1; zi++)
    {
      G4double za = z[zi];
      G4double zb = z[zi+1]; // TODO only 2 points for now
      
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

void BDSTube::SubMultipleConstruction(G4double dZNegative,
				      G4double dZPositive,
				      const std::vector<G4TwoVector>& startingPoints,
				      const std::vector<G4TwoVector>& finishingPoints)
{
  SetSolidClosed(true);
}
