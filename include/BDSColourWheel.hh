#ifndef __BDS_COLOUR_WHEEL_HH
#define __BDS_COLOUR_WHEEL_HH
#include "G4Colour.hh"
#include <list>
#include <iterator>

class BDSColourWheel{
public:
  BDSColourWheel();
  ~BDSColourWheel();
  G4Colour colour();
  void spin();
  void reset();
  
private:
  std::list<G4Colour> _palette;
  std::list<G4Colour>::const_iterator _colour_it;
};

#endif
