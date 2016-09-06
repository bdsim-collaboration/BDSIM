#ifndef BDSUTILITIES_H
#define BDSUTILITIES_H

#include "BDSExtent.hh"

#include "globals.hh"   // geant4 globals / types
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"

#include <string>
#include <utility>

namespace CLHEP {
  class HepRotation;
}
typedef CLHEP::HepRotation G4RotationMatrix;

/**
 * @brief Various utility functions that have no specific place - 
 * for example string manipulations, and checking
 *
 * Functions are part of the BDS namespace 
 * @author Laurie Nevay
 */

namespace BDS
{
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

  /// Calculate input and output normal vector
  std::pair<G4ThreeVector,G4ThreeVector> CalculateFaces(G4double angleInIn,
							G4double angleOutIn);

  /// Calculate the radial distance that the two planes inputface and outputface intersect one another
  G4double CalculateFacesOverlapRadius(G4double angleInIn,
                            G4double angleOutIn,
                            G4double lengthIn);

  /// Ensure a variable passed by reference is capped within lowerLimit and upperLimit.
  /// If it's outside these it is defaulted to those limit values.
  void EnsureInLimits(G4double& value, G4double lowerLimit, G4double upperLimit);

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

  /// Rotate a two vector in polar coordinates by an angle.
  G4TwoVector Rotate(const G4TwoVector& vec, const G4double& angle);

  /// Check if two planes will overlap - as defined by their UNIT normal
  /// vectors at 0,0,0 and the z separation of the two. Incoming is assumed to have
  /// a positive z component and outgoing a -ve z component in this convention.
  G4bool WillIntersect(const G4ThreeVector& incomingNormal,
		       const G4ThreeVector& outgoingNormal,
		       const G4double&      zSeparation,
		       const BDSExtent&     incomingExtent,
		       const BDSExtent&     outgoingExtent);

  /// Checking for "cutted crossed Z planes" for a cylinder with angled faces. This
  /// utility only works in one dimension unlike the other one with the same name and
  /// vector arguments.
  G4bool WillIntersect(const G4double angleIn,
		       const G4double angleOut,
		       const G4double outerDiameter,
		       const G4double length);

  /// Given an x and y coordinate, calculate what the z coordinate must be to
  /// lie on the plane defined by the supplied normal vector - assumes plane
  /// intercepts 0,0,0. Used in WillIntersect.
  G4double GetZOfPointOnPlane(G4ThreeVector normal, G4double x, G4double y);

  /// Rotate a face normal vector for a given component to that of the reference
  /// trajectory on that face.  This takes the component full angle and assumes the
  /// component changes the angle continuously between its faces and therefore half
  /// of this angle is used for the rotation.
  G4ThreeVector RotateToReferenceFrame(G4ThreeVector faceNormal, G4double fullAngle);
}

#endif
