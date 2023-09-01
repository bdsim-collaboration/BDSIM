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
#ifndef BDSSURFACEMESHCGAL_H
#define BDSSURFACEMESHCGAL_H

#ifdef USE_CGAL_BOOLEAN_PROCESSOR
class G4Polyhedron;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wgnu-statement-expression"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "CGAL/Aff_transformation_3.h"
#include "CGAL/Exact_predicates_exact_constructions_kernel.h"
#include "CGAL/Surface_mesh.h"
#include "CGAL/Polygon_mesh_processing/corefinement.h"
#include "CGAL/Polygon_mesh_processing/orientation.h"
#include "CGAL/Polygon_mesh_processing/repair.h"
#include "CGAL/Polygon_mesh_processing/transform.h"
#include "CGAL/Polygon_mesh_processing/triangulate_faces.h"


typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_3                                   Point;
typedef Kernel::Vector_3                                  Vector;
typedef CGAL::Surface_mesh<Kernel::Point_3>               Surface_mesh;
typedef CGAL::Aff_transformation_3<Kernel>                Aff_transformation_3;
#pragma GCC diagnostic pop

#include "BDSVSurfaceMesh.hh"

#include <vector>

class BDSSurfaceMeshCGAL: public BDSVSurfaceMesh
{
public:
  BDSSurfaceMeshCGAL();
  virtual ~BDSSurfaceMeshCGAL();

  virtual void Fill(G4Polyhedron* polyIn);

  BDSSurfaceMeshCGAL* Subtraction(BDSSurfaceMeshCGAL* surfaceMesh);
  BDSSurfaceMeshCGAL* Union(BDSSurfaceMeshCGAL* surfaceMesh);
  BDSSurfaceMeshCGAL* Intersection(BDSSurfaceMeshCGAL* surfaceMesh);

  void AddVertex(double x, double y, double z);
  void AddFace(int i1, int i2, int i3);
  void AddFace(int i1, int i2, int i3, int i4);
  std::vector<G4double> GetVertex(G4int iVertex);
  std::vector<G4int> GetFace(G4int iFace);
  virtual int NumberOfVertices();
  virtual int NumberOfFaces();

private:
  Surface_mesh sm;
};
#endif

#endif
