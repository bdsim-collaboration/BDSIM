#include <cstddef>

#include "CLHEP/Units/SystemOfUnits.h"

#include "parser/options.h"

#include "FixedStepTracker.hh"
#include "TRKBunch.hh"
#include "TRKElement.hh"
#include "TRKLine.hh"
#include "TRKOutput.hh"

namespace trk {
FixedStepTracker::FixedStepTracker(TRKLine *line, TRKStrategy *strategyIn,
                                   const GMAD::Options &options,
                                   std::shared_ptr<TRKOutput> outputIn)
    : beamline(line), strategy(strategyIn), maxturns(options.nturns),
      useaperture(options.useAperture),
      backtracker(strategyIn, options.backtracking,
                  options.lossPrecision * CLHEP::m),
      output(std::move(outputIn)) {}

void FixedStepTracker::Track(TRKBunch *bunch) {
  if (!bunch) {
    throw std::runtime_error("No bunch has been provided.");
  }
  if (bunch->empty()) {
    throw std::runtime_error("Bunch is empty.");
  }

  for (int turn = 0; turn < maxturns; ++turn) {
    output->currentTurn = turn;
    for (auto it = beamline->cbegin(); it != beamline->cend(); ++it) {
      (*it)->Track(bunch, (*it)->GetLength(), strategy);
    }
  } // end of turns iteration
}
} // namespace trk


