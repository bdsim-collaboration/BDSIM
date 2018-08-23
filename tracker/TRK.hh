/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#ifndef TRK_h
#define TRK_h

#include <cmath>

namespace TRK {
  // tracking parameters
  /// steps for thin lens tracking
  const int DEFAULT_TRACKING_STEPS=10;
  /// maximum number of turns for rings
  const int NR_TURNS=200;

  // constants
  /// speed of light
  const double SOL=3.0e8;
  /// electric charge
  const double e = 1.6e-19;

  /** tracking type
   * thin: thin lens tracking
   * thin symplectic
   * thick: thick lens tracking
   * hybrid: Geant4 field stepper
   */
  //enum TRKType 
  enum Strategy {THIN,THINSYMPLECTIC,THICK,HYBRID};
  enum Aperture {CIRCULAR,RECTANGULAR,ELLIPSOIDAL};

  /** 
   * fast way to calculate both sinh and cosh
   * syntax similar to sincos(phi,s,c),
   * except sincosh expects doubles and no pointers!
   */
  static inline void sincosh(const double a, double &s, double &c)
  {  
    if (std::abs(a) <= 0.05)
      {
	s = std::sinh(a);
	c = std::sqrt(1.0 + s*s);
      }
    else
      {
	double ea = std::exp(a);
	double inv_ea = 1./(ea);
	s = (ea - inv_ea)*0.5;
	c = (ea + inv_ea)*0.5;
      }
  }
}

#endif
