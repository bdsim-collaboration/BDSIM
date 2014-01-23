/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSSextupole_h
#define BDSSextupole_h 1

#include "globals.hh"
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"
#include "BDSSextStepper.hh"

#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"               

#include "BDSMultipole.hh"
#include "BDSSextMagField.hh"

class BDSSextupole :public BDSMultipole
{
public:
  BDSSextupole(G4String aName, G4double aLength,
	       G4double bpRad, G4double FeRad,
	       G4double BDblPrime, G4double tilt, G4double outR, 
               std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
               G4String aTunnelMaterial = "",
	       G4String aMaterial = "");
  ~BDSSextupole();

  void SynchRescale(G4double factor);

protected:

private:
  G4double itsBDblPrime;

  void BuildBPFieldAndStepper();

  G4VisAttributes* SetVisAttributes();

  // field related objects:
  BDSSextStepper* itsStepper;
  BDSSextMagField* itsMagField;
  G4Mag_UsualEqRhs* itsEqRhs;

};

#endif
