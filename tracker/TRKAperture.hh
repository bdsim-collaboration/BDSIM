/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#ifndef TRKAperture_h
#define TRKAperture_h 1

#include "TRK.hh"

#include "TRKParticle.hh"
#include "TRKBunch.hh"

#include <ostream>

/**
 * @brief Aperture base class
 */

class TRKAperture
{
public:
  TRKAperture(){};
  virtual ~TRKAperture(){}; 

  /// checks if aperture is hit - specific to each type of aperture
  //  the actual algorithm here and purely virtual
  virtual bool OutsideAperture(const TRKParticle& particle)=0;
  
  /// loop over bunch and apply aperture test - implemented in this base class
  TRKBunch* CheckAperture(TRKBunch* bunch);

  virtual std::ostream& PrintDetails(std::ostream& out) const = 0;
  //you can't make a friend operator virtual so wrap the real output in a method 
  //and use that polymorphically
  friend std::ostream& operator<< (std::ostream& out, const TRKAperture& aperture);
};

#endif
