#include "BDSMagnetColours.hh"

#include "globals.hh"            // geant4 types / globals
#include <map>
#include "G4Colour.hh"


BDSMagnetColours* BDSMagnetColours::_instance = 0;

BDSMagnetColours* BDSMagnetColours::Instance()
{
  if (_instance == 0)
    {_instance = new BDSMagnetColours();}
  return _instance;
}

BDSMagnetColours::BDSMagnetColours()
{
  magnetName[2] = "quadrupole";
  magnetName[3] = "sextupole";
  magnetName[4] = "octupole";
  magnetName[5] = "decapole";
  
  colours["default"]          = new G4Colour(1,     0.078, 0.576); // hot warning pink
  colours["sectorbend"]       = new G4Colour(0,     0,     1);     // blue
  colours["rectangularbend"]  = new G4Colour(0,     0,     1);     // blue
  colours["quadrupole"]       = new G4Colour(1,     0,     0);     // red
  colours["sextupole"]        = new G4Colour(1,     1,     0);     // yellow
  colours["octupole"]         = new G4Colour(0,     1,     0);     // green
  colours["decapole"]         = new G4Colour(0,     1,     0);     // green
  colours["solenoid"]         = new G4Colour(1,     0.549, 0);     // orange
  colours["multipole"]        = new G4Colour(0.466, 0.533, 0.6);   // slate gray
  colours["rfcavity"]         = new G4Colour(0.466, 0.533, 0.6);   // slate gray
  colours["muspoiler"]        = new G4Colour(0,     0.807, 0.819); // "light blue" / tab blue
  colours["vkicker"]          = new G4Colour(0,     0,     1);     // blue
  colours["hkicker"]          = new G4Colour(0,     0,     1);     // blue
}

G4Colour* BDSMagnetColours::GetMagnetColour(G4String magnetTypeName)
{
  if (colours.find(magnetTypeName) == colours.end())
    {// colour not found
      return colours["default"];
    }
  else
    {// colour must therefore exist
      return colours[magnetTypeName];
    }
}

G4Colour* BDSMagnetColours::GetMagnetColour(G4int magnetOrder)
{
  if (magnetOrder < 2)
    { // too low order
      return colours["default"];
    }
  else if (magnetOrder > 5)
    { // high order multipole
      return colours["multipole"];
    }
  else
    { // specified order
      return colours[magnetName[magnetOrder]];
    }
}
