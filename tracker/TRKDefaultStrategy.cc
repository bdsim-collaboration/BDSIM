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
#include "TRKDefaultStrategy.hh"
#include "TRKBunch.hh"
#include "TRKElement.hh"
#include "TRKKicker.hh"
#include "TRKMaps.hh"
#include "TRKQuadrupole.hh"
#include "TRKSextupole.hh"
#include "TRKOctupole.hh"
#include "TRKSBend.hh"
#include "TRKRBend.hh"
#include "TRKDipoleFringe.hh"

#include <cmath>

void TRKDefaultStrategy::Track(TRKDrift* el, TRKParticle& particle, double step) {
  trk::maps::drift(particle, step);
}

void TRKDefaultStrategy::Track(TRKSBend* el, TRKParticle& particle, double step) {
  trk::maps::sbend(particle, step, el->GetAngle() * step / el->GetLength(), el->GetK1());
}

void TRKDefaultStrategy::Track(TRKDipoleFringe* el, TRKParticle& particle, double step) {
    auto poleface = el->GetPoleface();
    double k0 = el->GetK0();

    trk::maps::dipole_fringe(particle, k0, poleface);
}

void TRKDefaultStrategy::Track(TRKRBend* el, TRKParticle& particle, double step) {
  auto angle = el->GetAngle();
  double length = el->GetLength();
  double k0 = angle / length;

  auto poleface = angle / 2;
  trk::maps::dipole_fringe(particle, k0, poleface);
  trk::maps::sbend(particle, step, k0, el->GetK1());
  trk::maps::dipole_fringe(particle, k0, poleface);
}

void TRKDefaultStrategy::Track(TRKQuadrupole* el, TRKParticle& particle, double step) {
  trk::maps::quadrupole(particle, step, el->GetK1());
}

void TRKDefaultStrategy::Track(TRKSextupole* el, TRKParticle& particle, double step) {
    trk::maps::drift(particle, step / 2);
    trk::maps::sextupole(particle, step, el->GetK2());
    trk::maps::drift(particle, step / 2);
}

void TRKDefaultStrategy::Track(TRKOctupole *el, TRKParticle& particle, double step) {
  trk::maps::drift(particle, step);
}

void TRKDefaultStrategy::Track(TRKKicker *el, TRKParticle& particle, double step) {

  auto length = el->GetLength();
  auto hkick = el->GetHKick();
  auto vkick = el->GetVKick();

  if (length == 0.0)
    {
      trk::maps::kicker(particle, hkick, vkick);
    }
  else
    {
      hkick *= step / length;
      vkick *= step / length;
      trk::maps::kicker(particle, hkick/2., vkick/2.);
      trk::maps::drift(particle, step);
      trk::maps::kicker(particle, hkick/2., vkick/2.);
    }
}
