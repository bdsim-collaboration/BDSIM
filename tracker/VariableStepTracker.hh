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
#ifndef VARIABLESTEPTRACKER_H
#define VARIABLESTEPTRACKER_H

#include <memory>

#include "TRKBacktracker.hh"
#include "Tracker.hh"

class EventOutput;

class TRKBunch;
class TRKLine;
class TRKStrategy;

namespace GMAD {
class Options;
}

/**
 * @brief class that takes care of the tracker
 */

namespace trk {

class VariableStepTracker : public Tracker {
public:
  VariableStepTracker(TRKLine *line, TRKStrategy *strategy,
                      const GMAD::Options &options,
                      std::shared_ptr<EventOutput> outputIn);
  /// track bunch through beamline - main tracking loop
  void Track(TRKBunch *bunch);

  /// Generate a random step length
  double RandomStep();

private:
  TRKLine *line;
  /// tracking type
  TRKStrategy *strategy;
  int maxTurns;
  bool useaperture;
  TRKBacktracker backtracker;
  std::shared_ptr<EventOutput> output;

  void EndOfTurn(TRKBunch &);
  void ResetS(TRKBunch &);
};

} // namespace trk
#endif
