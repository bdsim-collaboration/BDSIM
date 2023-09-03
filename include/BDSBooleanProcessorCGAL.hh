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
#ifndef BDSBOOLEANPROCESSOR_H
#define BDSBOOLEANPROCESSOR_H

#ifdef USE_CGAL_BOOLEAN_PROCESSOR

#include "G4VBooleanProcessor.hh"
#include "G4Polyhedron.hh"
#include "G4PolyhedronArbitrary.hh"

/**
 * @brief Custom Boolean mesh processor using CGAL.
 *
 * @author Stewart Boogert
 */

class BDSBooleanProcessorCGAL: public G4VBooleanProcessor
{
public:
  BDSBooleanProcessorCGAL();
  virtual ~BDSBooleanProcessorCGAL();

  /// @{ Overload the relevant mesh generation for each Boolean operation.
  virtual G4PolyhedronArbitrary* Intersection(G4Polyhedron* p1, G4Polyhedron* p2);
  virtual G4PolyhedronArbitrary* Union(G4Polyhedron* p1, G4Polyhedron* p2);
  virtual G4PolyhedronArbitrary* Subtraction(G4Polyhedron* p1, G4Polyhedron* p2);
  /// @}
};

#endif
#endif
