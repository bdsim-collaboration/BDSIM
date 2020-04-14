#include <cmath>

#include "TRKBunch.hh"
#include "TRKMaps.hh"
#include "TRKParticle.hh"


void trk::maps::drift(TRKParticle &p, double length) noexcept {
  p.x += p.px * length;
  p.y += p.py * length;
  p.z += p.pz * length / std::pow(p.beta0 * p.gamma0, 2);
}

void trk::maps::drift(TRKBunch &b, double length) noexcept {
  for (auto &p : b) {
    drift(p, length);
  }
}

// void quadrupole(TRKParticle part, double length, float k1l) noexcept
// {
//   // part.x
// }
