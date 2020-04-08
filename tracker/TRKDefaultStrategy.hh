#ifndef TRKDEFAULTSTRATEGY
#define TRKDEFAULTSTRATEGY

#include "TRKStrategy.hh"

class TRKOctupole;
class TRKQuadrupole;
class TRKRBend;
class TRKSBend;
class TRKSextupole;
class TRKSolenoid;
typedef class TRKElement TRKDrift;

class TRKBunch;


class TRKDefaultStrategy : public TRKStrategy {
public:
  TRKDefaultStrategy() : TRKStrategy(0 /* Tracking steps */){};

  void Track(TRKDrift* el, TRKBunch* bunch) override;
  void Track(TRKRBend* el, TRKBunch* bunch) override;
  void Track(TRKSBend* el, TRKBunch* bunch) override;
  void Track(TRKQuadrupole* el, TRKBunch* bunch) override;
  void Track(TRKSextupole* el, TRKBunch* bunch) override;
  void Track(TRKOctupole* el, TRKBunch* bunch) override;
  void Track(TRKSolenoid* el, TRKBunch* bunch) override;

};

#endif
