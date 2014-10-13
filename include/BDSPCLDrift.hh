/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSPCLDrift_h
#define BDSPCLDrift_h 1

#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "G4FieldManager.hh"
//#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"               
#include "BDSMultipole.hh"

class G4VPhysicalVolume;

class BDSPCLDrift :public BDSMultipole
{
public:
  BDSPCLDrift(G4String aName, G4double aLength,
           std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta, 
	      G4double aperX, G4double aperyUp, G4double aperYDown, G4double aperDy,  G4String aTunnelMaterial="", G4double aper=0, G4double tunnelRadius=0., G4double tunnelOffsetX=BDSGlobalConstants::Instance()->GetTunnelOffsetX(), G4String Material="");
  ~BDSPCLDrift();

protected:

private:
  virtual void SetVisAttributes();
  virtual void BuildBLMs();
  virtual void BuildBeampipe(G4String materialName = "");
  /// field related objects
  virtual void BuildBPFieldAndStepper();

  G4double itsYAperUp, itsYAperDown, itsDyAper;

  G4VSolid* outer_solid;
  G4VSolid* inner_solid;

  G4LogicalVolume* itsOuterBeamPipeLogicalVolume;
  G4LogicalVolume* itsInnerBeamPipeLogicalVolume;
  
  G4VPhysicalVolume* itsPhysiInner;
  G4VPhysicalVolume* itsPhysiOuter;

  G4VisAttributes* itsBeampipeVisAtt;
  G4VisAttributes* itsInnerBeampipeVisAtt;

  //  G4UserLimits* itsBeampipeUserLimits;
  //  G4UserLimits* itsInnerBeampipeUserLimits;
};

#endif
