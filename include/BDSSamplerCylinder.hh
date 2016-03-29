#ifndef BDSSAMPLERCYLINDER_H
#define BDSSAMPLERCYLINDER_H

#include "BDSSampler.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

class BDSSamplerSD;

/** 
 * @brief Cylindrical sampler around an object.
 * 
 * Creates a hollow very thin (1um thick) cylinder around an object
 * without end caps that acts as a sampler.
 */

class BDSSamplerCylinder: public BDSSampler
{
public:
  BDSSamplerCylinder(G4String name,
		     G4double lengthIn,
		     G4double radiusIn);

  virtual ~BDSSamplerCylinder(){;}

private:
  /// Private default constructor to ensure use of provided one.
  BDSSamplerCylinder();
};

#endif
