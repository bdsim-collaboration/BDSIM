#include <iostream>
#include "stdlib.h"

#include "BDSDebug.hh"
#include "parser/options.h"

#include "TRKTracker.hh"

#include "TRKBunch.hh"
#include "TRKLine.hh"
#include "TRKStrategy.hh"
#include "TRKParticle.hh"
#include "TRKElement.hh"

TRKTracker::TRKTracker(TRKLine* lineIn, TRKStrategy* strategyIn, Options& options):line(lineIn),strategy(strategyIn)
{
  dontuseaperture = options.dontUseAperture;
  maxTurns = std::abs(options.nturns);
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << "number of turns to take: " << maxTurns << std::endl;
#endif
}

TRKTracker::~TRKTracker() {
}

void TRKTracker::Track(TRKBunch* bunch) 
{
  std::cout << __METHOD_NAME__ << "starting tracking" << std::endl;
  
  if (!bunch) return; //can't track nothing
  TRKLineConstIter elIter = line->begin();
  //iterate over number of turns - if linear, just 1 'turn'
  for (unsigned int i=0; i<maxTurns; i++) 
    {
      std::cout << "Turn Number: " << i << " / " << maxTurns << std::endl;
      //iterate over beamline elements
      for (;elIter!=line->end(); ++elIter) 
	{
	  //track all particles through an element
	  (*elIter)->Track(bunch,strategy);
	  
	  //check the aperture and decide whether to shift to bdsim
	  if (!dontuseaperture) //is aperture checking turned on?
	    {(*elIter)->CheckAperture(bunch);}
	  if (bunch->size() < 1) break;
	}// end of beamline iteration
      if (bunch->size() < 1) {std::cout << "No further particles to track" << std::endl;break;}
    }// end of turns iteration
  std::cout << "All turns completed" << std::endl
	    << "Thank you for using BDSIM Tracker" << std::endl;
}
