
#include "TRKDefaultStrategy.hh"
#include "TRKBunch.hh"
#include "TRKElement.hh"
#include "TRKQuadrupole.hh"
#include "TRKSBend.hh"

#include <cmath>

void TRKDefaultStrategy::Track(TRKDrift* el, TRKBunch* bunch) {
  double length = el->GetLength();

  for (auto &p : *bunch)
    {
      p.x += p.xp * length;
      p.y += p.yp * length;
      p.ct += p.dp * length / std::pow(p.beta0*p.gamma0, 2);
    }

}

void TRKDefaultStrategy::Track(TRKRBend *el, TRKBunch *bunch) {}

void TRKDefaultStrategy::Track(TRKSBend *el, TRKBunch *bunch) {}

void TRKDefaultStrategy::Track(TRKDipole *el, TRKBunch *bunch) {}

void TRKDefaultStrategy::Track(TRKQuadrupole *el, TRKBunch *bunch) {}

void TRKDefaultStrategy::Track(TRKSextupole *el, TRKBunch *bunch) {}

void TRKDefaultStrategy::Track(TRKOctupole *el, TRKBunch *bunch) {}

void TRKDefaultStrategy::Track(TRKSolenoid *el, TRKBunch *bunch) {}
