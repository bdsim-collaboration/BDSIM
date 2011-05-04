//GammaConversion Leading Particle Biasing Method, M.D. Salt, R.B. Appleby, 15/10/09

#ifndef GAMMACONVERSION_LPB_HH
#define GAMMACONVERSION_LPB_HH

#include "G4WrapperProcess.hh"
#include "G4RunManager.hh"

class GammaConversion_LPB : public G4WrapperProcess {
  public:
    G4VParticleChange* PostStepDoIt(const G4Track& track, const G4Step& step);

  private:

};
#endif
