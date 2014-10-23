#ifndef BDS_TUNNEL_HH
#define BDS_TUNNEL_HH

#include "G4LogicalVolume.hh"
#include <stdexcept>
#include "BDSAcceleratorComponent.hh"

class BDSTunnel{
public:
  BDSTunnel(BDSAcceleratorComponent* val);
  ~BDSTunnel();

  //  GLogicalVolume* logicalVolume();
  //  void offsetX(G4double val);
  //  void radius(G4double val);
  //  void floorBeamlineHeight(G4double val);
  //  void beamlineCeilingHeight(G4double val);

  //void motherComponent(BDSAcceleratorComponent* val);  

  virtual void Build();




private:
  BDSTunnel();
  void CheckExceptions();
  virtual void BuildSolidVolumes();
  virtual void PlaceLogicalVolumes();
  virtual void CalculateDimensions();
  void BuildLogicalVolumes();

  void defaults();
  G4LogicalVolume* _motherVolume;
  BDSAcceleratorComponent* _motherComponent;
  G4String _motherName;
  //Materials.
  G4Material* _material;
  G4Material* _soilMaterial;
  G4Material* _cavityMaterial;
  //Dimensions.  
  G4double _length;
  G4double _radius;
  G4double _offsetX;
  G4double _offsetY;
  G4double _floorBeamlineHeight;
  G4double _beamlineCeilingHeight;
    //Position/rotation
  G4RotationMatrix* _rot;
  G4ThreeVector _trans;
  G4ThreeVector _nullThreeVector;

  G4VSolid* _solid;
  G4VSolid* _soilSolid;
  G4VSolid* _innerSolid;
  G4VSolid*  _cavity;
  G4VSolid * _floor;
  G4VSolid * _tunnelWallsSolid;
  G4LogicalVolume* _logicalVolume;

  
  /// Tunnel logical volumes
  G4LogicalVolume* _soilLogicalVolume;
  G4LogicalVolume* _cavityLogicalVolume;
  G4LogicalVolume*  _tunnelWallsLogicalVolume;
  /// Tunnel physical volumes
  G4VPhysicalVolume* _tunnelPhysiInner;
  G4VPhysicalVolume* _tunnelPhysiComp;
  G4VPhysicalVolume* _tunnelPhysiCompSoil;
  /// Tunnel user limits
  G4UserLimits* _tunnelUserLimits;
  G4UserLimits* _soilUserLimits;
  G4UserLimits* _innerUserLimits;

};



#endif
