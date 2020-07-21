#include "TRKDefaultStrategy.hh"
#include "TRKBunch.hh"
#include "TRKElement.hh"
#include "TRKKicker.hh"
#include "TRKMaps.hh"
#include "TRKQuadrupole.hh"
#include "TRKSextupole.hh"
#include "TRKOctupole.hh"
#include "TRKSBend.hh"
#include "TRKRBend.hh"
#include "TRKSolenoid.hh"

#include <cmath>

void TRKDefaultStrategy::Track(TRKDrift *el, TRKBunch *bunch) {
  trk::maps::nldrift(*bunch, el->GetLength());
}

void TRKDefaultStrategy::Track(TRKSBend *el, TRKBunch *bunch) {
  trk::maps::sbend(*bunch, el->GetLength(), el->GetAngle(), el->GetK1());
}

void TRKDefaultStrategy::Track(TRKRBend *el, TRKBunch *bunch) {
  auto angle = el->GetAngle();
  auto k0 = el->GetStrength();
  auto poleface = angle / 2;
  trk::maps::dipole_fringe(*bunch, k0, poleface);
  trk::maps::sbend(*bunch, el->GetLength(), k0, el->GetK1());
  trk::maps::dipole_fringe(*bunch, k0, poleface);
}

void TRKDefaultStrategy::Track(TRKQuadrupole *el, TRKBunch *bunch) {
  trk::maps::quadrupole(*bunch, el->GetLength(), el->GetStrength());
}

void TRKDefaultStrategy::Track(TRKSextupole *el, TRKBunch *b) {
  for (auto &p : *b) {
    trk::maps::drift(p, el->GetLength() / 2);
    trk::maps::sextupole(p, el->GetLength(), el->GetStrength());
    trk::maps::drift(p, el->GetLength() / 2);
  }
}

void TRKDefaultStrategy::Track(TRKOctupole *el, TRKBunch *bunch) {
  trk::maps::drift(*bunch, el->GetLength());
}

void TRKDefaultStrategy::Track(TRKSolenoid *el, TRKBunch *bunch) {
  trk::maps::drift(*bunch, el->GetLength());
}

void TRKDefaultStrategy::Track(TRKKicker *el, TRKBunch *bunch) {
  auto hkick = el->GetHKick();
  auto vkick = el->GetVKick();

  for (auto &p : *bunch) {
    trk::maps::kicker(p, hkick/2., vkick/2.);
    trk::maps::drift(p, el->GetLength());
    trk::maps::kicker(p, hkick/2., vkick/2.);
  }
}
