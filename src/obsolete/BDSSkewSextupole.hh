/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSSkewSextupole_h
#define BDSSkewSextupole_h 1

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"
#include "BDSSkewSextStepper.hh"

#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"               

#include "BDSMultipole.hh"
#include "BDSSkewSextMagField.hh"

class BDSSkewSextupole :public BDSMultipole
{
  public:
    BDSSkewSextupole(G4String& aName, G4double aLength,
		     G4double bpRad,G4double FeRad,
                     std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                     G4String aTunnelMaterial, G4String aMaterial,		     G4double BDblPrime);
  ~BDSSkewSextupole();

  protected:

  private:
  G4double itsBDblPrime;

  //  void BuildOuterLogicalVolume();
  void BuildBPFieldAndStepper();
  //void BuildMarkerLogicalVolume();

  G4VisAttributes* SetVisAttributes();

  // field related objects:
  BDSSkewSextStepper* itsStepper;
  BDSSkewSextMagField* itsMagField;
  G4Mag_UsualEqRhs* itsEqRhs;
};

#endif
