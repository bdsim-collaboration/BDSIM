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
#ifndef TRKApertureCircular_h
#define TRKApertureCircular_h 1

#include "TRKAperture.hh"
#include <ostream>

class TRKParticle;
class TRKBunch;

/**
 * @brief Circular aperture class
 */
class TRKApertureCircular: public TRKAperture {
public:
  TRKApertureCircular(double radius);
  virtual ~TRKApertureCircular();

  virtual bool          OutsideAperture(const TRKParticle& particle);
  virtual std::ostream& PrintDetails   (std::ostream& out) const;
  void                  CheckAperture  (TRKBunch* bunch);
  
private:
  double radius;
  double radiusSq;
  TRKApertureCircular(); ///< not implemented
};

#endif
