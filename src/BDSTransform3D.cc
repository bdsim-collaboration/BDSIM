#include "BDSTransform3D.hh"

BDSTransform3D::BDSTransform3D(G4String name,
			       G4double dxIn,
			       G4double dyIn,
			       G4double dzIn,
			       G4double dPhiIn,
			       G4double dThetaIn,
			       G4double dPsiIn):
  BDSAcceleratorComponent(name,0,0,"transform3d"),
  dx(dxIn), dy(dyIn), dz(dzIn),
  dTheta(dThetaIn), dPsi(dPsiIn), dPhi(dPhiIn)
{;}

BDSTransform3D::~BDSTransform3D()
{;}
