/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSRfCavity_h
#define BDSRfCavity_h

#include"globals.hh"

#include "BDSMultipole.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMaterials.hh"
#include "BDSTunnelInfo.hh"

#include "G4ChordFinder.hh"
#include "G4EqMagElectricField.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4MagErrorStepper.hh"
#include "G4UserLimits.hh"
#include "G4UniformElectricField.hh"

class BDSRfCavity :public BDSMultipole
{
  public:
  BDSRfCavity(G4String           name,
	      G4double           length,
	      G4double           grad,
	      BDSBeamPipeInfo    beamPipeInfoIn,
	      BDSMagnetOuterInfo magnetOuterInfo,
	      BDSTunnelInfo      tunnelInfo);
  ~BDSRfCavity(){;};
  
  private:

  virtual void BuildBPFieldAndStepper();

  G4double itsGrad; // longitudinal E field grad in MV / m

  // field related objects:
  G4UniformElectricField* itsEField;
  G4ChordFinder*          fChordFinder ;
  G4MagIntegratorStepper* fStepper ;
  G4MagInt_Driver*        fIntgrDriver;
};

#endif
