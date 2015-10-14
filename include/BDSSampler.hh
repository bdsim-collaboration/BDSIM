#ifndef BDSSAMPLER_H
#define BDSSAMPLER_H 

#include "BDSSamplerBase.hh"

class BDSSamplerSD;

/** 
 * Rectangular sampler class
 */

class BDSSampler : public BDSSamplerBase
{
public:
  BDSSampler(G4String name,
	     G4double length);


  /// access for external classes to sensitive detector
  virtual BDSSamplerSD* GetSensitiveDetector()const override final;
  
private:
  virtual void BuildContainerLogicalVolume() override final;
};

#endif
