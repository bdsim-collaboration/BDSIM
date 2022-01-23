/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#ifndef TRKDipoleFringe_h
#define TRKDipoleFringe_h

#include "TRKElement.hh"

/**
 * @brief dipole fringe
 * 
 * dipole fringe tracking
 */

class TRKDipoleFringe: public TRKElement
{
public:
    TRKDipoleFringe() = delete;
    TRKDipoleFringe(std::string   name,
	   double        poleface,
	   TRKAperture  *aperture,
	   TRKPlacement *placement,
	   double        k0);
  
  void Track(TRKParticle& particle, double step, TRKStrategy* strategy) override;

  inline double GetPoleface() const {return poleface;}
  inline double GetK0() const {return k0;}
  
private:
  /// Poleface in radians
  double poleface;
  double k0;
};

#endif
