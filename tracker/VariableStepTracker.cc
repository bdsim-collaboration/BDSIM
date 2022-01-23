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
#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <random>
#include <map>
#include <memory>

#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"

#include "parser/options.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "TRKAperture.hh"
#include "TRKBacktracker.hh"
#include "TRKBunch.hh"
#include "TRKElement.hh"
#include "TRKLine.hh"
#include "TRKLineUtils.hh"
#include "EventOutput.hh"
#include "TRKParticle.hh"
#include "TRKStrategy.hh"
#include "VariableStepTracker.hh"

namespace trk {

VariableStepTracker::VariableStepTracker(TRKLine*       lineIn,
					 TRKStrategy*   strategyIn,
					 const GMAD::Options& options,
					 std::shared_ptr<EventOutput> outputIn):
  line(lineIn), strategy(strategyIn),
  maxTurns(options.nturns),
  useaperture(options.useAperture),
  backtracker(strategyIn,
	      options.backtracking,
	      options.lossPrecision * CLHEP::m),
  output(std::move(outputIn))
{
  if (maxTurns <= 0)
    { throw std::runtime_error("Must be a positive number of turns"); }
  if (trk::BeamlineLength(line) <= 0.0) {
    throw std::runtime_error(
        "Tracked line must have a positive, non-zero length.");
  }
}

double VariableStepTracker::RandomStep()
{
    ///double rn = (((double) rand() / (RAND_MAX))) * 0.1e3;
    double rn = 50;
    return rn;
}

void VariableStepTracker::Track(TRKBunch* bunch)
{
  if (!bunch)
    {throw std::runtime_error("No bunch has been provided.");}
  if (bunch->empty())
    {throw std::runtime_error("Bunch is empty.");}

    BDSGlobalConstants::Instance()->ResetTurnNumber(); //used in output data

    for (int turn = 0; turn < maxTurns; turn++) {
      output->currentTurn = turn;
      for (auto &p : *bunch) {
        auto eIt = line->cbegin();
        auto esIt = line->beginS();
        auto element = *eIt;
        auto SEnd = *esIt;

        if (turn == 0) {
          /// On the first turn, find the element where the particle starts
          /// off in
          eIt = line->FindElement(p.getS());
          std::advance(esIt, std::distance(line->cbegin(), eIt));
        }

        double proposedStep = RandomStep();
        double proposedEndPoint = p.getS() + proposedStep;
        bool advance = false;

        int count = 0;
        while (eIt != line->end() && esIt != line->endS()) {
          count++; // Maximum tries
          if (count > 1e6) {
            throw std::runtime_error(
                "Reached maximum number of tries in stepper");
          }

          if (advance) {
            element = *eIt++;
            SEnd = *esIt++;
            advance = false;
          }

          // Check if current step (maybe applied over multiple
          // elements) has finished being applied.
          if (fabs(proposedEndPoint - p.getS()) < proposedEndPoint * 1.E-6) {
            /// Step is completed, prepare a new step
            proposedStep = RandomStep();
            proposedEndPoint = p.getS() + proposedStep;
          }

          //  If current step goes beyond this element's boundary
          //  then reduce step to the distance to the boundary and
          //  mark advance to true to progress line iterators on next
          //  go of while loop.
          double step;
          if (proposedEndPoint > SEnd) {
            step = SEnd - p.getS();
            advance = true;
          } else // We can use the proposed step as the actual step.

          {
            step = proposedStep;
          }

          // Track the particle with this proposed step.
          element->Track(p, step, strategy);

          // Update the particle's s position in light of step taken.
          p.S += step;
          // Decrement the proposed step for some reason?
          proposedStep -= step;
        }
      }
      EndOfTurn(*bunch);
    }
}

void VariableStepTracker::EndOfTurn(TRKBunch& bunch)
{
  BDSGlobalConstants::Instance()->IncrementTurnNumber(); //used in output data
  ResetS(bunch);
}

void VariableStepTracker::ResetS(TRKBunch& bunch)
{
  for (auto &p: bunch)
    {
      p.S = 0.0;
    }
}

}
