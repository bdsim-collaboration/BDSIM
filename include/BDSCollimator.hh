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
#include "G4VisAttributes.hh"
#include "G4Box.hh"
#include "G4EllipticalTube.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"

class BDSCollimator :public BDSAcceleratorComponent
{
public:
  BDSCollimator(G4String aName, G4double aLength,G4double bpRad, 
                G4double xAper, G4double yAper,G4int type,G4Material *collimatorMaterial, G4double outR, 
                std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
G4String aTunnelMaterial="");
  ~BDSCollimator();

protected:

private:
  void BuildBLMs();
  void BuildInnerCollimator();
  void BuildCollimatorTunnel();
  G4VisAttributes* SetVisAttributes();

  // Geometrical objects:

  G4VPhysicalVolume* itsPhysiComp;
  G4VPhysicalVolume* itsPhysiComp2;
  G4LogicalVolume* itsSolidLogVol;
  G4LogicalVolume* itsTempSolidLogVol;
  G4LogicalVolume* itsInnerLogVol;

  G4VSolid* itsInnerSolid;
  G4VSolid* itsOuterSolid;
  G4SubtractionSolid* itsSolid;


  G4Tubs* itsSoilTube;
  G4Tubs* itsTunnelTube;
  G4Tubs* itsInnerTunnelTube;
  G4LogicalVolume* itsInnerTunnelLogicalVolume;
  G4LogicalVolume* itsSoilTunnelLogicalVolume;
  G4UserLimits* itsTunnelUserLimits;
  G4UserLimits* itsSoilTunnelUserLimits;
  G4UserLimits* itsInnerTunnelUserLimits;
                    
  G4UserLimits* itsUserLimits;
  G4VisAttributes* itsVisAttributes;
  G4Mag_UsualEqRhs* itsEqRhs;

private:
  G4Material* itsCollimatorMaterial;
  //BDSEnergyCounterSD* itsEnergyCounterSD;
  G4double itsOuterR;
};

#endif
