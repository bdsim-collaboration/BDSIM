#ifndef TRACKERUTILS_H
#define TRACKERUTILS_H

#include <memory>

namespace GMAD {
  class Options;
}

class TRKLine;
class TRKStrategy;

namespace trk
{
  class Tracker;
  class EventOutput;

  std::unique_ptr<Tracker> CreateTracker(TRKLine*,
					 TRKStrategy*,
					 const GMAD::Options &options,
					 std::shared_ptr<EventOutput>);
  
}


#endif /* TRACKERUTILS_H */
