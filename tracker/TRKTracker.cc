#include <iostream>

#include "BDSDebug.hh"
#include "parser/options.h"

#include <iterator> // std::back_inserter
#include <algorithm> // std::copy_if erase_if remove_if
#include <vector>
#include <functional>

#include "TRKTracker.hh"

#include "TRKBunch.hh"
#include "TRKLine.hh"
#include "TRKStrategy.hh"
#include "TRKParticle.hh"
#include "TRKElement.hh"

TRKTracker::TRKTracker(TRKLine* lineIn, TRKStrategy* strategyIn, Options& options):line(lineIn),strategy(strategyIn)
{
  dontuseaperture = options.dontUseAperture;
  maxTurns = (line->GetCircular() ? TRK::NR_TURNS : 1);
}

TRKTracker::~TRKTracker() {
}

void TRKTracker::Track(TRKBunch* bunch) 
{
  std::cout << __METHOD_NAME__ << "starting tracking" << std::endl;
  
  if (!bunch) return; //can't track nothing
  TRKLineConstIter elIter = line->begin();
  //iterate over number of turns
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
	}// end of beamline iteration
    }// end of turns iteration
}
