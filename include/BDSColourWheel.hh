#ifndef BDSCOLOURWHEEL_H
#define BDSCOLOURWHEEL_H

#include "G4Colour.hh"

#include <list>
#include <iterator>

/**
 * @brief Three colours that are supplied sequentially.
 *
 * @author Lawrence Deacon.
 */

class BDSColourWheel
{
public:
  BDSColourWheel();
  ~BDSColourWheel();

  /// Return current colour.
  G4Colour Colour();

  /// Iterate colour choice from RGB.
  void Spin();

  /// Start sequence again.
  void Reset();
  
private:
  std::list<G4Colour> palette;
  std::list<G4Colour>::const_iterator colour_it;
};

#endif
