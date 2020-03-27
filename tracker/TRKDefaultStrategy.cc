
#include "TRKDefaultStrategy.hh"
#include "TRKBunch.hh"
#include "TRKElement.hh"
#include "TRKQuadrupole.hh"
#include "TRKSBend.hh"

// void TRKDefaultStrategy::Track(


void TRKDefaultStrategy::Track(TRKDrift* el, TRKBunch* bunch) {
  double length = el->GetLength();
  std::cout << "DRIFT LENGTH = " << length << "\n";


}

void TRKDefaultStrategy::Track(TRKRBend *el, TRKBunch *bunch) {}

void TRKDefaultStrategy::Track(TRKSBend *el, TRKBunch *bunch) {}

void TRKDefaultStrategy::Track(TRKDipole *el, TRKBunch *bunch) {}

void TRKDefaultStrategy::Track(TRKQuadrupole *el, TRKBunch *bunch) {}

void TRKDefaultStrategy::Track(TRKSextupole *el, TRKBunch *bunch) {}

void TRKDefaultStrategy::Track(TRKOctupole *el, TRKBunch *bunch) {}

void TRKDefaultStrategy::Track(TRKSolenoid *el, TRKBunch *bunch) {}
