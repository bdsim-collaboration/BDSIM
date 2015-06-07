/* BDSIM code.    Version 1.0
  
*/

#ifndef BDSCollimator_h
#define BDSCollimator_h 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"

#include "G4UserLimits.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"

class BDSCollimator :public BDSAcceleratorComponent
{
public:
  BDSCollimator(G4String aName, G4double aLength,G4double bpRad, 
                G4double xAper, G4double yAper,G4Material *collimatorMaterial, G4double outR, 
                std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
G4String aTunnelMaterial="");
  ~BDSCollimator();

protected:
  virtual void Build();

private:
  virtual void BuildMarkerLogicalVolume();
  virtual void BuildBLMs();
  void BuildInnerCollimator();

  virtual void SetVisAttributes();

  // Geometrical objects:

  G4VPhysicalVolume* itsPhysiComp;
  G4VPhysicalVolume* itsPhysiComp2;
  G4LogicalVolume* itsSolidLogVol;
  G4LogicalVolume* itsInnerLogVol;

  G4VSolid* itsInnerSolid;
  G4VSolid* itsOuterSolid;

  G4Material* itsCollimatorMaterial;
  G4double itsOuterR;
};

#endif
