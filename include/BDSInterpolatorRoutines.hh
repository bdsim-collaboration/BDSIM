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
#ifndef BDSINTERPOLATORROUTINES_H
#define BDSINTERPOLATORROUTINES_H

#include "BDSFieldValue.hh"

#include "G4Types.hh"

namespace BDS
{
  /// Same as below but for a type T instead of a 3-vector. T must have
  /// the +,- and * operators implemented.
  template<class T>
  inline T Linear1D(T v0,
		    T v1,
		    G4double t)
  {return v0*(1.-t) + v1*t;};
  
  /// Linear interpolation in 1 dimension. Uses slightly different
  /// version of the classic formula that's equivalent to guarantee
  /// value = vmax when x = 1.
  BDSFieldValue Linear1D(const BDSFieldValue p[2],
			 const G4double      x);

  /// Linear interpolation in 2 dimensions.
  BDSFieldValue Linear2D(const BDSFieldValue p[2][2],
			 const G4double      x,
			 const G4double      y);

  /// Linear interpolation in 3 dimensions.
  BDSFieldValue Linear3D(const BDSFieldValue p[2][2][2],
			 const G4double      x,
			 const G4double      y,
			 const G4double      z);

  /// Linear interpolation in 4 dimensions.
  BDSFieldValue Linear4D(const BDSFieldValue p[2][2][2][2],
			 const G4double      x,
			 const G4double      y,
			 const G4double      z,
			 const G4double      t);

  /// Cubic interpolation in 1 dimension.
  BDSFieldValue Cubic1D(const BDSFieldValue p[4],
			const G4double      x);
  
  /// Cubic interpolation in 2 dimensions.
  BDSFieldValue Cubic2D(const BDSFieldValue p[4][4],
			const G4double      x,
			const G4double      y);

  /// Cubic interpolation in 3 dimensions.
  BDSFieldValue Cubic3D(const BDSFieldValue p[4][4][4],
			const G4double      x,
			const G4double      y,
			const G4double      z);

  /// Cubic interpolation in 4 dimensions.
  BDSFieldValue Cubic4D(const BDSFieldValue p[4][4][4][4],
			const G4double      x,
			const G4double      y,
			const G4double      z,
			const G4double      t);
}

#endif
