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
#ifndef TRKSextupole_h
#define TRKSextupole_h

#include "TRKElement.hh"

/**
 * @brief Sextupole. Strength in Tesla/meter^2.
 */

class TRKSextupole: public TRKElement
{
public:
  TRKSextupole() = delete;
  TRKSextupole(std::string  name,
	       double       length,
	       double       k2,
	       TRKAperture  *aperture,
	       TRKPlacement *placement);
  void Track(TRKParticle &particle, double step,
             TRKStrategy *strategy) override;
  double GetK2() const { return k2; }

private:
  double k2;
};

#endif
