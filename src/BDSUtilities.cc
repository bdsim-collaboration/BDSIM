#include "BDSUtilities.hh"
#include <cctype>
#include "globals.hh"        // geant4 globals / types

/// logical not for isalpha predictator as needed for string manipulations
G4bool BDS::non_alpha::operator()(char c)
{
  return !isalpha(c);
}
