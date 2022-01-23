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
#ifndef TRKDecapole_h
#define TRKDecapole_h

#include "TRKElement.hh"

/**
 * @brief Sextupole tracking. Strength in Tesla/meter^3.
 */

class TRKDecapole: public TRKElement
{ 
public:
  TRKDecapole() = delete;
  TRKDecapole(std::string   name,
	      double        length,
	      double        k5,
	      TRKAperture  *aperture,
	      TRKPlacement *placement);

  void Track(TRKParticle &particle, double step,
             TRKStrategy *strategy) override;

  double GetK5() { return k5; }

private:
  double k5;
};

#endif
