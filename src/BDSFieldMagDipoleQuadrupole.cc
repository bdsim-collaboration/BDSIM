#include "BDSDebug.hh"
#include "BDSFieldMagDipoleQuadrupole.hh"
#include "BDSFieldMagQuadrupole.hh"
#include "BDSFieldMagDipole.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"


BDSFieldMagDipoleQuadrupole::BDSFieldMagDipoleQuadrupole(BDSMagnetStrength const* strength,
							 G4double          const  brho):
  quad(new BDSFieldMagQuadrupole(strength, brho)),
  dipole(new BDSFieldMagDipole(strength, brho))
{;}

BDSFieldMagDipoleQuadrupole::~BDSFieldMagDipoleQuadrupole()
{
  delete quad;
  delete dipole;
}

G4ThreeVector BDSFieldMagDipoleQuadrupole::GetField(const G4ThreeVector &position,
						    const G4double       t) const
{  
  G4ThreeVector   quadField = quad->GetFieldTransformed(position, t);
  G4ThreeVector dipoleField = dipole->GetFieldTransformed(position, t);
  return quadField + dipoleField;
}

void BDSFieldMagDipoleQuadrupole::SetTransform(const G4Transform3D& transformIn)
{
  quad->SetTransform(transformIn);
  dipole->SetTransform(transformIn);
}
