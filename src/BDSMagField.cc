/* BDSIM code.

*/

#include "globals.hh"
#include "BDSMagField.hh"
#include "BDSExecOptions.hh"
#include "BDSDebug.hh"

BDSMagField::BDSMagField():rotation(NULL), _isPrepared(false)
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

void BDSMagField::checkPrepared() const{
  G4cout << __METHOD_NAME__ << " - _isPrepared = " << _isPrepared << G4endl;
  //Field must be prepared!
  if(!_isPrepared){
    G4String exceptionString = __METHOD_NAME__ + "- field has not been prepared. Rotation not set. Aborting.";
    G4Exception(exceptionString.c_str(), "-1", FatalErrorInArgument, "");   
  }
}

void BDSMagField::SetFieldExtent(const G4ThreeVector& pmin, const G4ThreeVector& pmax){
  // default - do nothing.
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
