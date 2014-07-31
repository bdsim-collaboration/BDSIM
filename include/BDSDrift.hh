/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSDrift_h
#define BDSDrift_h 1

#include "globals.hh"
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"

#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"               
#include "BDSDriftStepper.hh"
#include "BDSMagField.hh"
#include "BDSMultipole.hh"
#include "G4MagIntegratorStepper.hh"

class BDSDrift :public BDSMultipole
{
public:
  BDSDrift(G4String aName, 
	   G4double aLength,
           std::list<G4double> blmLocZ, 
	   std::list<G4double> blmLocTheta, 
	   G4double startAper, 
	   G4double endAper = BDSGlobalConstants::Instance()->GetBeampipeRadius(), 
	   G4String aTunnelMaterial = "", 
	   G4bool aperset = false, 
	   G4double aper = 0, 
	   G4double tunnelOffsetX = BDSGlobalConstants::Instance()->GetTunnelOffsetX(), 
	   G4double phiAngleIn = 0, 
	   G4double phiAngleOut = 0);
  ~BDSDrift();

protected:
  void BuildBpFieldAndStepper();
  //field related objects
  G4MagIntegratorStepper* itsStepper;
  //BDSDriftStepper* itsStepper;
  BDSMagField* itsMagField;
  G4Mag_UsualEqRhs* itsEqRhs;
private:
  void BuildBLMs();
  G4VisAttributes* SetVisAttributes();
  G4double itsStartOuterR;
  G4double itsEndOuterR;
};

#endif
