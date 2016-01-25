#ifndef BDSSAMPLERCYLINDER_H
#define BDSSAMPLERCYLINDER_H

#include "BDSSampler.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

class BDSSamplerSD;

/** 
 * Cylindrical sampler class
 */

class BDSSamplerCylinder: public BDSSampler
{
public:
  BDSSamplerCylinder(G4String      name,
		     G4Transform3D transform,
		     G4double      lengthIn,
		     G4double      radiusIn);

  virtual ~BDSSamplerCylinder();

  virtual void SetSensitiveDetector();

private:
  /// Private default constructor to ensure use of provided one.
  BDSSamplerCylinder();
};

#endif
