#ifndef TRKDEFAULTSTRATEGY
#define TRKDEFAULTSTRATEGY

#include "TRKStrategy.hh"

class TRKOctupole;
class TRKQuadrupole;
class TRKRBend;
class TRKSBend;
class TRKSextupole;
class TRKKicker;
class TRKSolenoid;
class TRKDipoleFringe;
typedef class TRKElement TRKDrift;

class TRKBunch;



class TRKDefaultStrategy : public TRKStrategy {
public:
  TRKDefaultStrategy() : TRKStrategy(0 /* Tracking steps */){};

  void Track(TRKDrift* el, TRKParticle& particle, double step) override;
  void Track(TRKSBend* el, TRKParticle& particle, double step) override;
  void Track(TRKRBend* el, TRKParticle& particle, double step) override;
  void Track(TRKDipoleFringe* el, TRKParticle& particle, double step) override;
  void Track(TRKQuadrupole* el, TRKParticle& particle, double step) override;
  void Track(TRKSextupole* el, TRKParticle& particle, double step) override;
  void Track(TRKOctupole* el, TRKParticle& particle, double step) override;
  void Track(TRKSolenoid* el, TRKParticle& particle, double step) override;
  void Track(TRKKicker* el, TRKParticle& particle, double step) override;

};

#endif
