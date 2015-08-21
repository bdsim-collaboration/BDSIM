#include "BDSMagnetColours.hh"

#include "globals.hh"            // geant4 types / globals
#include <map>
#include "G4Colour.hh"


BDSMagnetColours* BDSMagnetColours::_instance = nullptr;

BDSMagnetColours* BDSMagnetColours::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSMagnetColours();}
  return _instance;
}

BDSMagnetColours::~BDSMagnetColours()
{
  _instance = nullptr;
}

BDSMagnetColours::BDSMagnetColours()
{
  magnetName[1] = "sectorbend";
  magnetName[2] = "quadrupole";
  magnetName[3] = "sextupole";
  magnetName[4] = "octupole";
  magnetName[5] = "decapole";
  
  colours["default"]          = new G4Colour(1,     0.078, 0.576); // hot warning pink
  colours["sectorbend"]       = new G4Colour(0,     0.4,   0.8);   // blue
  colours["rectangularbend"]  = new G4Colour(0,     0.4,   0.8);   // blue
  colours["quadrupole"]       = new G4Colour(1,     0,     0);     // red
  colours["sextupole"]        = new G4Colour(1,     0.8,   0);     // yellow
  colours["octupole"]         = new G4Colour(0,     0.6,   0.3);   // green
  colours["decapole"]         = new G4Colour(0.4,   0,     0.8);   // green
  colours["solenoid"]         = new G4Colour(1,     0.549, 0);     // orange
  colours["multipole"]        = new G4Colour(0.466, 0.533, 0.6);   // slate gray
  colours["rfcavity"]         = new G4Colour(0.466, 0.533, 0.6);   // slate gray
  colours["muspoiler"]        = new G4Colour(0,     0.807, 0.819); // "light blue" / tab blue
  colours["vkicker"]          = new G4Colour(0,     0.4,   0.8);   // blue
  colours["hkicker"]          = new G4Colour(0,     0.4,   0.8);   // blue
  colours["coil"]             = new G4Colour(0.722, 0.451, 0.2);   // copper
  colours["gdml"]             = new G4Colour(0.4  , 0.2,   0.0);   // poo brown
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
  if (magnetOrder < 1)
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
