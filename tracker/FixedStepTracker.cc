/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <cstddef>

#include "CLHEP/Units/SystemOfUnits.h"

#include "parser/options.h"

#include "FixedStepTracker.hh"
#include "TRKBunch.hh"
#include "TRKElement.hh"
#include "TRKLine.hh"
#include "EventOutput.hh"

namespace trk {
FixedStepTracker::FixedStepTracker(TRKLine *line, TRKStrategy *strategyIn,
                                   const GMAD::Options &options,
                                   std::shared_ptr<EventOutput> outputIn)
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
    if (output)
      output->currentTurn = turn;
    for (auto it = beamline->cbegin(); it != beamline->cend(); ++it) {
      (*it)->Track(bunch, (*it)->GetLength(), strategy);
    }
  } // end of turns iteration
}
} // namespace trk


