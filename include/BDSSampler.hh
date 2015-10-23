#ifndef BDSSAMPLER_H
#define BDSSAMPLER_H 

#include "BDSSamplerBase.hh"

class BDSSamplerSD;
class G4Box;
class G4LogicalVolume;

/** 
 * Rectangular sampler class
 */

class BDSSampler : public BDSSamplerBase
{
public:
  BDSSampler(G4String name);

  /// access for external classes to sensitive detector
  virtual BDSSamplerSD* GetSensitiveDetector()const override final;
  
private:
  /// build container logical volume
  virtual void BuildContainerLogicalVolume() override final;

  /// static containerSolid (since same for every sampler)
  static G4Box* containerSolidSampler;
  /// static containerLogicalVolume (since same for every sampler)
  static G4LogicalVolume* containerLogicalVolumeSampler;
};

#endif
