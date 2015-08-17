/* BDSIM code for LW Calorimeter.    Version 1.0
   Author: John Carter, Royal Holloway, Univ. of London.
   Last modified 26.7.2004
   Copyright (c) 2004 by J.C.Carter.  ALL RIGHTS RESERVED. 
*/
#ifndef BDSLWCalorimeter_h
#define BDSLWCalorimeter_h 1

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

class G4Box;
class G4FieldManager;
class G4LogicalVolume;
class G4Tubs;
class G4UserLimits;
class G4VPhysicalVolume;

class BDSLWCalorimeter: public BDSAcceleratorComponent
{
public:
  BDSLWCalorimeter(G4String         name,
		   G4double         length,
		   BDSBeamPipeInfo* beamPipeInfo,
		   G4int            precisionRegion = 0);
  ~BDSLWCalorimeter();

protected:
  virtual void Build();

  G4LogicalVolume* itsBeampipeLogicalVolume;
  G4LogicalVolume* itsInnerBPLogicalVolume;
  G4VPhysicalVolume* itsPhysiInner;
  G4VPhysicalVolume* itsPhysiComp;
  G4LogicalVolume* itsLWCalLogicalVolume;
  G4UserLimits* itsBeampipeUserLimits;
  G4FieldManager* itsBPFieldMgr;

private:
  virtual void SetVisAttributes();
  virtual void BuildMarkerLogicalVolume();
  void BuildBeampipe();
  void BuildCal(G4double aLength);

  G4Tubs* itsBPTube;
  G4Tubs* itsInnerBPTube;
  G4Box*  itsLWCal;
  G4VPhysicalVolume* itsPhysiLWCal;
};

#endif
