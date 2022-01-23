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
#include <cstdlib>
#include <string>
#include <iostream>
#include <utility>

#include "TRK.hh"
#include "TRKBunch.hh"
#include "TRKParticle.hh"
#include "TRKParticleDefinition.hh"

#include "BDSBunch.hh"
#include "BDSBunchFactory.hh"
#include "BDSDebug.hh"
#include "BDSParticleCoordsFullGlobal.hh"
#include "BDSParticleDefinition.hh"

#include "parser/beam.h"

#include "CLHEP/Units/SystemOfUnits.h"

TRKBunch::TRKBunch() : population(0) {}

TRKBunch::TRKBunch(const GMAD::Beam& beam,
                   BDSParticleDefinition* designParticle,
                   BDSParticleDefinition* beamParticle,
                   long int nGenerate):
  population(nGenerate)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << "Initialisation" << std::endl;
#endif
  //populate particles using options & random number generator
  Populate(beam, beamParticle, designParticle);
}

TRKBunch::TRKBunch(const std::vector<TRKParticle>& particleVectorIn)
{
  bunch = particleVectorIn;
}

TRKBunch::TRKBunch(std::vector<TRKParticle> &&particles)
    : bunch(std::move(particles)) {}

TRKBunch::~TRKBunch()
{;}

void TRKBunch::Populate(const GMAD::Beam& beam,
			BDSParticleDefinition* designParticle, BDSParticleDefinition* beamParticle)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << "Generating particles" << std::endl;
#endif
  //particle definition / generation
  //just now this allows every particle to have a different
  //mass and charge
  //if we assume all the same, then it could be done on a global
  //basis, which would save around 20% memory on each particle...

  // Initialise bunch
  BDSBunch* bdsbunch = BDSBunchFactory::CreateBunch(beamParticle, beam);

  //must have positive number of particles
  if (population < 0)
    {population = std::abs(population);}
  //must have at least 1 particle
  if (population == 0)
    {population = 1;}
  //initialise the vector of particles
  bunch.reserve(population);

  bdsbunch->BeginOfRunAction(population); // offsetSampleMean



  double p0 = designParticle->Momentum();
  double mass0 = designParticle->Mass();
  double charge0 = designParticle->Charge();
  double beta0 = designParticle->Beta();
  double E0 = designParticle->TotalEnergy();
  double S0 = 0.0;

  for (int i = 0; i < population; i++)
    {
      // bdsbunch generates values in CLHEP mm standard.
      BDSParticleCoordsFullGlobal c = bdsbunch->GetNextParticle();

      // Get the updated particle definition if that has changed
        BDSParticleDefinition const *beamParticle =
                bdsbunch->ParticleDefinition();

        mass   = beamParticle->Mass();
        charge = beamParticle->Charge();
        totalEnergy   = beamParticle->TotalEnergy();
        kineticEnergy = beamParticle->KineticEnergy();

      double energy = c.local.totalEnergy * (mass0 / mass); // This is also normalised
      double p = std::sqrt(energy*energy - mass*mass);

      // Momenta from BDSBunch are px/|p|, but we want px/p0
      // (i.e. normalised w.r.t reference momentum, not particle momentum).
      auto px = c.local.xp * (p / p0) * (mass0 / mass);
      auto py = c.local.yp * (p / p0) * (mass0 / mass);

      auto z = S0 - beta0 * c.local.T; // z = S0 - beta0 * c * t (the c is omitted for now).
      auto pz = ((mass / mass0) * energy - E0) / (beta0 * p0); // pz = (E*m0/m - E0)/(beta0*c*P) (the c is omitted)

      bunch.emplace_back(c.local.x, px, c.local.y, py,
			 z, pz,  // longitudinal coordinates
			 mass0, charge0,
			 S0, i, designParticle);

      //weight not required - maybe should be kept though to pass on to bdsim
    }
}

std::ostream& operator<< (std::ostream &out, const TRKBunch &beam)
{
  auto iter = beam.bunch.begin();
  auto end  = beam.bunch.end();

  for (;iter!=end;++iter)
    {out << *iter << std::endl;}
  return out;
}
