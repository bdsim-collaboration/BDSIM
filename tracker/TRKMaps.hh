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
#ifndef MAPS_H
#define MAPS_H

#include "TRKParticle.hh"

namespace trk {
namespace maps {

void drift(TRKParticle &, double length) noexcept;
void nldrift(TRKParticle &, double length) noexcept;
void drift(TRKBunch &, double length) noexcept;
void nldrift(TRKBunch &, double length) noexcept;
void sbend(TRKBunch &, double length, double angle, double k1 = 0.0) noexcept;
void sbend(TRKParticle &, double length, double angle, double k1 = 0.0) noexcept;
void dipole_fringe(TRKParticle &, double k0, double angle) noexcept;
void dipole_fringe(TRKBunch &, double k0, double angle) noexcept;
void quadrupole(TRKParticle &, double length, double k1) noexcept;
void quadrupole(TRKBunch &, double length, double k1) noexcept;
void sextupole(TRKParticle &, double length, double k1) noexcept;
void kicker(TRKParticle &, double hkick, double vkick) noexcept;

} // namespace maps

} // namespace trk
#endif /* MAPS_H */
