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

void TRKDefaultStrategy::Track(TRKDrift* el, TRKParticle* particle, double step) {
  trk::maps::nldrift(*particle, step);
}

void TRKDefaultStrategy::Track(TRKSBend* el, TRKParticle* particle, double step) {
  trk::maps::sbend(*particle, step, el->GetAngle() * step / el->GetLength(), el->GetK1());
}

void TRKDefaultStrategy::Track(TRKRBend* el, TRKParticle* particle, double step) {
  auto angle = el->GetAngle();
  double length = el->GetLength();
  double k0 = angle / length;

  auto poleface = angle / 2;
  trk::maps::dipole_fringe(*particle, k0, poleface);
  trk::maps::sbend(*particle, step, k0, el->GetK1());
  trk::maps::dipole_fringe(*particle, k0, poleface);
}

void TRKDefaultStrategy::Track(TRKQuadrupole* el, TRKParticle* particle, double step) {
  trk::maps::quadrupole(*particle, step, el->GetStrength());
}

void TRKDefaultStrategy::Track(TRKSextupole* el, TRKParticle* particle, double step) {
    trk::maps::drift(*particle, step / 2);
    trk::maps::sextupole(*particle, step, el->GetStrength());
    trk::maps::drift(*particle, step / 2);
}

void TRKDefaultStrategy::Track(TRKOctupole *el, TRKParticle* particle, double step) {
  trk::maps::drift(*particle, step);
}

void TRKDefaultStrategy::Track(TRKSolenoid *el, TRKParticle* particle, double step) {
  trk::maps::drift(*particle, step);
}

void TRKDefaultStrategy::Track(TRKKicker *el, TRKParticle* particle, double step) {
  auto hkick = el->GetHKick() * step/el->GetLength();
  auto vkick = el->GetVKick() * step/el->GetLength();
  trk::maps::kicker(*particle, hkick/2., vkick/2.);
  trk::maps::drift(*particle, step);
  trk::maps::kicker(*particle, hkick/2., vkick/2.);
}
