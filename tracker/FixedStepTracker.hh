#ifndef FIXEDSTEPTRACKER_H
#define FIXEDSTEPTRACKER_H

#include "Tracker.hh"

#include <memory>

#include "TRKBacktracker.hh"

class TRKOutput;

class TRKBunch;
class TRKLine;
class TRKStrategy;


namespace GMAD
{
  class Options;
}


namespace trk {
  

class FixedStepTracker: public Tracker
{
public:
  FixedStepTracker(TRKLine*             line,
		   TRKStrategy*         strategy,
		   const GMAD::Options& options,
		   std::shared_ptr<TRKOutput> outputIn);
  void Track(TRKBunch*) override;

private:
  TRKLine* beamline;
  TRKStrategy* strategy;
  int maxturns;
  bool useaperture;
  TRKBacktracker backtracker;
  std::shared_ptr<TRKOutput> output;

};

}

#endif /* FIXEDSTEPTRACKER_H */
