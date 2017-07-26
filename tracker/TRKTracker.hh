#ifndef TRKTracker_h
#define TRKTracker_h

#include "TRKBunch.hh"

class BDSOutput;

class TRKLine;
class TRKStrategy;
class TRKElement;
class TRKParticle;

namespace GMAD
{
  class Options;
}

/**
 * @brief class that takes care of the tracker
 */

class TRKTracker
{
public:
  TRKTracker(TRKLine*             line,
	     TRKStrategy*         strategy,
	     const GMAD::Options& options,
	     BDSOutput*           outputIn);
  ~TRKTracker();
  /// track bunch through beamline - main tracking loop
  void Track(TRKBunch* bunch);
  /// move a particle to bdsim tracking queue and delete from bunch, 
  void Shift(TRKBunch* bunch, TRKBunchIter& iter);

private:
  TRKLine*  line;
  /// tracking type
  TRKStrategy* strategy;
  /// aperture check
  void CheckAperture(TRKBunch* bunch, TRKElement* element);
  /// maximum number of turns
  unsigned int maxTurns;
  bool dontuseaperture;

  BDSOutput* output;
};

#endif
