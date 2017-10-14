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
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"

#include "parser/beam.h"

#include "CLHEP/Units/SystemOfUnits.h"

TRKBunch::TRKBunch(const GMAD::Beam& beam)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << "Initialisation" << std::endl;
#endif
  //calculate energy based on particle mass - must do before we populate
  TRK::CalculateKineticEnergy(beam);
  //populate particles using options & random number generator
  Populate(beam);
}

TRKBunch::TRKBunch(const std::vector<TRKParticle>& particleVectorIn)
{
  bunch = particleVectorIn;
}

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
  std::string particlename = std::string(beam.particleName);
  std::pair<double, int> pmc = TRKParticleDefinition::Instance()->GetParticleMassAndCharge(particlename);
  double mass = pmc.first*0.001; //mass converted from MeV to GeV manually without CLHEP
  int  charge = pmc.second;

  // Initialise bunch
  BDSBunch bdsbunch;

  // Get bunch type from gmad options for correct population
  bdsbunch.SetOptions(beam);


  // Update population according to changes in bunch type
  population = BDSGlobalConstants::Instance()->NGenerate();

  //must have positive number of particles
  if (population < 0){population = abs(population);}
    //must have at least 1 particle
    if (population == 0){population = 1;}
      //initialise the vector of particles
      bunch.reserve(population);


  //note in this tracker there are only local rectilinear coordinates
  //therefore 'z' is misleading and 's' should be used instead
  double x0=0.0, y0=0.0, s0=0.0, xp=0.0, yp=0.0, sp=0.0, t=0.0, E=0.0, weight=1.0;

  for (int i = 0; i < population; i++)
    {
      // bdsbunch generates values in CLHEP mm standard.
      bdsbunch.GetNextParticle(x0,y0,s0,xp,yp,sp,t,E,weight);

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


TRKBunch::~TRKBunch() {}


std::ostream& operator<< (std::ostream &out, const TRKBunch &beam)
{
  std::vector<TRKParticle>::const_iterator iter = beam.bunch.begin();
  std::vector<TRKParticle>::const_iterator end  = beam.bunch.end();

  for (;iter!=end;++iter)
    {out << *iter << std::endl;}
  return out;
}
