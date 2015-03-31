#ifndef BDSUTILITIES_H
#define BDSUTILITIES_H

#include "globals.hh"   // geant4 globals / types

/**
 * @brief Various utility functions that have no specific place - 
 * for example string manipulations, and checking
 *
 * Functions are part of the BDS namespace 
 * @author Laurie Nevay <Laurie.Nevay@rhul.ac.uk>
 */

namespace BDS {
  /// logical not for isalpha predictator as needed for string manipulations
  struct non_alpha
  {
    G4bool operator()(char c);
  };

  // remove white space and special characters in the name
  G4String PrepareSafeName(G4String name);

  /// calculate the +- 1 orientation multiplier for absolute angles
  /// seems trivial, but used in a lot of places so put in one place
  G4int    CalculateOrientation(G4double angle);


  
}


#endif
