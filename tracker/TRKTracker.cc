#include <iostream>
#include "BDSDebug.hh"

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

TRKTracker::TRKTracker(TRKLine* lineIn, TRKStrategy* strategyIn):line(lineIn),strategy(strategyIn)
{
  //need to add variable TRKApertureStrategy* aperturestrategy here
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
      //(*elIter)->CheckAperture(bunch,aperturestrategy);

      
      //CheckAperture(bunch,*elIter);
    }
  }
}

void TRKTracker::CheckAperture(TRKBunch* bunch,TRKElement* element) {
#ifdef BDSDEBUG
  std::cout << __METHOD_NAME__ << " at element " << *element << std::endl;
#endif
  //copy aperture here for quick access (avoid pointer traversal)
  TRKAperture& aperture = *(element->GetAperture());

  //aperture->CheckBunch(*bunch);
 
  
  TRKBunchIter iter     = bunch->begin();
  TRKBunchIter  end     = bunch->end();
  
  //iterate over bunch and use the aperture check function from this element
  //create output vector to copy to
  //unlikely to be 100% loss so don't initialise as big as
  
  //std::vector<TRKParticle> lp(1000); //lost particles
  //std::vector<TRKParticle>::iterator lpp = lp.begin();
  
  //std::cout << "Initial size of temp lost particles vector " << lp.size() << std::endl;
  //std::copy_if(iter,end,std::back_inserter(lp),[](TRKBunchIter iterIn){return aperture.OutsideAperture(*iterIn;)});
  
  //std::copy_if(iter,end,lostparticles.begin(),aperture->OutsideAperture);
  //iter = bunch->begin();
  //std::remove_if(iter,end,true);
  //std::remove_if(iter,end,aperture->OutsideAperture(*iter));

  //
  //while (iter != end) {
  //  (aperture.OutisdeAperture(*iter)) ? {*lpp = *iter;++lpp 
  /*
  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    (aperture->OutsideAperture(part)) ? Shift(bunch,iter) : part.ConfirmNewCoordinates();
  }
  */
  /*
  std::cout << "Number of lost particles " << lostparticles.size() << std::endl;
  for (std::vector<TRKParticle>::iterator i = lostparticles.begin(); i != lostparticles.end(); ++i)
    {
      std::cout << *i << std::endl;
    }
  */
}

void TRKTracker::Shift(TRKBunch* bunch, TRKBunchIter& iter) {
  //1 copy particle to bdsim queue
  //2 delete particle from bunch... or flag for deletion at end..
  std::cout << "Naughty particle! shifting to BDSIM... not implemented yet" << std::endl;
  iter = bunch->Erase(iter);
  iter--;
}
