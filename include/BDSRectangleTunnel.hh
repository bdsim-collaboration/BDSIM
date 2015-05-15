#ifndef BDS_RECTANGLE_TUNNEL_HH
#define BDS_RECTANGLE_TUNNEL_HH
#include <stdexcept>
#include "G4LogicalVolume.hh"
#include "BDSTunnel.hh"
#include "../parser/element.h"

class BDSRectangleTunnel:public BDSTunnel{
public:
  BDSRectangleTunnel(Element val, G4double length, G4double angle);
  ~BDSRectangleTunnel();

protected:
  virtual void CalculateDimensions();
  virtual void BuildSolidVolumes();
private:
  G4double _floorCeilingHeight;
};



#endif
