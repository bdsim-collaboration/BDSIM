/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Added extra parameter to BuildOuterLogicalVolume so that it is 
     possible to set the material as either Iron or Vacuum
   Removed StringFromInt function
*/

#ifndef BDSMultipole_h
#define BDSMultipole_h 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipeType.hh"
#include "BDSBeamPipe.hh"

#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4LogicalVolume.hh"
#include "G4MagneticField.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4Material.hh"
#include "G4RotationMatrix.hh"
#include "G4UserLimits.hh"


class BDSMultipole :public BDSAcceleratorComponent
{
public:
  BDSMultipole( G4String aName, 
		G4double aLength,
		G4double aBpRadius,
		G4double aInnerIronRadius,
		G4String aMaterial = "",
		G4double aXAper=0.,
		G4double aYAper=0.,
		G4double angle=0.,
		G4bool beampipeThicknessSet=false,
		G4double beampipeThickness=-1);
  
  /// Constructor for components with tunnel material added
  BDSMultipole( G4String aName, 
		G4double aLength,
		G4double aBpRadius,
		G4double aInnerIronRadius,
                G4String aTunnelMaterial = "",
                G4String aMaterial="",
		G4double aXAper=0.,
		G4double aYAper=0.,
		G4double angle=0.,
		G4double tunnelRadius=0.,
		G4double tunnelOffsetX=BDSGlobalConstants::Instance()->GetTunnelOffsetX(),
		G4bool beampipeThicknessSet=false,
		G4double beampipeThickess=-1);


  /// Constructor for components with blms and tunnel material added
  BDSMultipole( G4String aName, 
		G4double aLength,
		G4double aBpRadius,
		G4double aInnerIronRadius,
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

  // Constructor for new beampipe
  BDSMultipole( G4String        name, 
		G4double        length,
		BDSBeamPipeType beamPipeType,
		G4double        aper1,
		G4double        aper2,
		G4double        aper3,
		G4double        aper4,
		G4Material*     vacuumMaterial,
		G4double        beamPipeThickness,
		G4Material*     beamPipeMaterial,
		G4String        outerMaterial="",
		G4String        tunnelMaterial="",
		G4double        tunnelRadius=0,
		G4double        tunnelOffsetX=0);

  virtual ~BDSMultipole();

protected:
  virtual void Build();

private:
  virtual void BuildMarkerLogicalVolume();
  virtual void BuildOuterLogicalVolume(G4bool OuterMaterialIsVacuum=true);
  /// build and set field manager and chord finder
  void BuildBPFieldMgr(G4MagIntegratorStepper* aStepper,
		       G4MagneticField* aField);

  /// define field and stepper
  virtual void BuildBPFieldAndStepper()=0;

  /// build beam loss monitors
  virtual void BuildBLMs();

  /// Method for common parts of both Buildbeampipe methods
  void FinaliseBeampipe(G4String materialName = "",G4RotationMatrix* RotY=NULL);

protected:
  /// Standard beam pipe
  // protected since called by BDSDrift::BuildBeampipe, change to private in future whenever possible
  virtual void BuildBeampipe(G4String materialName = ""); 
  /// Builds a tapered beam pipe (only used for drifts at the moment)
  //void BuildBeampipe(G4double startAper, G4double endAper, G4String materialName = "");

  void BuildOuterFieldManager(G4int nPoles, G4double poleField, 
			      G4double phiOffset);

  void SetOuterRadius(G4double outR);
  void SetStartOuterRadius(G4double outR);
  void SetEndOuterRadius(G4double outR);

protected:
  // field related objects, set by BuildBPFieldAndStepper
  G4MagIntegratorStepper* itsStepper;
  G4MagneticField* itsMagField;
  G4Mag_UsualEqRhs* itsEqRhs;

  // beam pipe volumes
  G4LogicalVolume* itsBeampipeLogicalVolume;
  G4LogicalVolume* itsInnerBPLogicalVolume;

  //-----------------------------
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


  //for beampipe construction
  BDSBeamPipeType beamPipeType;
  G4double        aper1;
  G4double        aper2;
  G4double        aper3;
  G4double        aper4;
  G4Material*     vacuumMaterial;
  G4double        beamPipeThickness;
  G4Material*     beamPipeMaterial;

  //the constructed beampipe
  BDSBeamPipe*    beampipe;
  
  // G4double itsStartOuterR;
  // G4double itsEndOuterR;

private:
  /// constructor initialisation
  void ConstructorInit();

  void SetBeampipeThickness(G4bool, G4double);
};

inline void BDSMultipole::SetOuterRadius(G4double outR)
{itsOuterR = outR;}

inline void BDSMultipole::SetStartOuterRadius(G4double outR)
{itsOuterR = outR;}

inline void BDSMultipole::SetEndOuterRadius(G4double outR)
{itsOuterR = outR;}

#endif
