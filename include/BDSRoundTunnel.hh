#ifndef BDS_ROUND_TUNNEL_HH
#define BDS_ROUND_TUNNEL_HH
#include <stdexcept>
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "BDSTunnel.hh"
#include "../parser/element.h"

class BDSRoundTunnel:public BDSTunnel{
public:
  BDSRoundTunnel(Element val);
  ~BDSRoundTunnel();



protected:
  virtual void CalculateDimensions();

private:
  BDSRoundTunnel();
  
  virtual void BuildSolidVolumes();
  virtual void BuildStraightSolids();
  virtual void BuildAngleSolids();


  G4VSolid* _tunnelSizedBlock;
  G4LogicalVolume* _floorLogicalVolume;
  G4VSolid* _largerInnerSolid; 
  G4VSolid * _largerCavity;
  G4ThreeVector _floorOffset;
  G4double _blockSize;
  
  
};



#endif
