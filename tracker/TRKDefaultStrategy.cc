
#include "TRKDefaultStrategy.hh"
#include "TRKBunch.hh"
#include "TRKElement.hh"
#include "TRKQuadrupole.hh"
#include "TRKSBend.hh"
#include "TRKMaps.hh"

#include <cmath>

void TRKDefaultStrategy::Track(TRKDrift* el, TRKBunch* bunch) {
  trk::maps::drift(*bunch, el->GetLength());
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

  auto sx = (rootx == 0) ? 1 : std::sin(rootxl) / rootx;
  auto cx = std::cos(rootxl);

  auto sy = (rooty == 0) ? 1 : std::sinh(rootyl) / rooty;
  auto cy = std::cosh(rootyl);

  for (auto &p : *bunch) {
    auto hbar = h / p.beta0;
    auto x = p.x;
    auto px = p.px;
    auto y = p.y;
    auto py = p.py;
    auto pz = p.pz;

    // Matrix multiplication part
    p.x = (x * cx
	   + px * sx
	   + pz * hbar * (1 - cx) / std::pow(rootx, 2));

    p.px = (x * -std::pow(rootx, 2) * sx
	    + px * cx
	    + pz * hbar * sx);

    p.y = y * cy + py * sy;
    p.py = y * std::pow(rooty, 2) * sy + py * cy;

    // Vector addition part
    p.x += (h - k0) * (1 - cx) / std::pow(rootxl, 2);
    p.px += (h - k0) * sx;

    // TODO: z (longitudinal path lenght difference)

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
      auto px = p.px;
      auto y = p.y;
      auto py = p.py;

      p.x = x * f11 + px * f12;
      p.px += x * f21 + px * f22;

      p.y = y * df11 + py * df12;
      p.py += y * df21 + py * df22;

      p.z += p.pz * length / std::pow(p.beta0*p.gamma0, 2);
    }

  // Calculate matrix terms
}

void TRKDefaultStrategy::Track(TRKSextupole *, TRKBunch *) {}

void TRKDefaultStrategy::Track(TRKOctupole *, TRKBunch *) {}

void TRKDefaultStrategy::Track(TRKSolenoid *, TRKBunch *) {}

void TRKDefaultStrategy::Track(TRKRBend *, TRKBunch *) {

}
