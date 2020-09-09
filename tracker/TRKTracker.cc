/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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

#include "TRKAperture.hh"
#include "TRKBacktracker.hh"
#include "TRKBunch.hh"
#include "TRKElement.hh"
#include "TRKLine.hh"
#include "TRKOutput.hh"
#include "TRKParticle.hh"
#include "TRKStrategy.hh"
#include "TRKTracker.hh"

class BDSTrajectory;

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

TRKTracker::TRKTracker(TRKLine*       lineIn,
		       TRKStrategy*   strategyIn,
		       const GMAD::Options& options,
		       std::shared_ptr<TRKOutput> outputIn):
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
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << "number of turns to take: " << maxTurns << std::endl;
#endif
}

TRKTracker::~TRKTracker()
{}

double TRKTracker::RandomStep()
{
    ///double rn = (((double) rand() / (RAND_MAX))) * 0.1e3;
    double rn = 50;
    return rn;
}

void TRKTracker::Track(TRKBunch* bunch)
{
  if (!bunch)
    {throw std::runtime_error("No bunch has been provided.");}
  if (bunch->empty())
    {throw std::runtime_error("Bunch is empty.");}

    BDSGlobalConstants::Instance()->ResetTurnNumber(); //used in output data

    for (int turn = 0; turn < maxTurns; turn++)
      {
	output->currentTurn = turn;
	for (auto &p : *bunch)
	  {
	    auto eIt = line->cbegin();
	    auto esIt = line->beginS();
	    auto element = *eIt;
	    auto SEnd = *esIt;

	    if (turn == 0)
	      {
		/// On the first turn, find the element where the particle starts
		/// off in
		eIt = line->FindElement(p.getS());
		std::advance(esIt, std::distance(line->cbegin(), eIt));
	      }

	    double ds = (*eIt)->GetLength();
	    // double ds = RandomStep();
	    double endPoint = p.getS() + ds;
	    bool advance = false;

	    int count = 0;
	    while (eIt != line->end() && esIt != line->endS())
	      {
		count++; // Maximum tries
		if (count > 1e6)
		  {
		    throw std::runtime_error(
                      "Reached maximum number of tries in stepper");
		  }

                if (advance)
		  {
		    element = *eIt++;
		    SEnd = *esIt++;
		    advance = false;
		  }

		if (fabs(endPoint - p.getS()) < endPoint * 1.E-6)
		  {
		    /// Step is completed, prepare a new step
		    ds = RandomStep();
		    endPoint = p.getS() + ds;
		  }

		//  If current step goes beyond this element's boundary
		//  then reduce step to the distance to the boundary and
		//  mark advance to true to progress line iterators on next
		//  go of while loop
		double step;
		if (endPoint > SEnd)
		  {
		    step = SEnd - p.getS();
		    advance = true;
		  } else // We can use the proposed step as the actual step.

		  {
		    step = ds;
		  }

		// Track the particle with this proposed step.
		element->Track(p, step, strategy);

		// Update the particle's s position in light of step taken.
		p.S += step;
		// Decrement the proposed step for some reason?
		ds -= step;
	      }
	  }

	EndOfTurn(*bunch);
      }
}

void TRKTracker::EndOfTurn(TRKBunch& bunch)
{
  BDSGlobalConstants::Instance()->IncrementTurnNumber(); //used in output data
  ResetS(bunch);
}

void TRKTracker::ResetS(TRKBunch& bunch)
{
  for (auto &p: bunch)
    {
      p.S = 0.0;
    }
}
