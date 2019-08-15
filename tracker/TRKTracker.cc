/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#include <iostream>
#include <map>

#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutput.hh"

#include "parser/options.h"

#include "TRKBunch.hh"
#include "TRKElement.hh"
#include "TRKLine.hh"
#include "TRKParticle.hh"
#include "TRKStrategy.hh"
#include "TRKTracker.hh"

class BDSTrajectory;

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

TRKTracker::TRKTracker(TRKLine*       lineIn,
		       TRKStrategy*   strategyIn,
		       const GMAD::Options& options,
		       BDSOutput*           outputIn):
  line(lineIn), strategy(strategyIn), output(outputIn)
{
  dontuseaperture = options.dontUseAperture;
  maxTurns = std::abs(options.nturns);
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << "number of turns to take: " << maxTurns << std::endl;
#endif
}

TRKTracker::~TRKTracker()
{}

void TRKTracker::Track(TRKBunch* bunch) 
{
  std::cout << __METHOD_NAME__ << "starting tracking" << std::endl;
  
  Clock::time_point tStart = Clock::now(); 
  
  if (!bunch)
    {return;} //can't track nothing
  if (bunch->empty())
    {return;} //even if bunch exists, there must be particles in it
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " bunch before any tracking " << std::endl;
  std::cout << *bunch << std::endl;
#endif
  TRKLineConstIter elIter = line->begin();
  //iterate over number of turns - if linear, just 1 'turn'
  BDSGlobalConstants::Instance()->ResetTurnNumber(); //used in output data
  for (unsigned int i=0; i<maxTurns; i++) 
    {
      //check if all particles have been lost before turns completed
      if (bunch->empty())
	{break;}
      std::cout << "Turn Number: " << i << " / " << maxTurns << std::endl;
      //iterate over beamline elements
      for (elIter = line->begin();elIter!=line->end(); ++elIter) 
	{
	  //check if all particles have been lost after previous element
	  if (bunch->empty())
	    {break;}
	  
	  //calculate optical functions if required - TBC

	  //track all particles through an element
	  (*elIter)->Track(bunch,strategy);
	  
	  //check the aperture and decide whether to shift to bdsim
	  if (!dontuseaperture) //is aperture checking turned on?
	    {
	      TRKBunch* lostParticles = (*elIter)->CheckAperture(bunch);
	      if (lostParticles)
		{
		  int turnsTaken = BDSGlobalConstants::Instance()->TurnsTaken();
		  output->WriteTrackerPrimaryLoss(lostParticles, turnsTaken);
		  delete lostParticles;
		}
	    }
	  
	  //confirm coordinates for particles that didn't 'hit' aperture
	  (*elIter)->ConfirmCoordinates(bunch);
	}// end of beamline iteration
      
      BDSGlobalConstants::Instance()->IncrementTurnNumber(); //used in output data
      if (bunch->empty())
	{
	  std::cout << "No further particles to track" << std::endl;
	  break;
	}

      // finish an event in the output which is a turn here
      std::map<BDSTrajectory*, bool> trajectorymap;
      output->FillEvent(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			nullptr, trajectorymap, nullptr, nullptr, i);
    }// end of turns iteration
    
    
  Clock::time_point tEnd = Clock::now();
  milliseconds timeDiff = std::chrono::duration_cast<milliseconds>(tEnd-tStart);
  
  std::cout << "Time taken: " << timeDiff.count() << "ms" << std::endl;
    
  std::cout << "All turns completed" << std::endl
	    << "Thank you for using BDSIM Tracker" << std::endl;
}
