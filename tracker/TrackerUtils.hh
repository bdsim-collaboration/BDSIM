#ifndef TRACKERUTILS_H
#define TRACKERUTILS_H

#include <memory>

namespace GMAD {
  class Options;
}

class TRKLine;
class TRKStrategy;
class TRKOutput;

namespace trk
{
  class Tracker;

  std::unique_ptr<Tracker> CreateTracker(TRKLine*,
					 TRKStrategy*,
					 const GMAD::Options &options,
					 std::shared_ptr<TRKOutput>);
  
}


#endif /* TRACKERUTILS_H */
