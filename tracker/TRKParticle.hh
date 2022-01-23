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
#include <BDSParticleDefinition.hh>
#include "TRKReferenceParticle.hh"
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
              double pzIn, double massIn, int chargeIn, double SIn,
              int eventidIn, BDSParticleDefinition* referenceParticleIn)
      : x(xIn), px(pxIn), y(yIn), py(pyIn), z(zIn), pz(pzIn), S(SIn),
       referenceParticle(referenceParticleIn), mass(massIn), charge(chargeIn), eventid(eventidIn) {}

  //accessors
 double getS() const { return S; }

  double Energy() const
  {
      double beta0 = referenceParticle->Beta();
      double p0 = referenceParticle->Momentum();
      double E0 = referenceParticle->TotalEnergy();
      return pz * beta0 * p0 + E0;    /// TODO check this expression
  }

  double KineticEnergy() const
  { return Energy() - mass; }

  double Momentum() const
  { return std::sqrt(std::pow(Energy(), 2) - std::pow(M(), 2)); }


  double Delta() const
    { return (Momentum() - referenceParticle->Momentum())/ referenceParticle->Momentum(); }


    double Chi() const
    { return (Charge() * referenceParticle->Mass()) / (referenceParticle->Charge() * M()); }

  double Xp() const
  {
      double m0 = referenceParticle->Mass();
      return px * referenceParticle->Momentum() * (mass / m0) / Momentum();
  }

  double Yp() const
  {
      double m0 = referenceParticle->Mass();
      return py * referenceParticle->Momentum() * (mass / m0) / Momentum();
  }

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


  // s-position of the particle, needed for arbitrary step-length tracking
  double S;


  /// Keep a pointer to the reference particle definition in order to convert the normalised coordinates
  /// used for tracking to absolute quantities such as energy, momentum etc.
  BDSParticleDefinition* referenceParticle;

private:
    /// mass in MeV / c^2
    double mass;
    /// charge in units of elementary charge
    int    charge;

    int    eventid;


};

#endif
