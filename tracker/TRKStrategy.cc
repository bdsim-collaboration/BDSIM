#include "TRKStrategy.hh"

#include "BDSDebug.hh"
#include "BDSOutputBase.hh"
#include "TRKSampler.hh"
#include "TRKTiltOffset.hh"

extern BDSOutputBase* trkOutput;

TRKStrategy::TRKStrategy(int trackingStepsIn) : 
  trackingSteps(trackingStepsIn) {
}

TRKStrategy::~TRKStrategy() { 
}

/*
void TRKStrategy::Track(TRKElement* el, TRKBunch* bunch) {
  Track((TRKDrift*)el,bunch);
}
*/
void TRKStrategy::Track(TRKSampler* el, TRKBunch* bunch) {
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " Sampler writing output" << std::endl;
#endif
  //don't modify particles, but we write to output here
  trkOutput->WriteTrackerBunch(el->GetName(),bunch);
}

void TRKStrategy::Track(TRKTiltOffset* el, TRKBunch* bunch) {
  std::cout << __METHOD_NAME__ << " TiltOffset" << std::endl;

  // TODO
}
