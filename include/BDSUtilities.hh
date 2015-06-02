#ifndef BDSUTILITIES_H
#define BDSUTILITIES_H

#include "globals.hh"   // geant4 globals / types
#include <string>

/**
 * @brief Various utility functions that have no specific place - 
 * for example string manipulations, and checking
 *
 * Functions are part of the BDS namespace 
 * @author Laurie Nevay <Laurie.Nevay@rhul.ac.uk>
 */

namespace BDS {
  /// Logical not for isalpha predictator as needed for string manipulations
  struct non_alpha
  {
    G4bool operator()(char c);
  };

  /// Remove white space and special characters in the name
  G4String PrepareSafeName(G4String name);

  /// Calculate the +- 1 orientation multiplier for absolute angles
  /// seems trivial, but used in a lot of places so put in one place
  G4int    CalculateOrientation(G4double angle);

  /// Returns path from which BDSIM is executed
  /// supports linux/unix and mac OS
  std::string GetBDSIMExecPath();

  /** 
      Try to catch abort signals. This is not guaranteed to work.
      Main goal is to close output stream / files.
  */
  void HandleAborts(int signal_number);

  /// Determine whether a parameter is finite
  G4bool IsFinite(const G4double& variable);
}


#endif
