#ifndef BDSSAMPLERCYLINDER_H
#define BDSSAMPLERCYLINDER_H

#include "BDSSampler.hh"

class BDSSamplerSD;
class G4Transform3D;

/** 
 * Cylindrical sampler class
 */

class BDSSamplerCylinder: public BDSSampler
{
public:
  BDSSamplerCylinder(G4String       name,
		     G4Transform3D* transform,
		     G4double       lengthIn,
		     G4double       radiusIn);

  virtual ~BDSSamplerCylinder();

  /// access for external classes to sensitive detector
  virtual BDSSamplerSD* GetSensitiveDetector()const override final;

private:
  /// Private default constructor to ensure use of provided one.
  BDSSamplerCylinder();
};

#endif
