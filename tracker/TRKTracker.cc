#include "TRKTracker.hh"

#include "TRKBunch.hh"
#include "TRKLine.hh"
#include "TRKStrategy.hh"

TRKTracker::TRKTracker(TRKLine* lineIn, TRKStrategy* strategyIn):line(lineIn),strategy(strategyIn)
{
  maxTurns = (line->GetCircular() ? TRK::NR_TURNS : 1);
}

TRKTracker::~TRKTracker() {
}

void TRKTracker::Track(TRKBunch* bunch){
  if (!bunch) return;
  TRKLineConstIter elIter = line->begin();
  for (unsigned int i=0; i<maxTurns; i++) {
    for (;elIter!=line->end(); ++elIter) {
      (*elIter)->Track(bunch,strategy);
      /// vTemp = vOut;
      // Q(JS): transform to local of element or not?
      Check((*elIter)->GetAperture());
    }
  }
}

void TRKTracker::Check(TRKAperture* /*ap*/) {
}
