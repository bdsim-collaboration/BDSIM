#include "BDSColourWheel.hh"

#include "G4Colour.hh"

BDSColourWheel::BDSColourWheel()
{
  palette.push_back(G4Colour(1.0, 0.0, 0.0, 0.4));
  palette.push_back(G4Colour(0.0, 1.0, 0.0, 0.4));
  palette.push_back(G4Colour(0.0, 0.0, 1.0, 0.4));
  Reset();
}

BDSColourWheel::~BDSColourWheel()
{;}

void BDSColourWheel::Spin()
{
  colour_it++;
  if(colour_it == palette.end())
    {Reset();}
}

void BDSColourWheel::Reset()
{
  colour_it = palette.begin();
}

G4Colour BDSColourWheel::Colour()
{
  return (*colour_it);
}
