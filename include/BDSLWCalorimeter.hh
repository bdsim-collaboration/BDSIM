#ifndef BDSLWCalorimeter_h
#define BDSLWCalorimeter_h 1

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

#include <vector>

class BDSBeamPipe;
class G4Box;
class G4LogicalVolume;
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

  G4LogicalVolume* lwCalLogicalVolume;

private:
  virtual void SetVisAttributes();
  virtual void BuildMarkerLogicalVolume();
  void BuildBeampipe();
  void BuildCal(G4double aLength);

  G4Box*  lwCal;
  G4VPhysicalVolume* physiLWCal;

  BDSBeamPipe* beampipe;
};

#endif
