#ifndef TRKPARTICLEDEFINITION_h
#define TRKPARTICLEDEFINITION_h 1

#include <map>
#include <string>

/**
 * @brief class that holds constants related to elementary particles
 */

class TRKParticleDefinition {
public:
  ~TRKParticleDefinition();
  static TRKParticleDefinition* Instance();   ///> access method for instance
  std::pair<double,int> GetParticleMassAndCharge(const std::string particlename);

protected:
  TRKParticleDefinition();
private:
  static TRKParticleDefinition* _instance; ///> singleton pattern
  /// map of particle name and mass in MeV and elementary charge
  std::map<std::string,std::pair<double, int> > particleMassCharge;
  void PrepareDefinitions();
};

#endif
