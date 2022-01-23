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
#include <algorithm>
#include <cmath>
#include <iostream>

#include "TRKBunch.hh"
#include "TRKMaps.hh"
#include "TRKParticle.hh"

void trk::maps::drift(TRKParticle &p, double length) noexcept {
    double delta = p.Delta();

  // beta0/beta = E * P0 * c / (P * c * E ) is the SixTrack defition for the beta ratio, the c is omitted
  double betaRatio = p.Energy() * p.referenceParticle->Momentum() / (p.referenceParticle->TotalEnergy() * p.Momentum());

  double nlterm = std::sqrt(std::pow(1 + delta, 2) - std::pow(p.px, 2) - std::pow(p.py, 2));

  p.x += p.px * length / nlterm;
  p.y += p.py * length / nlterm;
  p.z += (1 - betaRatio * (1 + delta) / nlterm) * length;

}

void trk::maps::drift(TRKBunch &bunch, double length) noexcept {
  for (auto &p : bunch) {
    drift(p, length);
  }
}

void trk::maps::sbend(TRKParticle &particle, double length, double angle,
                      double k1) noexcept {

    if (length == 0){
        return;
    }
    double rho = length / angle;

    double k0 = 1. / rho;
    double h = k0;

    double chi = particle.Chi();
    double delta = particle.Delta();

    double omegax = std::sqrt(chi * h * k0 / (1 + delta));
    double sx = std::sin(omegax * length);
    double cx = std::cos(omegax * length);
    double qx = (1 + delta) / (k0 * chi) - 1 / h;

    double betaRatio = (particle.Energy() * particle.referenceParticle->Momentum() /
            (particle.referenceParticle->TotalEnergy() * particle.Momentum()));

    double x = particle.x;
    double px = particle.px;
    double y = particle.y;
    double py = particle.py;
    double z = particle.z;

    particle.x = x * cx + px * sx / (omegax * (1 + delta)) + qx * (1 - cx);

    particle.px = - x * omegax * (1 + delta) * sx + px*cx + (1 + delta) * qx * omegax * sx;

    particle.y = y + py * length / (1 + delta);

    particle.py = py;

    particle.z = z + length * (1 - betaRatio)
            - betaRatio * (h * sx * x / omegax + (1 - cx) * px / h + delta*(2 * length - sx / omegax))
            - 0.25 * betaRatio *
            (omegax * (length - sx * cx / omegax) * std::pow(x - delta/h, 2) +
            (length + sx * cx/omegax) * std::pow(px, 2) / std::pow(1 + delta, 2)
            - (x - delta/h)* 2 * std::pow(sx, 2) * px / (1 + delta))
            + length * std::pow(py, 2) / std::pow(1 + delta, 2);
}

void trk::maps::sbend(TRKBunch &bunch, double length, double angle,
                      double k1) noexcept {
  for (auto &p : bunch) {
      sbend(p, length, angle, k1);
  }
}

void trk::maps::quadrupole(TRKParticle &particle, double length, double k1) noexcept {
    // Calculate matrix terms
    if (k1 == 0) {
        trk::maps::drift(particle, length);
        return;
    }

    bool xdefocusing = k1 < 0;

    double delta = particle.Delta();
    double oneplusdelta = 1 + delta;
    double chi = particle.Chi();
    double betaRatio = (particle.Energy() * particle.referenceParticle->Momentum() /
                        (particle.referenceParticle->TotalEnergy() * particle.Momentum()));

    double omegasquare = std::abs(chi * k1) * 1. / oneplusdelta;

    double rootk = std::sqrt(omegasquare);
    double rootkl = rootk * length;

    // Focusing 2x2 matrix terms
    double f11 = std::cos(rootkl);
    double f12 = std::sin(rootkl) / rootk;
    double f21 = std::sin(rootkl) * -rootk;
    double f22 = f11;

    // Defocusing 2x2 matrix terms
    double df11 = std::cosh(rootkl);
    double df12 = std::sinh(rootkl) / rootk;
    double df21 = std::sinh(rootkl) * rootk;
    double df22 = df11;

    if (xdefocusing) {
        std::swap(f11, df11);
        std::swap(f12, df12);
        std::swap(f21, df21);
        std::swap(f22, df22);
    }

    double x = particle.x;
    double px = particle.px;
    double y = particle.y;
    double py = particle.py;
    double z = particle.z;

    particle.x = x * f11 + px * f12 / oneplusdelta;
    particle.px = x * f21 * oneplusdelta + px * f22;

    particle.y = y * df11 + py * df12 / oneplusdelta;
    particle.py = y * df21 * oneplusdelta + py * df22;

    particle.z = z + length*(1 - betaRatio) - omegasquare *
            0.25 * betaRatio *
            ((length - f11 * f12)*std::pow(x, 2) - (length - df11*df12)*std::pow(y ,2)) -
            0.25 * betaRatio *
            ( (length + f11*f12) * std::pow(px/oneplusdelta, 2) + (length + df11*df12)*std::pow(py/oneplusdelta, 2)) -
            0.5 * betaRatio *
            (-x*px*std::pow(f12*rootk, 2)/oneplusdelta + y*py*std::pow(df12*rootk,2)/oneplusdelta);
}

void trk::maps::quadrupole(TRKBunch &bunch, double length, double k1) noexcept {
    for (auto &p : bunch) {
            sbend(p, length, k1);
        }
    }

void trk::maps::sextupole(TRKParticle &p, double length, double k2) noexcept {
    double chi = p.Chi();
  p.px -= 0.5 * length * chi * k2 * (std::pow(p.x, 2) - std::pow(p.y, 2));
  p.py += length * chi * k2 * p.x * p.y;
}

void trk::maps::kicker(TRKParticle &p, double hkick, double vkick) noexcept {
    double chi = p.Chi();
    double oneplusdelta = 1 + p.Delta();
  p.px += hkick * chi / oneplusdelta;
  p.py += vkick * chi / oneplusdelta;
}

// c.f. BDSIntegratorDipoleFringe
void trk::maps::dipole_fringe(TRKParticle &p, double k0,
                              double angle) noexcept {

  double chi = p.Chi();
  double oneplusdelta = 1 + p.Delta();

  p.px += p.x * k0 * chi * k0 * oneplusdelta * std::tan(angle);
  p.py -= p.y * k0 * chi * k0 * oneplusdelta * std::tan(angle);
}

void trk::maps::dipole_fringe(TRKBunch &b, double k0, double angle) noexcept {
  for (auto &p : b) {
    dipole_fringe(p, k0, angle);
  }
}
