#ifndef BDS_SQUARE_TUNNEL_HH
#define BDS_SQUARE_TUNNEL_HH
#include <stdexcept>
#include "G4LogicalVolume.hh"
#include "BDSTunnel.hh"

class BDSSquareTunnel:public BDSTunnel{
public:
  BDSSquareTunnel();
  ~BDSSquareTunnel();

protected:
  virtual void CalculateDimensions();
  virtual void BuildSolidVolumes();
};



#endif
