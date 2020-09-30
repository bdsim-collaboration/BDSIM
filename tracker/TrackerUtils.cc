#include <memory>

#include "parser/options.h"

#include "FixedStepTracker.hh"
#include "TrackerUtils.hh"
#include "VariableStepTracker.hh"

namespace trk {
std::unique_ptr<Tracker> CreateTracker(TRKLine *line, TRKStrategy *strat,
                                       const GMAD::Options &options,
                                       std::shared_ptr<TRKOutput> output) {
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
    throw std::runtime_error(std::string("Unknown tracker type") + tracker);
  }
}

} // namespace trk
