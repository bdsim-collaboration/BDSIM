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
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"
#include "BDSTunnelInfo.hh"

#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4LogicalVolume.hh"
#include "G4MagneticField.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4Material.hh"
#include "G4RotationMatrix.hh"
#include "G4UserLimits.hh"


class BDSMultipole: public BDSAcceleratorComponent
{
public:
  // Constructor for new beampipe
  BDSMultipole( G4String        name, 
		G4double        length,
		BDSBeamPipeInfo beamPipeInfo,
		G4double        boxSize,
		G4String        outerMaterial="",
		G4String        tunnelMaterial="",
		G4double        tunnelRadius=0,
		G4double        tunnelOffsetX=0);

  BDSMultipole(BDSMagnetType      type,
	       G4String           name, 
	       G4double           length,
	       BDSBeamPipeInfo    beamPipeInfo,
	       BDSMagnetOuterInfo magnetOuterInfo,
	       BDSTunnelInfo      tunnelInfo);
  
  virtual ~BDSMultipole();

protected:
  virtual void Build();

private:
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
  virtual void BuildMarkerLogicalVolume();
  
  /// method to create outer volume
  virtual void BuildOuterVolume();
  
  /// general straight beampipe - can be overloaded by derived classes
  virtual void BuildBeampipe();
  /// common tasks after the beampipe solids have been defined.
  /// derived classes that override BuildBeampipe implement this manually
  /// in the contents of their BuildBeampipe
  void BeamPipeCommonTasks();

  void BuildOuterFieldManager(G4int nPoles, G4double poleField, 
			      G4double phiOffset);

  void SetOuterRadius(G4double outR);
  void SetStartOuterRadius(G4double outR);
  void SetEndOuterRadius(G4double outR);

  // don't need but provide null implementation here so it needn't be
  // in the derived classes
  virtual void SetVisAttributes();

protected:
  // type
  BDSMagnetType itsType;
  
  // field related objects, set by BuildBPFieldAndStepper
  G4MagIntegratorStepper* itsStepper;
  G4MagneticField* itsMagField;
  G4Mag_UsualEqRhs* itsEqRhs;

  // beam pipe volumes
  G4LogicalVolume* itsBeampipeLogicalVolume;
  G4LogicalVolume* itsInnerBPLogicalVolume;
  
  G4UserLimits* itsBeampipeUserLimits;
  G4VPhysicalVolume* itsPhysiComp;
  G4VPhysicalVolume* itsPhysiInner;
  G4FieldManager* itsBPFieldMgr;
  G4FieldManager* itsOuterFieldMgr;

  G4double itsInnerIronRadius;
  
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

  //for outer volume construction
  G4double        boxSize;

  //the assembled outer logical volume
  BDSGeometryComponent* outer;

  //for the outer volume construction
  BDSMagnetOuterInfo itsMagnetOuterInfo;

  //for the tunnel construction
  BDSTunnelInfo itsTunnelInfo;

private:
  /// constructor initialisation
  void ConstructorInit();
};

inline void BDSMultipole::SetOuterRadius(G4double outR)
{itsOuterR = outR;}

inline void BDSMultipole::SetStartOuterRadius(G4double outR)
{itsOuterR = outR;}

inline void BDSMultipole::SetEndOuterRadius(G4double outR)
{itsOuterR = outR;}

#endif
