/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSMAGNET
#define BDSMAGNET 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"

#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4LogicalVolume.hh"
#include "G4MagneticField.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4RotationMatrix.hh"
#include "G4UserLimits.hh"

class BDSMagnet: public BDSAcceleratorComponent
{
public:
  /// Magnet base class constructor that is for basic geometric information rather
  /// magnetic field details, which are handled in the derived classes
  BDSMagnet(BDSMagnetType      type,
	    G4String           name, 
	    G4double           length,
	    BDSBeamPipeInfo*   beamPipeInfo,
	    BDSMagnetOuterInfo magnetOuterInfo);
  
  virtual ~BDSMagnet();

  ///@{ Magnet strengh parameter accessor - to be moved into its own class soon
  G4double GetK1();
  G4double GetK2();
  G4double GetK3();
  ///@}

private:
  /// build and set field manager and chord finder
  void BuildBPFieldMgr(G4MagIntegratorStepper* aStepper,
		       G4MagneticField* aField);

  /// define field and stepper
  virtual void BuildBPFieldAndStepper()=0;

  /// Method for common parts of both Buildbeampipe methods
  void FinaliseBeampipe(G4String materialName = "",G4RotationMatrix* RotY=nullptr);

protected:

  virtual void Build();
  virtual void BuildContainerLogicalVolume();
  
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

  ///@{ normal vector for faces when preparing solids
  G4ThreeVector inputface;
  G4ThreeVector outputface;
  ///@}
  
  // type
  BDSMagnetType itsType;
  
  // field related objects, set by BuildBPFieldAndStepper
  G4MagIntegratorStepper* itsStepper;
  G4MagneticField*   itsMagField;
  G4Mag_UsualEqRhs*  itsEqRhs;

  // beam pipe volumes
  G4LogicalVolume*   itsBeampipeLogicalVolume;
  G4LogicalVolume*   itsInnerBPLogicalVolume;
  
  G4UserLimits*      itsBeampipeUserLimits;
  G4VPhysicalVolume* itsPhysiComp;
  G4VPhysicalVolume* itsPhysiInner;
  G4FieldManager*    itsBPFieldMgr;
  G4FieldManager*    itsOuterFieldMgr;

  G4double itsInnerIronRadius;

  G4ChordFinder*     itsChordFinder;
  G4MagneticField*   itsOuterMagField;
  
  //for beampipe construction
  BDSBeamPipeInfo* beamPipeInfo;
  
  //the constructed beampipe
  BDSBeamPipe*    beampipe;

  //for outer volume construction
  G4double        outerDiameter;

  //the assembled outer logical volume
  BDSGeometryComponent* outer;

  //for the outer volume construction
  BDSMagnetOuterInfo itsMagnetOuterInfo;

  // Magnetic strength parameters
  G4double itsK1, itsK2, itsK3;
};

#endif
