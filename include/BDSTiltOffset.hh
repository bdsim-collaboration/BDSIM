#ifndef BDSTILTOFFSET_H
#define BDSTILTOFFSET_H

#include "globals.hh" // geant4 types / globals

/**
 * @brief A holder for any placement offsets and rotations for a
 * BDSAcceleratorComponent.
 * 
 * Note, no rotation variables are provided other than along the beam
 * axis (z) as this would cause overlapping volumes between a component
 * and the next one in the line.  This leads to much more complicated 
 * geometry or padding space (perhaps acceptable for very low angles), 
 * but for now is left to be implemented in future.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSTiltOffset
{
public:
  BDSTiltOffset();

  BDSTiltOffset(G4double xOffset,
		G4double yOffset,
		G4double tiltIn);

  ///@{ Accessor
  inline G4double GetXOffset() const;
  inline G4double GetYOffset() const;
  inline G4double GetTilt()    const;
  ///@}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSTiltOffset const &to);
  
private:
  /// Horizontal displacement (mm) - note right handed coordinate system
  G4double dx;
  /// Vertical displacement (mm)
  G4double dy;
  /// Tilt angle (rad) - rotation angle about Z axis
  G4double tilt;
};

inline G4double BDSTiltOffset::GetXOffset() const
{return dx;}

inline G4double BDSTiltOffset::GetYOffset() const
{return dy;}

inline G4double BDSTiltOffset::GetTilt() const
{return tilt;}

#endif
