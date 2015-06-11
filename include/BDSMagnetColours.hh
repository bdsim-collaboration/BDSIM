#ifndef BDSMAGNETCOLOURS_H
#define BDSMAGNETCOLOURS_H

#include "globals.hh"         // geant4 types / globals
#include "G4Colour.hh"

#include <map>

class BDSMagnetColours
{
public:
  static BDSMagnetColours* Instance(); /// singleton pattern

  ~BDSMagnetColours();

  G4Colour* GetMagnetColour(G4String magnetTypeName);
  G4Colour* GetMagnetColour(G4int    magnetOrder);

private:
  BDSMagnetColours(); //private constructor as singleton
  static BDSMagnetColours* _instance;

  /// A map of the magnet name for which order it is, which
  /// can be used to access the appropriate colour map
  std::map<G4int, G4String> magnetName;

  /// A map of the colour for each type of magnet by name -
  /// one for each class that inherits the BDSMagnet class
  std::map<G4String, G4Colour*> colours;
};

#endif
