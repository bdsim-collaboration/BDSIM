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

//#include "BDSAcceleratorComponent.hh"
//#include "G4Mag_UsualEqRhs.hh"
//#include "BDSMaterials.hh"
//#include "G4LogicalVolume.hh"

//#include "G4UserLimits.hh"
//#include "G4VisAttributes.hh"

//#include "G4Box.hh"
//#include "BDSMuSpoilerMagField.hh"
//#include "G4FieldManager.hh"

class BDSMuSpoiler :public BDSMultipole
{
public:
  /*
  BDSMuSpoiler(G4String& aName, G4double aLength,G4double bpRad, 
	       G4double rInner, G4double rOuter,G4double BField, 
               std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
               G4String aTunnelMaterial="");*/
  BDSMuSpoiler(G4String     name,
	       G4double     length,
	       G4double     bField,
	       beamPipeInfo beamPipeInfo,
	       G4double     boxSize,
	       G4String     outerMaterial="",
	       G4String     tunnelMaterial="",
	       G4double     tunnelRadius=0,
	       G4double     tunnelOffsetX=0);
  ~BDSMuSpoiler();

private:
  G4double     itsBField;

  /// override multipole build beampipe method as we don't want to set the field there
  virtual void BuildBeamPipe();
  /// create the desired outer logical volume and attach the field to it
  virtual void BuildOuterLogicalVolume(bool outerMaterialsIsVacuum);
  /// build the necessary field for muspoiler
  virtual void BuildBPFieldAndStepper();
  virtual void SetVisAttributes();

  virtual void BuildOuterVolume();

  BDSMuSpoilerMagField* outerMagField;
  G4FieldManager*       outerFieldMgr;

  //virtual void Build();
  //void SetBPVisAttributes();

  // Geometrical objects:


  //virtual void BuildMarkerLogicalVolume();

  //void         BuildBeampipe();
  //virtual void BuildBLMs();
  //void         BuildMuSpoiler();
  /*
  
  G4VPhysicalVolume* itsPhysiComp;
  G4VPhysicalVolume* itsPhysiComp2;
  G4VPhysicalVolume* itsPhysiInnerBP;
  G4VPhysicalVolume* itsPhysiBP;
  G4LogicalVolume*   itsSolidLogVol;
  G4LogicalVolume*   itsInnerLogVol;
  G4LogicalVolume*   itsBeampipeLogicalVolume;
  G4LogicalVolume*   itsInnerBPLogicalVolume;
  G4Tubs*            itsBPTube;
  G4Tubs*            itsInnerBPTube;                 
  G4VisAttributes*   itsBPVisAttributes;
  G4double           itsBeampipeRadius;
  G4double           itsInnerRadius;
  G4double           itsOuterRadius;

  BDSMuSpoilerMagField* itsMagField;
  G4FieldManager*    itsFieldMgr;
  */
};

#endif
