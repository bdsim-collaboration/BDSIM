#include <cmath>

#include "TRKBunch.hh"
#include "TRKMaps.hh"
#include "TRKParticle.hh"

void trk::maps::drift(TRKParticle &p, double length) noexcept {
  p.x += p.px * length;
  p.y += p.py * length;
  p.z += p.pz * length / std::pow(p.beta0 * p.gamma0, 2);
}

void trk::maps::drift(TRKBunch &bunch, double length) noexcept {
  for (auto &p : bunch) {
    drift(p, length);
  }
}

void trk::maps::sbend(TRKBunch &bunch, double length, double angle,
                      double k1) noexcept {

  auto rho = length / angle;
  auto k0 = 1. / rho;
  auto h = k0;

  auto rootx = std::sqrt(h * k0 + k1);
  auto rootxl = rootx * length;
  auto rooty = std::sqrt(k1);
  auto rootyl = rooty * length;

  auto sx = (rootx == 0) ? length : std::sin(rootxl) / rootx;
  auto cx = std::cos(rootxl);

  auto sy = (rooty == 0) ? length : std::sinh(rootyl) / rooty;
  auto cy = std::cosh(rootyl);

  for (auto &p : bunch) {
    auto hbar = h / p.beta0;
    auto x = p.x;
    auto px = p.px;
    auto y = p.y;
    auto py = p.py;
    auto pz = p.pz;

    p.x = (x * cx
	   + px * sx
	   + pz * hbar * (1 - cx) / std::pow(rootx, 2));

    p.px = (x * -std::pow(rootx, 2) * sx
	    + px * cx
	    + pz * hbar * sx);

    p.y = y * cy + py * sy;
    p.py = (y * std::pow(rooty, 2) * sy
	    + py * cy);


    p.z + (x * -hbar * sx
	    + px * -hbar * (1 - cx) / std::pow(rootx, 2)
	    + pz * (length / std::pow(p.beta0 * p.gamma0, 2)
		    - std::pow(hbar, 2)
		    * (length - sx)
		    / std::pow(rootx, 2))
	   + z);
  }
}

void trk::maps::quadrupole(TRKBunch &bunch, double length, double k1) noexcept {

  // Calculate matrix terms
  auto rootk = std::sqrt(k1);
  auto rootkl = rootk * length;

  // Focusing 2x2 matrix terms
  auto f11 = std::cos(rootkl);
  auto f12 = std::sin(rootkl) / rootk;
  auto f21 = std::sin(rootkl) * -rootk;
  auto f22 = f11;

  // Defocusing 2x2 matrix terms
  auto df11 = std::cosh(rootkl);
  auto df12 = std::sinh(rootkl) / rootk;
  auto df21 = std::sinh(rootkl) * rootk;
  auto df22 = df11;

  for (auto &p : bunch) {
    auto x = p.x;
    auto px = p.px;
    auto y = p.y;
    auto py = p.py;

    p.x = x * f11 + px * f12;
    p.px += x * f21 + px * f22;

    p.y = y * df11 + py * df12;
    p.py += y * df21 + py * df22;

    p.z += p.pz * length / std::pow(p.beta0 * p.gamma0, 2);
  }
}

void trk::maps::sextupole(TRKParticle &p, double length, double k2) noexcept {
  p.px -= 0.5 * length * k2 * (std::pow(p.x, 2) - std::pow(p.y, 2));
  p.py += 0.5 * length * k2 * p.x * p.y;
}

void trk::maps::kicker(TRKParticle &p, double hkick, double vkick) noexcept {
  p.px += hkick;
  p.py += vkick;
}

// c.f. BDSIntegratorDipoleFringe
void trk::maps::dipole_fringe(TRKParticle &p, double k0,
                              double angle) noexcept {
  auto r12 = k0 * std::tan(angle);
  auto r34 = -k0 * std::tan(angle);

  p.px += p.x * r12;
  p.py += p.y * r34;
}

void trk::maps::dipole_fringe(TRKBunch &b, double k0, double angle) noexcept {
  for (auto &p : b) {
    dipole_fringe(p, k0, angle);
  }
}
