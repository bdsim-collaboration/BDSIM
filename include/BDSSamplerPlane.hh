#ifndef BDSSAMPLERPLANE_H
#define BDSSAMPLERPLANE_H 

#include "BDSSampler.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

/** 
 * @brief Rectangular sampler with fixed thickness but variable x,y.
 * 
 * This produces a square box that's 1pm thick but has square half
 * width of boxHalfWidth.
 */

class BDSSamplerPlane: public BDSSampler
{
public:
  BDSSamplerPlane(G4String      name,
		  G4Transform3D transform,
		  G4double      boxHalfWidth);

  virtual ~BDSSamplerPlane();

  virtual void SetSensitiveDetector();

private:
  /// Private default constructor to ensure use of provided one.
  BDSSamplerPlane();

  /// The chord length for all is fixed and can be static.
  static const G4double chordLength;
};

#endif
