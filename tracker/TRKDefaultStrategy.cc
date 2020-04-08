
#include "TRKDefaultStrategy.hh"
#include "TRKBunch.hh"
#include "TRKElement.hh"
#include "TRKQuadrupole.hh"
#include "TRKSBend.hh"

#include <cmath>

void TRKDefaultStrategy::Track(TRKDrift* el, TRKBunch* bunch) {
  double length = el->GetLength();

  for (auto &p : *bunch)
    {
      p.x += p.xp * length;
      p.y += p.yp * length;
      p.ct += p.dp * length / std::pow(p.beta0*p.gamma0, 2);
    }

}

void TRKDefaultStrategy::Track(TRKSBend *, TRKBunch *) {

void TRKDefaultStrategy::Track(TRKDipole *, TRKBunch *) {}

void TRKDefaultStrategy::Track(TRKQuadrupole *el, TRKBunch *bunch) {
  auto k1 = el->GetStrength();
  auto length = el->GetLength();

  auto rootk = std::sqrt(k1);
  auto rootkl = rootk * length;

  // Focusing 2x2 matrix terms
  auto f11 = std::cos(rootkl);
  auto f12 = std::sin(rootkl) / rootk;
  auto f21 = std::sin(rootkl) *-rootk;
  auto f22 = f11;

  // Defocusing 2x2 matrix terms
  auto df11 = std::cosh(rootkl);
  auto df12 = std::sinh(rootkl) / rootk;
  auto df21 = std::sinh(rootkl) * rootk;
  auto df22 = df11;

  for (auto &p : *bunch)
    {
      auto x = p.x;
      auto xp = p.xp;
      auto y = p.y;
      auto yp = p.yp;

      p.x = x * f11 + xp * f12;
      p.xp += x * f21 + xp * f22;

      p.y = y * df11 + yp * df12;
      p.yp += y * df21 + yp * df22;

      p.ct += p.dp * length / std::pow(p.beta0*p.gamma0, 2);
    }

  // Calculate matrix terms
}

void TRKDefaultStrategy::Track(TRKSextupole *, TRKBunch *) {}

void TRKDefaultStrategy::Track(TRKOctupole *, TRKBunch *) {}

void TRKDefaultStrategy::Track(TRKSolenoid *, TRKBunch *) {}

void TRKDefaultStrategy::Track(TRKRBend *, TRKBunch *) {

}
