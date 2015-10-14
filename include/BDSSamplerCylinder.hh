#ifndef BDSSAMPLERCYLINDER_H
#define BDSSAMPLERCYLINDER_H

#include "BDSSamplerBase.hh"

class BDSSamplerSD;

/** 
 * Cylindrical sampler class
 */

class BDSSamplerCylinder :public BDSSamplerBase
{
public:
  BDSSamplerCylinder(G4String name,G4double length, G4double radiusIn);

  /// access for external classes to sensitive detector
  virtual BDSSamplerSD* GetSensitiveDetector() override final;

private:
  virtual void BuildContainerLogicalVolume() override final;

  /// radius of cylindrical sampler
  G4double radius;
};

#endif
