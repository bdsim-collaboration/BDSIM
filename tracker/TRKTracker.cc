#include <iostream>
#include "BDSDebug.hh"

#include "TRKTracker.hh"

#include "TRKBunch.hh"
#include "TRKLine.hh"
#include "TRKStrategy.hh"
#include "TRKParticle.hh"
#include "TRKElement.hh"

TRKTracker::TRKTracker(TRKLine* lineIn, TRKStrategy* strategyIn):line(lineIn),strategy(strategyIn)
{
  maxTurns = (line->GetCircular() ? TRK::NR_TURNS : 1);
}

TRKTracker::~TRKTracker() {
}

void TRKTracker::Track(TRKBunch* bunch){
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << std::endl;
#endif
  if (!bunch) return; //can't track nothing
  TRKLineConstIter elIter = line->begin();
  //iterate over number of turns
  for (unsigned int i=0; i<maxTurns; i++) {
#ifdef TRKDEBUG
    std::cout << "Turn Number: " << i << " / " << maxTurns << std::endl;
#endif
    //iterate over beamline elements
    for (;elIter!=line->end(); ++elIter) {
#ifdef TRKDEBUG
      std::cout << "before tracking" << std::endl;
      std::cout << *bunch;
#endif
      //track all particles through an element
      (*elIter)->Track(bunch,strategy);
#ifdef TRKDEBUG
      std::cout << "after tracking" << std::endl;
      std::cout << *bunch;
#endif
      //check the aperture and decide whether to shift to bdsim
      CheckAperture(bunch,*elIter);
    }
  }
}

void TRKTracker::CheckAperture(TRKBunch* bunch,TRKElement* element) {
#ifdef BDSDEBUG
  std::cout << __METHOD_NAME__ << " at element " << *element << std::endl;
#endif
  TRKAperture* aperture = element->GetAperture();
  TRKBunchIter iter     = bunch->begin();
  TRKBunchIter  end     = bunch->end();
  
  //iterate over bunch and use the aperture check function from this element
  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    (aperture->OutsideAperture(part)) ? Shift(part) : part.ConfirmNewCoordinates();
  }
}

void TRKTracker::Shift(TRKParticle& /*particle*/) {
  //1 copy particle to bdsim queue
  //2 delete particle from bunch... or flag for deletion at end..
  std::cout << "Naughty particle! shifting to BDSIM... not implemented yet" << std::endl;
;}
