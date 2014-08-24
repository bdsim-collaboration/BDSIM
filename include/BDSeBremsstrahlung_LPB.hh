//eBremsstrahlung Leading Particle Biasing Method, M.D. Salt, R.B. Appleby, 15/10/09

#ifndef EBREMSSTRAHLUNG_LPB_HH
#define EBREMSSTRAHLUNG_LPB_HH

#include "G4WrapperProcess.hh"

class eBremsstrahlung_LPB : public G4WrapperProcess {
  public:
    G4VParticleChange* PostStepDoIt(const G4Track& track, const G4Step& step);

  private:

};

class eBremsstrahlung_LPB_v2 : public G4WrapperProcess {
  public:
    G4VParticleChange* PostStepDoIt(const G4Track& track, const G4Step& step);

  private:

};
#endif
