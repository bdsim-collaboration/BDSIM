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

  /// Returns current number of samplers
  static int GetNSamplers();

  /// Method to add sampler independent of beamline
  static void AddExternalSampler(G4String outputName);

  /// Names of samplers for output
  static std::vector <G4String> outputNames;

  /// Access for external classes to sensitive detector
  virtual BDSSamplerSD* GetSensitiveDetector() const=0;

  /// Get SamplerId
  G4int GetSamplerNumber()const {return nThisSampler;}

  /// Get OutputName. [nthisSampler-1] since count starts at 1.
  G4String GetOutputName()const {return outputNames[nThisSampler-1];}

protected:
  /// Build container volume
  virtual void BuildContainerLogicalVolume();
private:
  /// Set user limits
  void SetUserLimits();

  /// Set vis attributes and sensitive detector
  void SetVisAttributes();
  
  /// ID of sampler (count starts at 1)
  G4int nThisSampler;

  /// number of total Samplers
  static G4int nSamplers;
};

#endif
