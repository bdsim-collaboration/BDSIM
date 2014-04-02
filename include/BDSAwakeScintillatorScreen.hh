/* BDSIM code.   
A scintillator screen.
Work in progress.  
*/

#ifndef BDSAwakeScintillatorScreen_h
#define BDSAwakeScintillatorScreen_h 

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
#include "BDSAwakeMultilayerScreen.hh"
#include "BDSCCDCamera.hh"

class BDSAwakeScintillatorScreen :public BDSAcceleratorComponent
{
public:
  BDSAwakeScintillatorScreen(G4String aName, G4String material, G4double thickness);
  ~BDSAwakeScintillatorScreen();

protected:

private:
  void Build();
  void BuildCamera();
  void PlaceCamera();
  void BuildScreen();
  void PlaceScreen();
  void BuildAwakeScintillatorScreenTunnel();
  void BuildVacuumChamber1();
  void BuildVacuumChamber2();
  G4VisAttributes* SetVisAttributes();
  void ComputeDimensions();
  void BuildMarkerVolume();
  void BuildCameraScoringPlane();
  void BuildScreenScoringPlane();
  void BuildAwakeScintillatorMaterial();
  void BuildAwakeScintillatorCompound();
  void BuildAwakeScintillatorOpticalProperties();
  void BuildFrontLayer();
  void BuildAwakeScintillatorLayer();
  void BuildBaseLayer();
  void BuildBackLayer();
  void BuildOpticalSurfaces();
  

  // Geometrical objects:
  G4VPhysicalVolume* itsCameraScoringPlanePhys;
  G4VPhysicalVolume* itsScreenScoringPlanePhys;
  G4LogicalVolume* itsCameraScoringPlaneLog;
  G4LogicalVolume* itsCameraScoringPlaneLog2;
  G4LogicalVolume* itsCameraScoringPlaneLog3;
  G4LogicalVolume* itsCameraScoringPlaneLog4;
  G4LogicalVolume* itsCameraScoringPlaneLog5;
  G4LogicalVolume* itsCameraScoringPlaneLog6;
  G4LogicalVolume* itsScreenScoringPlaneLog;
  G4LogicalVolume* itsScreenScoringPlaneLog2;
  G4VSolid* itsCameraScoringPlaneSolid;
  G4VSolid* itsScreenScoringPlaneSolid;

  G4LogicalVolume* itsInnerTunnelLogicalVolume;
  G4LogicalVolume* itsSoilTunnelLogicalVolume;
  G4UserLimits* itsTunnelUserLimits;
  G4UserLimits* itsSoilTunnelUserLimits;
  G4UserLimits* itsInnerTunnelUserLimits;
                    
  G4VisAttributes* itsVisAttributes;
  G4Mag_UsualEqRhs* itsEqRhs;
  
private:
  G4double itsOuterR;

  G4RotationMatrix* _screenRotationMatrix;
  G4RotationMatrix* _vacRotationMatrix;

  //scoring plane
  G4double _totalThickness;
  G4double _screenThickness;
  G4double _screenHeight;
  G4double _screenWidth;
  G4double _scoringPlaneThickness;

  //Vacuum chamber dimensions
  G4double _vacWindowHeight;
  G4double _vacMylarThickness;
  G4double _vacKevlarThickness;
  G4double _vacThickness;
  G4double _vacHeight;
  G4double _vacInnerHeight;
  G4double _vacWidth;
  G4double _vacInnerWidth;
  G4double _vacLength;
  G4double _vacDispX;


  G4VisAttributes* _visAttFront;
  G4VisAttributes* _visAttScint;
  G4VisAttributes* _visAttBase;
  G4VisAttributes* _visAttSampler;

  G4String _scoringPlaneName;
  G4String _screenScoringPlaneName;
  G4String _screenSamplerName;
  G4String _screenSamplerName2;
  G4String _samplerName;
  G4String _samplerName2;
  G4String _samplerName3;
  G4String _samplerName4;
  G4String _samplerName5;
  G4String _samplerName6;


  BDSAwakeMultilayerScreen* _mlScreen;
  G4double _screenAngle;
  BDSCCDCamera* _camera;

  G4double _cameraScreenDist;
  G4double _thickness;
  G4String _material;
};

#endif
