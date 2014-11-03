#include <cstdlib>
#include <iostream>

#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
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
  if (options.circular)
    { maxTurns = std::abs(options.nturns);}
  else 
    { maxTurns = 1;}
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
  if (bunch->empty()) return; //even if bunch exists, there must be particles in it
  TRKLineConstIter elIter = line->begin();
  //iterate over number of turns - if linear, just 1 'turn'
  BDSGlobalConstants::Instance()->ResetTurnNumber(); //used in output data
  for (unsigned int i=0; i<maxTurns; i++) 
    {
      std::cout << "Turn Number: " << i << " / " << maxTurns << std::endl;
      //iterate over beamline elements
      for (elIter = line->begin();elIter!=line->end(); ++elIter) 
	{
	  //calculate optical functions if required

	  //track all particles through an element
	  (*elIter)->Track(bunch,strategy);
	  
	  //check the aperture and decide whether to shift to bdsim
	  if (!dontuseaperture) //is aperture checking turned on?
	    {(*elIter)->CheckAperture(bunch);}
	  
	  //confirm coordinates for particles that didn't 'hit' aperture
	  (*elIter)->ConfirmCoordinates(bunch);
	}// end of beamline iteration
      BDSGlobalConstants::Instance()->IncrementTurnNumber(); //used in output data
      if (bunch->empty()) {std::cout << "No further particles to track" << std::endl;break;}
    }// end of turns iteration
  std::cout << "All turns completed" << std::endl
	    << "Thank you for using BDSIM Tracker" << std::endl;
}
