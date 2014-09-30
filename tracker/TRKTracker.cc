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
  if (!bunch) return;
  TRKLineConstIter elIter = line->begin();
  for (unsigned int i=0; i<maxTurns; i++) {
#ifdef TRKDEBUG
    std::cout << "Turn Number: " << i << " / " << maxTurns << std::endl;
#endif
    for (;elIter!=line->end(); ++elIter) {
#ifdef TRKDEBUG
      std::cout << "before tracking" << std::endl;
      std::cout << *bunch;
#endif
      (*elIter)->Track(bunch,strategy);
#ifdef TRKDEBUG
      std::cout << "after tracking" << std::endl;
      std::cout << *bunch;
#endif
      CheckAperture(bunch,*elIter);
    }
  }
}

void TRKTracker::CheckAperture(TRKBunch* bunch,TRKElement* element) {
#ifdef BDSDEBUG
  std::cout << __METHOD_NAME__ << " at element " << *element << std::endl;
#endif
  TRKAperture* aperture = element->GetAperture();
  //do the actual comparison
  //if bad set some flag
  //else update
  TRKBunchIter iter = bunch->begin();
  TRKBunchIter  end = bunch->end();
  
  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    //do check here with if else
    (aperture->OutsideAperture(part)) ? Shift(part) : part.ConfirmNewCoordinates();
  }
}

void TRKTracker::Shift(TRKParticle& particle) {
  std::cout << "Naughty particle! shifting to BDSIM... not implemented yet" << std::endl;
;}
