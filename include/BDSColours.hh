#ifndef BDSCOLOURS_H
#define BDSCOLOURS_H

#include "globals.hh"         // geant4 types / globals
#include "G4Colour.hh"

#include <map>

class BDSColours
{
public:
  static BDSColours* Instance(); /// singleton pattern

  ~BDSColours();

  G4Colour* GetColour(G4String type);
  G4Colour* GetMagnetColour(G4int magnetOrder);

private:
  BDSColours(); //private constructor as singleton
  static BDSColours* _instance;

  /// A map of the magnet name for which order it is, which
  /// can be used to access the appropriate colour map
  std::map<G4int, G4String> magnetName;

  /// A map of the colour for each type or component by name
  std::map<G4String, G4Colour*> colours;
};

#endif
