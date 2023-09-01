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
#include "G4Version.hh"

#if G4VERSION_NUMBER > 1120

#include "BDSBooleanProcessorCGAL.hh"
#include "BDSSurfaceMeshCGAL.hh"

#include "G4Polyhedron.hh"
#include "G4PolyhedronArbitrary.hh"


BDSBooleanProcessorCGAL::BDSBooleanProcessorCGAL()
{;}

BDSBooleanProcessorCGAL::~BDSBooleanProcessorCGAL()
{;}

G4PolyhedronArbitrary* BDSBooleanProcessorCGAL::Intersection(G4Polyhedron* p1, G4Polyhedron* p2)
{
  BDSSurfaceMeshCGAL* sm1 = new BDSSurfaceMeshCGAL();
  BDSSurfaceMeshCGAL* sm2 = new BDSSurfaceMeshCGAL();
  sm1->Fill(p1);
  sm2->Fill(p2);
  BDSSurfaceMeshCGAL* sm3 = sm1->Intersection(sm2);
  G4PolyhedronArbitrary* ap = sm3->GetPolyhedronArbitrary();
  delete sm1;
  delete sm2;
  delete sm3;
  return ap;
}

G4PolyhedronArbitrary* BDSBooleanProcessorCGAL::Union(G4Polyhedron* p1, G4Polyhedron* p2)
{
  BDSSurfaceMeshCGAL* sm1 = new BDSSurfaceMeshCGAL();
  BDSSurfaceMeshCGAL* sm2 = new BDSSurfaceMeshCGAL();
  sm1->Fill(p1);
  sm2->Fill(p2);
  BDSSurfaceMeshCGAL* sm3 = sm1->Union(sm2);
  G4PolyhedronArbitrary* ap = sm3->GetPolyhedronArbitrary();
  delete sm1;
  delete sm2;
  delete sm3;
  return ap;
}

G4PolyhedronArbitrary* BDSBooleanProcessorCGAL::Subtraction(G4Polyhedron* p1, G4Polyhedron* p2)
{
  BDSSurfaceMeshCGAL* sm1 = new BDSSurfaceMeshCGAL();
  BDSSurfaceMeshCGAL* sm2 = new BDSSurfaceMeshCGAL();
  sm1->Fill(p1);
  sm2->Fill(p2);
  BDSSurfaceMeshCGAL* sm3 = sm1->Subtraction(sm2);
  G4PolyhedronArbitrary* ap = sm3->GetPolyhedronArbitrary();
  delete sm1;
  delete sm2;
  delete sm3;
  return ap;
}

#endif