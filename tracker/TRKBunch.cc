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
#include "BDSOutputBase.hh"
#include "parser/options.h"

#include "BDSDebug.hh"

extern BDSOutputBase* trkOutput;

TRKBunch::TRKBunch(struct Options& opt)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << "Initialisation" << std::endl;
#endif
  population= opt.numberToGenerate;
  //must have positive number of particles
  if (population < 0){population = abs(population);}
  //must have at least 1 particle
  if (population == 0){population = 1;}
  //initialise the vector of particles
  bunch.reserve(population);
  //calculate energy based on particle mass - must do before we populate
  TRK::CalculateKineticEnergy(options);
  //populate particles using options & random number generator
  Populate(opt);
  //write primaries to output file
  trkOutput->WriteTrackerBunch("primaries",this,true);
}


TRKBunch::TRKBunch(const std::vector<TRKParticle>& particleVectorIn)
{
  bunch = particleVectorIn;
}

void TRKBunch::Populate(struct Options& opt)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << "Generating particles" << std::endl;
#endif
  //particle definition / generation
  //just now this allows every particle to have a different
  //mass and charge
  //if we assume all the same, then it could be done on a global
  //basis, which would save around 20% memory on each particle...
  std::string particlename = std::string(opt.particleName);
  std::pair<double, int> pmc = TRKParticleDefinition::Instance()->GetParticleMassAndCharge(particlename);
  double mass = pmc.first;
  int  charge = pmc.second;

  BDSBunch bdsbunch;
  bdsbunch.SetOptions(opt);
  //note in this tracker there are only local rectilinear coordinates
  //therefore 'z' is misleading and 's' should be used instead
  double x0=0.0, y0=0.0, s0=0.0, xp=0.0, yp=0.0, sp=0.0, t=0.0, E=0.0, weight=1.0;
  for (int i = 0; i < population; i++)
    {
      bdsbunch.GetNextParticle(x0,y0,s0,xp,yp,sp,t,E,weight);
      double paramsIn[8] = {x0/CLHEP::m,y0/CLHEP::m,s0/CLHEP::m,xp/CLHEP::rad,yp/CLHEP::rad,sp/CLHEP::rad,E/CLHEP::GeV,mass};
      bunch.push_back(TRKParticle(paramsIn, charge));
      //need to finish / write particle...
      //weight not required
    }
}


TRKBunch::~TRKBunch() {}


std::ostream& operator<< (std::ostream &out, const TRKBunch &beam)
{
  std::vector<TRKParticle>::const_iterator iter = beam.bunch.begin();
  std::vector<TRKParticle>::const_iterator end  = beam.bunch.end();
  
  for (;iter!=end;++iter) {
    out << *iter << std::endl;
  }
  return out;
}
