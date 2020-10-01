#include <cmath>

#include "TRKBacktracker.hh"

#include "TRKAperture.hh"
#include "TRKElement.hh"
#include "TRKParticle.hh"

TRKBacktracker::TRKBacktracker(TRKStrategy *strategyIn,
			       bool backtrackingIn,
                               double lossPrecisionIn)
    : strategy(strategyIn), backtracking(backtrackingIn),
      lossPrecision(lossPrecisionIn) {}

BDSHitApertureImpact TRKBacktracker::GetApertureImpact(
    TRKParticle &lostParticle, TRKElement *element, double step) const
{
  BacktrackToLossPoint(lostParticle, element, step);
  return BDSHitApertureImpact(lostParticle.Energy(),
			      lostParticle.KineticEnergy(),
			      lostParticle.S,
			      lostParticle.x,
			      lostParticle.y,
			      lostParticle.px,
			      lostParticle.py,
			      1, // weight
			      -1, // time
			      2212, // partID, always a proton (for now)
			      0, // trackID
			      0, // parentID
			      0, // turn number, 0 for now
			      0, // beamlineIndex,
			      0 // nElectrons (???)
			      );
}

int TRKBacktracker::NBisectionSteps(double interval) const {
  return std::ceil(std::log2(lossPrecision / interval));
}

void TRKBacktracker::BacktrackToLossPoint(TRKParticle &lostParticle,
                                          TRKElement *element,
                                          double step) const
{
  auto maxsteps = NBisectionSteps(step);
  for (int i = step; i < maxsteps; ++i) {
    step /= 2.0;
    step = std::abs(step);
    if (element->GetAperture()->OutsideAperture(lostParticle)) {
      step *= -1;
    } // if outside then step back

    element->Track(lostParticle, step, strategy);
    lostParticle.S += step;
  }
}
