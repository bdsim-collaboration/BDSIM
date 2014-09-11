/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSSampler_h
#define BDSSampler_h 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSSamplerSD.hh"

#include "G4VisAttributes.hh"

class BDSSampler : public BDSAcceleratorComponent
{
public:
  BDSSampler(G4String aName,G4double aLength);
  ~BDSSampler();

  static int GetNSamplers();
  static void AddExternalSampler(G4String outputName);

  /// names of samplers for output
  static std::vector <G4String> outputNames;

  /// access for external classes to sensitive detector
  static BDSSamplerSD* GetSensitiveDetector(){return SensitiveDetector;}

private:
  virtual void BuildMarkerLogicalVolume();

  virtual G4VisAttributes* SetVisAttributes();

  /// id of sampler
  int nThisSampler;
  /// number of total Samplers
  static int nSamplers;
  /// pointer to sensitive detector, only one for all samplers
  static BDSSamplerSD* SensitiveDetector;
};

#endif
