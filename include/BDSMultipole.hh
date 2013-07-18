/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Added extra parameter to BuildDefaultOuterLogicalVolume so that it is 
     possible to set the material as either Iron or Vacuum
   Removed StringFromInt function
*/

#ifndef BDSMultipole_h
#define BDSMultipole_h 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"

#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4Mag_EqRhs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4MagneticField.hh"
#include "G4PVPlacement.hh"               
#include "G4EllipticalTube.hh"
#include "G4SubtractionSolid.hh"

class BDSMultipole :public BDSAcceleratorComponent
{
public:
  
  BDSMultipole( G4String aName, 
		G4double aLength,
		G4double aBpRadius,
		G4double aInnerIronRadius,
		G4VisAttributes* aVisAtt,
		G4String aMaterial = "",
		G4double aXAper=0.,
		G4double aYAper=0.,
		G4double angle=0.,
		G4bool beampipeThicknessSet=false,
		G4double beampipeThickness=-1);

  //Contructor for components with tunnel material added
  BDSMultipole( G4String aName, 
		G4double aLength,
		G4double aBpRadius,
		G4double aInnerIronRadius,
		G4VisAttributes* aVisAtt,
                G4String aTunnelMaterial = "",
                G4String aMaterial="",
		G4double aXAper=0.,
		G4double aYAper=0.,
		G4double angle=0.,
		G4double tunnelRadius=0.,
		G4double tunnelOffsetX=BDSGlobalConstants::Instance()->GetTunnelOffsetX(),
		G4bool beampipeThicknessSet=false,
		G4double beampipeThickess=-1);


  //Contructor for components with blms and tunnel material added
  BDSMultipole( G4String aName, 
		G4double aLength,
		G4double aBpRadius,
		G4double aInnerIronRadius,
		G4VisAttributes* aVisAtt,
                std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                G4String aTunnelMaterial = "",
                G4String aMaterial="",
		G4double aXAper=0.,
		G4double aYAper=0.,
		G4double angle=0.,
		G4double tunnelRadius=0.,
		G4double tunnelOffsetX=BDSGlobalConstants::Instance()->GetTunnelOffsetX(),
		G4double phiAngleIn=0,
		G4double phiAngleOut=0,
		G4bool beampipeThicknessSet=false,
		G4double beampipeThickness=-1);

  virtual ~BDSMultipole();

  void BuildBLMs();

  virtual void BuildBeampipe(G4String materialName = ""); //Standard beam pipe, cross section is elliptical (or circular)

  void BuildBeampipe(G4double startAper, G4double endAper, G4String materialName = ""); //Builds a tapered beam pipe

  void BuildDefaultMarkerLogicalVolume();
 
  void BuildDefaultOuterLogicalVolume(G4double aLength,G4bool OuterMaterialIsVacuum=false);

  void BuildEllipticalOuterLogicalVolume(G4double aLength,G4bool OuterMaterialIsVacuum=false);

  void BuildBPFieldMgr(G4MagIntegratorStepper* aStepper,
		       G4MagneticField* aField);

  G4FieldManager* GetBPFieldMgr();

  void BuildOuterFieldManager(G4int nPoles, G4double poleField, 
			      G4double phiOffset);

  void SetOuterRadius(G4double outR);
  void SetStartOuterRadius(G4double outR);
  void SetEndOuterRadius(G4double outR);

protected:
  G4LogicalVolume* itsBeampipeLogicalVolume;
  G4LogicalVolume* itsInnerBPLogicalVolume;

  //-----------------------------
  G4VisAttributes* itsVisAttributes;
  G4UserLimits* itsBeampipeUserLimits;
  G4VPhysicalVolume* itsPhysiComp;
  G4VPhysicalVolume* itsPhysiInner;
  G4FieldManager* itsBPFieldMgr;
  G4FieldManager* itsOuterFieldMgr;

protected:   // these might need to be accessed from the child classes
  G4double itsInnerIronRadius;
  G4double itsBeampipeThickness;
  
  G4VSolid* itsBeampipeSolid;
  G4VSolid* itsInnerBeampipeSolid;

  G4ChordFinder* itsChordFinder;
  G4MagneticField* itsOuterMagField;

  // G4double itsStartOuterR;
  // G4double itsEndOuterR;

private:
  /// constructor initialisation
  void ConstructorInit();

  void SetBeampipeThickness(G4bool, G4double);

};

inline G4FieldManager* BDSMultipole::GetBPFieldMgr()
{return itsBPFieldMgr;}

inline void BDSMultipole::SetOuterRadius(G4double outR)
{itsOuterR = outR;}

inline void BDSMultipole::SetStartOuterRadius(G4double outR)
{itsOuterR = outR;}

inline void BDSMultipole::SetEndOuterRadius(G4double outR)
{itsOuterR = outR;}

#endif
