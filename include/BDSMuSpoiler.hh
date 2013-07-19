/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 12.12.2004
   Copyright (c) 2004 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSMuSpoiler_h
#define BDSMuSpoiler_h 1

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"

#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include "G4Box.hh"
//#include "BDSEnergyCounterSD.hh"
#include "BDSMuSpoilerMagField.hh"
#include "G4FieldManager.hh"

class BDSMuSpoiler :public BDSAcceleratorComponent
{
public:
  BDSMuSpoiler(G4String& aName, G4double aLength,G4double bpRad, 
	       G4double rInner, G4double rOuter,G4double BField, 
               std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
               G4String aTunnelMaterial="");


  ~BDSMuSpoiler();

protected:

private:
  G4VisAttributes* SetVisAttributes();
  G4VisAttributes* SetBPVisAttributes();

  // Geometrical objects:

  void BuildMuSpoiler();
  void BuildMuSpoilerTunnel();
  void BuildBLMs();

  G4VPhysicalVolume* itsPhysiComp;
  G4VPhysicalVolume* itsPhysiCompSoil;
  G4VPhysicalVolume* itsPhysiComp2;
  G4VPhysicalVolume* itsPhysiInnerBP;
  G4VPhysicalVolume* itsPhysiBP;
  G4LogicalVolume* itsSolidLogVol;
  G4LogicalVolume* itsInnerLogVol;
  G4LogicalVolume* itsBeampipeLogicalVolume;
  G4LogicalVolume* itsInnerBPLogicalVolume;
  G4Tubs* itsBPTube;
  G4Tubs* itsInnerBPTube;
   
  G4Tubs* itsSoilTube;
  G4Tubs* itsTunnelTube;
  G4Tubs* itsInnerTunnelTube;

  G4LogicalVolume* itsInnerTunnelLogicalVolume;
  G4LogicalVolume* itsSoilTunnelLogicalVolume;
  G4UserLimits* itsTunnelUserLimits;
  G4UserLimits* itsSoilTunnelUserLimits;
  G4UserLimits* itsInnerTunnelUserLimits;
                 
  G4VisAttributes* itsVisAttributes;
  G4VisAttributes* itsBPVisAttributes;
  G4Mag_UsualEqRhs* itsEqRhs;

private:
  //  BDSEnergyCounterSD* itsEnergyCounterSD;
  G4double itsBeampipeRadius;
  G4double itsInnerRadius;
  G4double itsOuterRadius;
  G4double itsBField;

  BDSMuSpoilerMagField* itsMagField;
  G4FieldManager* itsFieldMgr;
};

#endif
