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

  /// returns path from which BDSIM is executed
  /// supports linux/unix and mac OS
  std::string GetBDSIMExecPath();

  /// get full absolute directory path where file can be found.
  /// returns absolute path
  ///
  /// option to exclude the filename path, such that
  /// getFullPath(filename,true) + filename 
  /// will return the absolute filename path
  G4String GetFullPath(G4String filename, bool excludePathFromFileName=false);

  
  /** 
      Try to catch abort signals. This is not guaranteed to work.
      Main goal is to close output stream / files.
  */
  void HandleAborts(int signal_number);
}


#endif
