#ifndef BDS_ROUND_TUNNEL_HH
#define BDS_ROUND_TUNNEL_HH
#include <stdexcept>
#include "G4LogicalVolume.hh"
#include "BDSTunnel.hh"

class BDSRoundTunnel:public BDSTunnel{
public:
  BDSRoundTunnel();
  ~BDSRoundTunnel();



protected:
  virtual void CalculateDimensions();

private:
  void BuildStraightSolids();
  void BuildAngleSolids();
  virtual void BuildSolidVolumes();
  virtual void BuildStraightSolids();
  virtual void BuildAngleSolids();


  G4CSGSolid* _tunnelSizedBlock;
  G4LogicalVolume* _floorLogicalVolume;
  G4VSolid* _largerInnerSolid; 
  G4VSolid * _largerCavity;
  G4VPhysicalVolume* _tunnelFloorPhysiComp;
  G4ThreeVector _floorOffset;
  G4doubel _blockSize;
  
  
};



#endif
