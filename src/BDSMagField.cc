/* BDSIM code.

*/

#include "globals.hh"
#include "BDSMagField.hh"
#include "BDSExecOptions.hh"

BDSMagField::BDSMagField():rotation(NULL)
{
}

BDSMagField::BDSMagField(G4String bmap, G4double bmapZOffset):_bmap(bmap),_bmapZOffset(bmapZOffset)
{
  BDSMagField();
  ParseBMapFormatAndFile();  
}

void BDSMagField::ParseBMapFormatAndFile(){
  _bFormat="none";
  if(_bmap != ""){
    G4int pos = _bmap.find(":");
    if(pos<0) {
      G4cerr<<"WARNING: invalid B map reference format : "<<_bmap<<G4endl;
    }
    else {
      _bFormat = _bmap.substr(0,pos);
      _bFile = BDSExecOptions::Instance()->GetBDSIMPATH() + _bmap.substr(pos+1,_bmap.length() - pos); 
#ifdef BDSDEBUG
      G4cout << "BDSElement::PlaceComponents bmap file is " << _bFile << G4endl;
#endif
    }
  }
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
