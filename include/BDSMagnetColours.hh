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

private:
  BDSMagnetColours(); //private constructor as singleton
  static BDSMagnetColours* _instance;

  std::map<G4String, G4Colour*> colours;
};

#endif
