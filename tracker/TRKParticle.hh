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
#ifndef TRKParticle_h
#define TRKParticle_h

#include <ostream>
#include "vector3.hh"
#include "vector6.hh"

/**
 * @brief Particle class has two 6 vectors for before and after stepping
 * pos/mom plus mass, momentum, charge. 3 vector momentum 'mom' is the 
 * differential momentum (x',y',z') and momentum is the total current
 * absolute momentum. Momentum is required for rigidity and therefore
 * tracking. Geant4 requires the kinetic energy of the particle as well
 * as the particle definition - the kinetic energy can be calculate at
 * output time / when information is passed to bdsim.
*/

class TRKParticle
{ 
public:
  //constructors
  TRKParticle() = delete;
  TRKParticle(double xIn, double pxIn, double yIn, double pyIn, double zIn,
              double pzIn, double beta0In, double gamma0In, double SIn,
              int eventidIn)
      : x(xIn), px(pxIn), y(yIn), py(pyIn), z(zIn), pz(pzIn), beta0(beta0In),
        gamma0(gamma0In), S(SIn), eventid(eventidIn), mass(938.27231) {}

  //accessors
 double getS() const { return S; }

  double Energy() const { return ReferenceMomentum() * (pz + 1/beta0); }
  double KineticEnergy() const { return Energy() - mass; }
  double Momentum() const { return std::sqrt(Energy()*Energy() - M() * M()); }
  double ReferenceMomentum() const
  {
    return mass * std::sqrt(gamma0*gamma0 - 1);
  }
  double Xp() const { return px * ReferenceMomentum() / Momentum(); }
  double Yp() const { return py * ReferenceMomentum() / Momentum(); }

  int EventID() const { return eventid; }

  /// return mass in MeV / c^2
  double M() const { return mass; }
  /// return elementary charge
  int Charge() const { return charge; }

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKParticle &part);

  double x;
  double px;
  double y;
  double py;
  double z;
  double pz;

  double beta0;
  double gamma0;


  // s-position of the particle, needed for arbitrary step-length tracking
  double S;

private:
  int eventid;
  /// mass in MeV / c^2
  double mass;
  /// charge in units of elementary charge
  int    charge;
};

#endif
