/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 5.3.2005
   Copyright (c) 2005 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSSamplerCylinder_h
#define BDSSamplerCylinder_h 1

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"

#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

class BDSSamplerCylinder :public BDSAcceleratorComponent
{
public:
  BDSSamplerCylinder(G4String aName,G4double aLength, G4double aRadius);
  ~BDSSamplerCylinder();

  static int GetNSamplers();
  static void AddExternalSampler(G4String outputName);

  /// names of samplers for output
  static std::vector <G4String> outputNames;

protected:

private:
  void SamplerCylinderLogicalVolume();
  G4VisAttributes* SetVisAttributes();
  G4double itsRadius;

  /// id of sampler
  int nThisSampler;
  /// number of total Samplers
  static int nSamplers;
};

#endif
