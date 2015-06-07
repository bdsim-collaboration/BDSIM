/* BDSIM code.    Version 1.0
  transform3d
*/

#ifndef __BDSTransform3D_h
#define __BDSTransform3D_h

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

class BDSTransform3D : public BDSAcceleratorComponent
{
public:

  BDSTransform3D(G4String aName, G4double x,
		 G4double y, G4double z, G4double phi,
		 G4double theta, G4double psi);

  ~BDSTransform3D();
  
  
private:
  
  virtual void BuildContainerLogicalVolume(){;};
};

#endif
