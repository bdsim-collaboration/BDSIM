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
#include <memory>

#include "parser/options.h"

#include "FixedStepTracker.hh"
#include "TrackerUtils.hh"
#include "VariableStepTracker.hh"

namespace trk {
std::unique_ptr<Tracker> CreateTracker(TRKLine *line, TRKStrategy *strat,
                                       const GMAD::Options &options,
                                       std::shared_ptr<EventOutput> output) {
  static std::string fixed("fixstep");
  static std::string variable("varstep");
  auto tracker = options.tracker;
  if (tracker == fixed) {
    return std::unique_ptr<FixedStepTracker>(
        new FixedStepTracker(line, strat, options, output));
  } else if (tracker == variable) {
    return std::unique_ptr<VariableStepTracker>(
        new VariableStepTracker(line, strat, options, output));
  } else {
    throw std::runtime_error(std::string("Unknown tracker type: ") + tracker);
  }
}

} // namespace trk
