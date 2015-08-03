/* BDSIM code.

*/

#include "globals.hh"
#include "BDSMagField.hh"

BDSMagField::BDSMagField():rotation(nullptr)
{
}

BDSMagField::~BDSMagField()
{
}

G4bool BDSMagField::GetHasNPoleFields(){return 0;}
G4bool BDSMagField::GetHasUniformField(){return 0;}
G4bool BDSMagField::GetHasFieldMap(){return 0;}

G4bool   BDSMagField::DoesFieldChangeEnergy() const
{
  //default
  return false;
}



void BDSMagField::GetFieldValue(const G4double[4],G4double *Bfield ) const
{
  // default 0

  Bfield[0] = 0;
  Bfield[1] = 0;
  Bfield[2] = 0;
}

void BDSMagField::Prepare(G4VPhysicalVolume*)
{
  // default - do nothing
}

void BDSMagField::SetOriginRotation(G4RotationMatrix* rot)
{
  rotation = rot;
}

void BDSMagField::SetOriginRotation(G4RotationMatrix rot)
{
  rotation = new G4RotationMatrix(rot);
}

G4RotationMatrix BDSMagField::Rotation() const{
  return *rotation;
}

void BDSMagField::SetOriginTranslation(G4ThreeVector trans)
{
  translation = trans;
}

G4bool GetHasNPoleFields(){
  return false;
}

G4bool GetHasUniformField(){
  return false;
}

G4bool GetHasFieldMap(){
  return false;
}
