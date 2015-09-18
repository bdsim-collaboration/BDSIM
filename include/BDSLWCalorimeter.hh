#ifndef BDSLWCalorimeter_h
#define BDSLWCalorimeter_h 1

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

#include <vector>

class BDSBeamPipe;

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

  /// Access all sensitive volumes belonging to this component including
  /// those belonging to the beam pipe
  virtual std::vector<G4LogicalVolume*> GetAllSensitiveVolumes() const;
  
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

  BDSBeamPipe* itsBeampipe;
};

#endif
