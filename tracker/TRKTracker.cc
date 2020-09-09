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

    for (unsigned int i = 0; i < maxTurns; i++)
    {
        for (auto &p : *bunch)
        {
            auto eIt = line->begin();
            auto esIt = line->beginS();
            auto element = *eIt;
            auto SEnd = *esIt;

            if (i == 0)
            {
                /// On the first turn, find the element where the particle starts off in
                auto startElement = line->FindElement(p.getS());
                while(*eIt != startElement)
                {
                    eIt++;
                    esIt++;
                }
            }

            double ds = RandomStep();
            double endPoint = p.getS() + ds;
            bool advance = false;

            int count = 0;
            while (eIt != line->end() && esIt != line->endS())
            {
                count++;
                if (count > 1e6) {break;}

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

                double step;
                if (endPoint > SEnd)
                {
                    step = SEnd - p.getS();
                    advance = TRUE;
//                    std::cout << "Step limited!" << std::endl;
                }
                else
                {
                    step = ds;
                }

                element->Track(p, step, strategy);

//                std::cout << "Particle s (before): " << p.S << std::endl;
//                std::cout << "Step: " << step << std::endl;

                p.S += step;
                ds -= step;

//                std::cout << "Particle s (after): " << p.S << std::endl;
//                std::cout << "End point: "<< endPoint << "\n" << std::endl;

            }
        }

        BDSGlobalConstants::Instance()->IncrementTurnNumber(); //used in output data
        if (bunch->empty())
        {
            std::cout << "No further particles to track" << std::endl;
            break;
        }

        // finish an event in the output which is a turn here
        const std::map<G4String, G4THitsMap<G4double>*> scorerhits;
        output->FillEvent(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                          nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                          nullptr, nullptr, nullptr, nullptr, scorerhits, i);

    }
}

/*void TRKTracker::Track(TRKBunch* bunch)
{
  
  if (!bunch)
    {return;} // can't track nothing
  if (bunch->empty())
    {return;} // even if bunch exists, there must be particles in it

  TRKLineConstIter elIter = line->begin();
  // iterate over number of turns - if linear, just 1 'turn'
  BDSGlobalConstants::Instance()->ResetTurnNumber(); //used in output data
  for (unsigned int i = 0; i < maxTurns; i++) 
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
	  
	  // //check the aperture and decide whether to shift to bdsim
	  // if (!dontuseaperture) //is aperture checking turned on?
	  //   {
	  //     TRKBunch* lostParticles = (*elIter)->CheckAperture(bunch);
	  //     if (lostParticles)
	  // 	{
	  // 	  int turnsTaken = BDSGlobalConstants::Instance()->TurnsTaken();
	  // 	  output->WriteTrackerPrimaryLoss(lostParticles, turnsTaken);
	  // 	  delete lostParticles;
	  // 	}
	  //   }
	  
	  //confirm coordinates for particles that didn't 'hit' aperture
	  // (*elIter)->ConfirmCoordinates(bunch);
	} // end of beamline iteration
      
      BDSGlobalConstants::Instance()->IncrementTurnNumber(); //used in output data
      if (bunch->empty())
	{
	  std::cout << "No further particles to track" << std::endl;
	  break;
	}

      // finish an event in the output which is a turn here
      const std::map<G4String, G4THitsMap<G4double>*> scorerhits;
      output->FillEvent(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr, nullptr, scorerhits, i);
    }// end of turns iteration
    
}*/
