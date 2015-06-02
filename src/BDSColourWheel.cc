#include "BDSColourWheel.hh"

BDSColourWheel::~BDSColourWheel(){
}

BDSColourWheel::BDSColourWheel(){
  _palette.push_back(G4Colour(1.0, 0.0, 0.0, 0.4));
  _palette.push_back(G4Colour(0.0, 1.0, 0.0, 0.4));
  _palette.push_back(G4Colour(0.0, 0.0, 1.0, 0.4));
  reset();
}

void BDSColourWheel::spin(){
  _colour_it++;
  if(_colour_it == _palette.end()){
    reset();
  }
}

void BDSColourWheel::reset(){
  _colour_it=_palette.begin();
}

G4Colour BDSColourWheel::colour(){
  return (*_colour_it);
}
