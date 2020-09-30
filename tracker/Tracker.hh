#ifndef TRACKER_H
#define TRACKER_H

class TRKBunch;

namespace trk {
class Tracker {
public:
  virtual void Track(TRKBunch *bunch) = 0;
  virtual ~Tracker(){};
};
} // namespace trk

#endif /* TRACKER_H */
