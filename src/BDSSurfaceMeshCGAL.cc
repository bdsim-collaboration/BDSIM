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
#ifdef USE_CGAL_BOOLEAN_PROCESSOR

#include "BDSSurfaceMeshCGAL.hh"

#include "G4Polyhedron.hh"

#include <CGAL/Polygon_mesh_processing/corefinement.h>

#include <vector>


BDSSurfaceMeshCGAL::BDSSurfaceMeshCGAL()
{;}

BDSSurfaceMeshCGAL::~BDSSurfaceMeshCGAL()
{;}

void BDSSurfaceMeshCGAL::Fill(G4Polyhedron* polyIn)
{
  BDSVSurfaceMesh::Fill(polyIn);
  CGAL::Polygon_mesh_processing::triangulate_faces(sm);
}

BDSSurfaceMeshCGAL* BDSSurfaceMeshCGAL::Subtraction(BDSSurfaceMeshCGAL* s1)
{
  Surface_mesh s2 = Surface_mesh();
  CGAL::Polygon_mesh_processing::corefine_and_compute_difference(sm, s1->sm, s2);

  BDSSurfaceMeshCGAL* res = new BDSSurfaceMeshCGAL();
  res->sm = s2;

  return res;
}

BDSSurfaceMeshCGAL* BDSSurfaceMeshCGAL::Union(BDSSurfaceMeshCGAL* s1)
{
  Surface_mesh s2 = Surface_mesh();
  CGAL::Polygon_mesh_processing::corefine_and_compute_union(sm, s1->sm, s2);

  BDSSurfaceMeshCGAL* res = new BDSSurfaceMeshCGAL();
  res->sm = s2;

  return res;
}

BDSSurfaceMeshCGAL* BDSSurfaceMeshCGAL::Intersection(BDSSurfaceMeshCGAL *s1)
{
  Surface_mesh s2 = Surface_mesh();
  CGAL::Polygon_mesh_processing::corefine_and_compute_intersection(sm, s1->sm, s2);

  BDSSurfaceMeshCGAL* res = new BDSSurfaceMeshCGAL();
  res->sm = s2;

  return res;
}

void BDSSurfaceMeshCGAL::AddVertex(double x, double y, double z)
{
  Point p(x, y, z);
  sm.add_vertex(p);
}

void BDSSurfaceMeshCGAL::AddFace(int i1, int i2, int i3)
{
  sm.add_face(Surface_mesh::Vertex_index(i1),
              Surface_mesh::Vertex_index(i2),
              Surface_mesh::Vertex_index(i3));
}

void BDSSurfaceMeshCGAL::AddFace(int i1, int i2, int i3, int i4)
{
  sm.add_face(Surface_mesh::Vertex_index(i1),
              Surface_mesh::Vertex_index(i2),
              Surface_mesh::Vertex_index(i3), Surface_mesh::Vertex_index(i4));
}

std::vector<G4double> BDSSurfaceMeshCGAL::GetVertex(G4int iVertex)
{
  std::vector<G4double> v;
  Surface_mesh::Vertex_index vi = Surface_mesh::Vertex_index(iVertex);
  Surface_mesh::Point p = sm.point(vi);
  v.push_back(CGAL::to_double(p.x()));
  v.push_back(CGAL::to_double(p.y()));
  v.push_back(CGAL::to_double(p.z()));
  return v;
}

std::vector<G4int> BDSSurfaceMeshCGAL::GetFace(G4int iFace)
{
  std::vector<G4int> f;
  Surface_mesh::Face_index fd = Surface_mesh::Face_index(iFace);

  for (Surface_mesh::Halfedge_index hd : CGAL::halfedges_around_face(sm.halfedge(fd), sm))
    {f.push_back((int)sm.source(hd));}
  return f;
}

int BDSSurfaceMeshCGAL::NumberOfVertices()
{
  return sm.number_of_vertices();
}

int BDSSurfaceMeshCGAL::NumberOfFaces()
{
  return sm.number_of_faces();
}

#endif