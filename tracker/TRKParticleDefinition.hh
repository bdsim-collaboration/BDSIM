#ifndef TRKPARTICLEDEFINITION_h
#define TRKPARTICLEDEFINITION_h 1

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <map>
#include <utility>
#include <string>

class TRKParticleDefinition {
public:
  ~TRKParticleDefinition();
  static TRKParticleDefinition* Instance();   ///access method for instance
  std::pair<double,int> GetParticleMassAndCharge(const std::string particlename);

protected:
  TRKParticleDefinition();
private:
  static TRKParticleDefinition* _instance; ///singleton patter
  std::map<std::string,std::pair<double, int> > particleMassCharge;
  std::map<std::string,std::pair<double, int> > PrepareDefinitions();
};

#endif
