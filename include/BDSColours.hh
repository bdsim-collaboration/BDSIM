#ifndef BDSCOLOURS_H
#define BDSCOLOURS_H

#include "globals.hh"         // geant4 types / globals
#include "G4Colour.hh"

#include <map>

/**
 * @brief Colour class that holds all colours used in BDSIM
 */

class BDSColours
{
public:
  static BDSColours* Instance(); ///< singleton pattern

  ~BDSColours();

  /// Get colour from name
  G4Colour* GetColour(G4String type) const;
  /// Get colour from magnet order (1=dipole, 2=quadrupole, etc.)
  G4Colour* GetMagnetColour(G4int magnetOrder) const;

private:
  BDSColours(); ///< Private constructor as singleton
  static BDSColours* _instance;

  /// A map of the magnet name for which order it is, which
  /// can be used to access the appropriate colour map
  std::map<G4int, G4String> magnetName;

  /// A map of the colour for each type or component by name
  std::map<G4String, G4Colour*> colours;
};

#endif
