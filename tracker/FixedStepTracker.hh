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
#ifndef FIXEDSTEPTRACKER_H
#define FIXEDSTEPTRACKER_H

#include "Tracker.hh"

#include <memory>

#include "TRKBacktracker.hh"



class TRKBunch;
class TRKLine;
class TRKStrategy;


namespace GMAD
{
  class Options;
}

namespace trk {
class EventOutput;
  

class FixedStepTracker: public Tracker
{
public:
  FixedStepTracker(TRKLine*             line,
		   TRKStrategy*         strategy,
		   const GMAD::Options& options,
		   std::shared_ptr<EventOutput> outputIn);
  void Track(TRKBunch*) override;

private:
  TRKLine* beamline;
  TRKStrategy* strategy;
  int maxturns;
  bool useaperture;
  TRKBacktracker backtracker;
  std::shared_ptr<EventOutput> output;

};

}

#endif /* FIXEDSTEPTRACKER_H */
