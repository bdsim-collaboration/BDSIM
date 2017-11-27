/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#include "TRKPhysicsCalculations.hh"

#include "BDSBunch.hh"
#include "BDSBunchFactory.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParticleDefinition.hh"

#include "parser/beam.h"

#include "CLHEP/Units/SystemOfUnits.h"

TRKBunch::TRKBunch(const GMAD::Beam& beam,
		   BDSParticleDefinition* particle)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << "Initialisation" << std::endl;
#endif
  mass   = particle->Mass()*0.001; //mass converted from MeV to GeV manually without CLHEP
  charge = particle->Charge();
  totalEnergy   = particle->TotalEnergy();
  kineticEnergy = particle->KineticEnergy();
  
  //populate particles using options & random number generator
  Populate(beam);
}

TRKBunch::TRKBunch(const std::vector<TRKParticle>& particleVectorIn)
{
  bunch = particleVectorIn;
}

TRKBunch::~TRKBunch()
{;}

void TRKBunch::Populate(const GMAD::Beam& beam)
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
  BDSBunch* bdsbunch = BDSBunchFactory::CreateBunch(beam);

  // Update population according to changes in bunch type
  population = BDSGlobalConstants::Instance()->NGenerate();

  //must have positive number of particles
  if (population < 0)
    {population = abs(population);}
  //must have at least 1 particle
  if (population == 0)
    {population = 1;}
  //initialise the vector of particles
  bunch.reserve(population);

  //note in this tracker there are only local rectilinear coordinates
  //therefore 'z' is misleading and 's' should be used instead
  double x0=0.0, y0=0.0, s0=0.0, xp=0.0, yp=0.0, sp=0.0, t=0.0, E=0.0, weight=1.0;

  for (int i = 0; i < population; i++)
    {
      // bdsbunch generates values in CLHEP mm standard.
      bdsbunch->GetNextParticle(x0,y0,s0,xp,yp,sp,t,E,weight);

      double energy = E/CLHEP::GeV;
      //momentum back calculated from kinetic energy - can't change bds bunch
      // p = sqrt( (E + m)^2 - m^2 )
      double p = sqrt( energy*energy + 2*energy*mass );

      double paramsIn[8] = {x0/CLHEP::um,y0/CLHEP::um,s0/CLHEP::um,
			    xp/CLHEP::rad,yp/CLHEP::rad,sp/CLHEP::rad,
			    p,mass};
      bunch.push_back(TRKParticle(paramsIn, charge, i));

      //weight not required - maybe should be kept though to pass on to bdsim
    }
}

std::ostream& operator<< (std::ostream &out, const TRKBunch &beam)
{
  std::vector<TRKParticle>::const_iterator iter = beam.bunch.begin();
  std::vector<TRKParticle>::const_iterator end  = beam.bunch.end();

  for (;iter!=end;++iter)
    {out << *iter << std::endl;}
  return out;
}
