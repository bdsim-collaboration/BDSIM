/* BDSIM code.   
A scintillator screen.
Work in progress.  
*/

#ifndef BDSScintillatorScreen_h
#define BDSScintillatorScreen_h 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"
#include "BDSSamplerSD.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"

#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4Box.hh"
#include "G4EllipticalTube.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"

class BDSScintillatorScreen :public BDSAcceleratorComponent
{
public:
  BDSScintillatorScreen(G4String aName, G4double tScint, G4double angle);
  ~BDSScintillatorScreen();

protected:

private:
  void BuildScintillatorScreen();
  void BuildScintillatorScreenTunnel();
  G4VisAttributes* SetVisAttributes();
  void ComputeDimensions();
  void BuildMarkerVolume();
  void BuildCameraScoringPlane();
  void BuildScintillatorMaterial();
  void BuildScintillatorCompound();
  void BuildScintillatorOpticalProperties();
  void BuildFrontLayer();
  void BuildScintillatorLayer();
  void BuildBaseLayer();
  void BuildBackLayer();
  void BuildOpticalSurfaces();
  

  // Geometrical objects:
  G4VPhysicalVolume* itsFrontLayerPhys;
  G4VPhysicalVolume* itsScintillatorLayerPhys;
  G4VPhysicalVolume* itsBaseLayerPhys;
  G4VPhysicalVolume* itsBackLayerPhys;
  G4VPhysicalVolume* itsCameraScoringPlanePhys;

  G4LogicalVolume* itsFrontLayerLog;
  G4LogicalVolume* itsScintillatorLayerLog;
  G4LogicalVolume* itsBaseLayerLog;
  G4LogicalVolume* itsBackLayerLog;
  G4LogicalVolume* itsCameraScoringPlaneLog;

  G4VSolid* itsFrontLayerSolid;
  G4VSolid* itsScintillatorLayerSolid;
  G4VSolid* itsBaseLayerSolid;
  G4VSolid* itsBackLayerSolid;
  G4VSolid* itsCameraScoringPlaneSolid;



  G4LogicalVolume* itsInnerTunnelLogicalVolume;
  G4LogicalVolume* itsSoilTunnelLogicalVolume;
  G4UserLimits* itsTunnelUserLimits;
  G4UserLimits* itsSoilTunnelUserLimits;
  G4UserLimits* itsInnerTunnelUserLimits;
                    
  G4VisAttributes* itsVisAttributes;
  G4Mag_UsualEqRhs* itsEqRhs;
  
private:
  G4MaterialPropertiesTable* _mptScintillatorMaterial;

  G4Material* _frontLayerMaterial;
  G4Material* _scintillatorLayerMaterial;
  G4Material* _baseLayerMaterial;
  G4Material* _backLayerMaterial;
  //BDSEnergyCounterSD* itsEnergyCounterSD;
  G4double itsOuterR;

  G4double _screenWidth;
  G4double _screenHeight;
  G4double _screenAngle;

  G4RotationMatrix* _screenRotationMatrix;
  
  G4double _frontThickness;
  G4double _scintillatorThickness;
  G4double _baseThickness;
  G4double _backThickness;
  G4double _totalThickness;

  //scoring plane
  G4double _scoringPlaneThickness;

  G4double _xLength;
  G4double _yLength;

  G4VisAttributes* _visAttFront;
  G4VisAttributes* _visAttScint;
  G4VisAttributes* _visAttBase;

  G4String _scoringPlaneName;
  G4String _samplerName;
};

#endif
