/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#ifndef TRKApertureEllipsoidal_h
#define TRKApertureEllipsoidal_h 1

#include "TRKAperture.hh"
#include "TRKParticle.hh"
#include <iostream>

/**
 * @brief Ellipsoidal aperture class
 */
class TRKApertureEllipsoidal: public TRKAperture {
public:
  TRKApertureEllipsoidal(double xWidthIn, double yWidthIn);
  virtual ~TRKApertureEllipsoidal();

  virtual bool          OutsideAperture(const TRKParticle& particle);
  virtual std::ostream& PrintDetails   (std::ostream& out) const;
  void                  CheckAperture  (TRKBunch* bunch);

private:
  double xWidth;
  double yWidth;
  //note these are specified separately so the subsequent
  //variables can be set in the initialiser list by
  //performing calculations on these variables
  double xWidthSq;
  double yWidthSq;
  double xyWidthSqProd;
  TRKApertureEllipsoidal(); ///< not implemented
};

#endif
