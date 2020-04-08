
#include "TRKDefaultStrategy.hh"
#include "TRKBunch.hh"
#include "TRKElement.hh"
#include "TRKQuadrupole.hh"
#include "TRKSBend.hh"
#include "TRKMaps.hh"

#include <cmath>

void TRKDefaultStrategy::Track(TRKDrift* el, TRKBunch* bunch) {
  double length = el->GetLength();

  trk::maps::drift(*bunch, length)

  // for (auto &p : *bunch)
  //   {
  //     p.x += p.xp * length;
  //     p.y += p.yp * length;
  //     p.ct += p.dp * length / std::pow(p.beta0*p.gamma0, 2);
  //   }

}

void TRKDefaultStrategy::Track(TRKSBend *el, TRKBunch *bunch) {

  auto angle = el->GetAngle();
  auto length = el->GetLength();
  auto rho = angle / length;
  auto k0 = 1. / rho;
  auto h = k0;
  auto k1 = el->GetK1();

  auto rootx = std::sqrt(h * k0 + k1);
  auto rootxl = rootx * length;
  auto rooty = std::sqrt(k1);
  auto rootyl = rooty * length;

  auto sx = std::sin(rootxl) / rootx;
  auto cx = std::cos(rootxl);

  auto sy = std::sinh(rootyl) / rooty;
  auto cy = std::cosh(rootyl);

  for (auto &p : *bunch) {
    auto hbar = h / p.beta0;
    auto x = p.x;
    auto xp = p.xp;
    auto y = p.y;
    auto yp = p.yp;
    auto dp = p.dp;

    // Matrix multiplication part
    p.x = (x * cx
	   + xp * sx
	   + dp * hbar * (1 - cx) / std::pow(rootx, 2));

    p.xp = (x * -std::pow(rootx, 2) * sx
	    + xp * cx
	    + dp * hbar * sx);

    p.y = y * cy + yp * sy;
    p.yp = y * std::pow(rooty, 2) * sy + yp * cy;

    // Vector addition part
    p.x += (h - k0) * (1 - cx) / std::pow(rootxl, 2);
    p.xp += (h - k0) * sx;

    // TODO: ct (longitudinal path lenght difference)

  }
}

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
