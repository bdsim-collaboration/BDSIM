/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSRfCavity_h
#define BDSRfCavity_h

#include"globals.hh"

#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"

#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4UserLimits.hh"

#include "G4UniformElectricField.hh"
#include "G4MagErrorStepper.hh"
#include "G4EqMagElectricField.hh"

#include "BDSMultipole.hh"

class BDSRfCavity :public BDSMultipole
{
  public:
  BDSRfCavity(G4String aName, G4double aLength,G4double bpRad, G4double grad, 
              G4String aTunnelMaterial="",G4String aMaterial = "");
  BDSRfCavity(G4String        name,
	      G4double        length,
	      G4double        grad,
	      BDSBeamPipeType beamPipeType,
	      G4double        aper1,
	      G4double        aper2,
	      G4double        aper3,
	      G4double        aper4,
	      G4Material*     vacuumMaterial,
	      G4double        beamPipeThickness,
	      G4Material*     beamPipeMaterial,
	      G4double        boxSize,
	      G4String        outerMaterial="",
	      G4String        tunnelMaterial="",
	      G4double        tunnelRadius=0,
	      G4double        tunnelOffsetX=0);
  ~BDSRfCavity(){;};
  
  private:

  virtual void BuildBPFieldAndStepper();

  virtual void SetVisAttributes();

  G4double itsGrad; // longitudinal E field grad in MV / m

  // field related objects:
  G4UniformElectricField* itsEField;
  G4ChordFinder*          fChordFinder ;
  G4MagIntegratorStepper* fStepper ;
  G4MagInt_Driver*        fIntgrDriver;
};

#endif
