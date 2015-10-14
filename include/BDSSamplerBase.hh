#ifndef BDSSAMPLERBASE_H
#define BDSSAMPLERBASE_H 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

class BDSSamplerSD;

/** 
 * Abstract base class for output storage elements (Samplers)
 */

class BDSSamplerBase : public BDSAcceleratorComponent
{
public:
  /// constructor with sampler name, length and type
  BDSSamplerBase(G4String name, G4double length, G4String type);
  virtual ~BDSSamplerBase();

  /// returns current number of samplers
  static int GetNSamplers();
  /// method to add sampler independent of beamline
  static void AddExternalSampler(G4String outputName);

  /// names of samplers for output
  static std::vector <G4String> outputNames;

  /// access for external classes to sensitive detector
  virtual BDSSamplerSD* GetSensitiveDetector()=0;

  /// get SamplerId
  G4int getSamplerNumber() {return nThisSampler;}
  
protected:
  /// build container volume
  virtual void BuildContainerLogicalVolume();
private:
  /// set user limits
  void SetUserLimits();
  /// set vis attributes and sensitive detector
  void SetVisAttributes();
  
  /// id of sampler
  G4int nThisSampler;
  /// number of total Samplers
  static G4int nSamplers;
};

#endif
