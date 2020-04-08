#include <cmath>

#include "TRKBunch.hh"
#include "TRKMaps.hh"
#include "TRKParticle.hh"


void trk::maps::drift(TRKParticle &p, double length) noexcept {
  p.x += p.xp * length;
  p.y += p.yp * length;
  p.ct += p.dp * length / std::pow(p.beta0 * p.gamma0, 2);
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
