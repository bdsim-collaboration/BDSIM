/* BDSIM code.

*/

#include "globals.hh"
#include "BDSField.hh"

BDSField::BDSField()
{
}

BDSField::~BDSField()
{ 
}

G4bool   BDSField::DoesFieldChangeEnergy() const
{
  //default
  return false;
}



void BDSField::GetFieldValue(const G4double[4],G4double *Bfield ) const
{
  // default 0

  Bfield[0] = 0;
  Bfield[1] = 0;
  Bfield[2] = 0;

  Bfield[3] = 0;
  Bfield[4] = 0;
  Bfield[5] = 0;

}

void BDSField::Prepare(G4VPhysicalVolume*)
{
  // default - do nothing
}

void BDSField::SetOriginRotation(G4RotationMatrix rot)
{
  rotation = new G4RotationMatrix(rot);
}

void BDSField::SetOriginTranslation(G4ThreeVector trans)
{
  translation = trans;
}

G4RotationMatrix BDSField::Rotation() const{
  return *rotation;
}
