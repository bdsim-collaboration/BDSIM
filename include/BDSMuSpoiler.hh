/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 12.12.2004
   Copyright (c) 2004 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSMuSpoiler_h
#define BDSMuSpoiler_h 1

#include "globals.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSMultipole.hh"
#include "BDSMuSpoilerMagField.hh"

class BDSMuSpoiler :public BDSMultipole
{
public:
  BDSMuSpoiler(G4String     name,
	       G4double     length,
	       G4double     bField,
	       BDSBeamPipeInfo beamPipeInfo,
	       G4double     boxSize,
	       G4String     outerMaterial="",
	       G4String     tunnelMaterial="",
	       G4double     tunnelRadius=0,
	       G4double     tunnelOffsetX=0);
  ~BDSMuSpoiler();

private:
  G4double     itsBField;

  /// create the desired outer logical volume and attach the field to it
  virtual void BuildOuterLogicalVolume(bool outerMaterialsIsVacuum);
  /// build the necessary field for muspoiler
  virtual void BuildBPFieldAndStepper();
  virtual void SetVisAttributes();

  BDSMuSpoilerMagField* outerMagField;
  G4FieldManager*       outerFieldMgr;
};

#endif
