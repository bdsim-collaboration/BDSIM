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
#ifndef BDSPOLYGON_H
#define BDSPOLYGON_H

#include "G4TwoVector.hh"
#include "G4Types.hh"

#include <iterator>
#include <vector>

class BDSExtent;
class BDSTiltOffset;

/**
 * @brief A 2D polygon made of line segments.
 *
 * @author Laurie Nevay
 */

class BDSPolygon
{
public:
  BDSPolygon(const std::vector<G4TwoVector>& pointsIn);
  ~BDSPolygon();

  /// Accessor.
  const std::vector<G4TwoVector>& Points() const {return points;}
  
  /// Return true if point inside this polygon.
  G4bool Inside(const G4TwoVector& point) const;

  /// Return true if polygon is completely contained inside this one.
  /// If any lines intersect, this will return false.
  G4bool Inside(const BDSPolygon& other) const;

  /// Return true if any segments that make this polyon intersect with
  /// any others.
  G4bool SelfIntersecting(G4int *const indexOfIntersectionA = nullptr,
			  G4int *const indexOfIntersectionB = nullptr) const;
  
  /// Return a copy with a tilt and offset applied.
  BDSPolygon ApplyTiltOffset(const BDSTiltOffset& to) const;

  BDSExtent Extent() const;

  ///@{ Iterator mechanics.
  typedef std::vector<G4TwoVector>::iterator               iterator;
  typedef std::vector<G4TwoVector>::const_iterator         const_iterator;
  typedef std::vector<G4TwoVector>::reverse_iterator       reverse_iterator;
  typedef std::vector<G4TwoVector>::const_reverse_iterator const_reverse_iterator;
  iterator               begin()        {return points.begin();}
  iterator               end()          {return points.end();}
  const_iterator         begin()  const {return points.begin();}
  const_iterator         end()    const {return points.end();}
  reverse_iterator       rbegin()       {return points.rbegin();}
  reverse_iterator       rend()         {return points.rend();}
  const_reverse_iterator rbegin() const {return points.rbegin();}
  const_reverse_iterator rend()   const {return points.rend();}
  G4bool                 empty()  const {return points.empty();}
  ///@}

  inline G4TwoVector front() const {return points.front();}
  inline G4TwoVector back()  const {return points.back();}
  std::vector<G4TwoVector>::size_type size() const {return points.size();}

  /// Algorithm to test if two straight line segments (segments are finite lines
  /// with a start and end point) intersect at any point. Returns 0 if no intersection,
  /// 1 if a unique intersection and 2 if the same line. This algorithm is from
  /// Geometric Tools for Computer Graphics by Philip J. Schneider David H. Eberly,
  /// ISBN 1-55860-594-0. Geant4 has a similar one in
  /// G4TessellatedGeometryAlgorithms::IntersectLineAndLineSegment2D, however they've
  /// parmaterised it in a different way that's relative to the first point with graidents
  /// and don't fully explain their input parameters.
  static G4int SegmentsIntersect(const G4TwoVector& p1,
				 const G4TwoVector& p2,
				 const G4TwoVector& q1,
				 const G4TwoVector& q2,
				 G4TwoVector* intersectionPoint = nullptr);

private:
  BDSPolygon() = delete;
  
  std::vector<G4TwoVector> points;

  /// Cache of extent if calculated - initialised on first call to Extent().
  mutable BDSExtent* extent;
};

#endif
