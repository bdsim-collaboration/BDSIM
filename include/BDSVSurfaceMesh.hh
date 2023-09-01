/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2023.

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
#ifndef BDSVSURFACEMESH_H
#define BDSVSURFACEMESH_H

#ifdef USE_CGAL_BOOLEAN_PROCESSOR
#include "G4Types.hh"

#include <vector>

class G4Polyhedron;
class G4PolyhedronArbitrary;

class BDSVSurfaceMesh
{
public:
  BDSVSurfaceMesh();
  virtual ~BDSVSurfaceMesh();

  virtual void Fill(G4Polyhedron *polyIn);
  virtual G4PolyhedronArbitrary* GetPolyhedronArbitrary();
  virtual void AddVertex(double x, double y, double z) = 0 ;
  virtual void AddFace(G4int i1, G4int i2, G4int i3) = 0;
  virtual void AddFace(G4int i1, G4int i2, G4int i3, G4int i4) = 0;
  virtual std::vector<G4double> GetVertex(G4int iVertex) = 0;
  virtual std::vector<G4int> GetFace(G4int iFace) = 0;
  virtual int NumberOfVertices() = 0;
  virtual int NumberOfFaces() = 0;
};
#endif
#endif