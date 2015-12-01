#ifndef BDSUTILITIES_H
#define BDSUTILITIES_H

#include "globals.hh"   // geant4 globals / types
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

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

  /// Checks if filename exists
  G4bool FileExists(G4String filename);
  
  /// Returns path from which BDSIM is executed
  /// supports linux/unix and mac OS
  std::string GetBDSIMExecPath();

  /// get full absolute directory path where file can be found.
  /// returns absolute path
  ///
  /// option to exclude the filename from path, such that
  /// getFullPath(filename,true) + filename 
  /// will return the absolute filename path
  G4String GetFullPath(G4String filename, bool excludeNameFromPath=false);

  /// Try to catch abort signals. This is not guaranteed to work.
  /// Main goal is to close output stream / files.
  void HandleAborts(int signal_number);

  ///@{ Determine whether a parameter is finite
  G4bool IsFinite(const G4double& variable);
  G4bool IsFinite(const G4ThreeVector& variable);
  ///@}

  /// Check if character array is an integer, and returns the integer by reference
  G4bool IsInteger(const char* s, int& convertedInteger);
  
  /// Check if character array is an integer, and returns the double by reference
  G4bool IsNumber(const char* s, double& convertedNumber);
  
  /// Print out details of a rotation matrix - the matrix itself, unit vectors.
  /// Optional keyname to identify in output stream
  void PrintRotationMatrix(G4RotationMatrix* rm, G4String keyName = "unkown");

  /// Check if the geant4 environmental variables necessary for a run are set
  G4bool Geant4EnvironmentIsSet();
    
  ///@{ Get parameter value from the specification ('spec') string
  G4double GetParameterValue      (const G4String spec, const G4String name);
  G4String GetParameterValueString(const G4String spec, const G4String name);
  ///@}
}


#endif
