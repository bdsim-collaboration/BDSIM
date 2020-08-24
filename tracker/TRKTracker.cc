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
  useaperture = options.useAperture;
  maxTurns = std::abs(options.nturns);
  backtracking = options.backtracking;
  lossPrecision = options.lossPrecision * CLHEP::m;
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

int TRKTracker::NBisectionSteps(double interval) const
{
  return std::ceil(std::log2(lossPrecision / interval));
}
