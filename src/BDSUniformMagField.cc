#include "BDSUniformMagField.hh"
#include "BDSDebug.hh"
#include <limits>
#include <typeinfo>

BDSUniformMagField::~BDSUniformMagField()
{;}

G4ThreeVector BDSUniformMagField::pmin() const{
  return _pmin;
}

void BDSUniformMagField::Defaults(){
  G4double dmin=std::numeric_limits<G4double>::min(), dmax=std::numeric_limits<G4double>::max();
  G4float fmin=std::numeric_limits<G4float>::min(), fmax=std::numeric_limits<G4float>::max();
  if(typeid(dmin)==typeid(_pmin.x())){
    _pmin.setX(dmin);
    _pmin.setY(dmin);
    _pmin.setZ(dmin);
    _pmax.setX(dmax);
    _pmax.setY(dmax);
    _pmax.setZ(dmax);
  } else if(typeid(fmin)==typeid(_pmin.x())){
    _pmin.setX(fmin);
    _pmin.setY(fmin);
    _pmin.setZ(fmin);
    _pmax.setX(fmax);
    _pmax.setY(fmax);
    _pmax.setZ(fmax);
  } else {
    G4Exception("Components of G4ThreeVector - expecting either double for float!","-1", FatalErrorInArgument, "");
  }
}

//Ovverride the constructor
BDSUniformMagField::BDSUniformMagField(const G4ThreeVector& fieldVector):G4UniformMagField(fieldVector){
  Defaults();
}

//Ovverride the constructor
BDSUniformMagField::BDSUniformMagField(G4double vField, G4double vTheta, G4double vPhi)
  :G4UniformMagField(vField, vTheta, vPhi){
  Defaults();
}

void BDSUniformMagField::Prepare(G4VPhysicalVolume *referenceVolume){
  _isPrepared=true;
  //  G4cout << __METHOD_NAME__ << " - _isPrepared = " << _isPrepared << G4endl;
  const G4RotationMatrix* Rot= referenceVolume->GetFrameRotation();
  const G4ThreeVector Trans=referenceVolume->GetFrameTranslation();
  SetOriginRotation(*Rot);
  SetOriginTranslation(Trans);
}

//Ovverride the GetFieldValue
void BDSUniformMagField::GetFieldValue(const G4double yTrack[4], G4double *MagField) const{
  //  G4cout << __METHOD_NAME__ << " - _isPrepared = " << _isPrepared << G4endl;
  //Default - field is zero.
  MagField[0]=0; MagField[1]=0; MagField[2]=0;

  if(_isPrepared){
    G4ThreeVector local;
    local[0] = yTrack[0] + translation[0];
    local[1] = yTrack[1] + translation[1];
    local[2] = yTrack[2] + translation[2];
    local *= Rotation();
    //Test if the track is within the field. If so, set the field.
    //  G4cout << __METHOD_NAME__ << " - ytrack = " << yTrack[0] << " "  << yTrack[1] << " "  << yTrack[2] << G4endl; 
    //  G4cout << __METHOD_NAME__ << " - pmin = " << _pmin[0] << " "  << _pmin[1] << " "  << _pmin[2] << G4endl; 
    //  G4cout << __METHOD_NAME__ << " - pmax = " << _pmax[0] << " "  << _pmax[1] << " "  << _pmax[2] << G4endl; 
    if(local[0]>_pmin.x() &&
       local[1]>_pmin.y() &&
       local[2]>_pmin.z() &&
       local[0]<_pmax.x() &&
       local[1]<_pmax.y() &&
       local[2]<_pmax.z()){
      G4UniformMagField::GetFieldValue(yTrack, MagField);  
    }
  }
  //  G4UniformMagField::GetFieldValue(yTrack, MagField);  
  //  G4cout << __METHOD_NAME__ << " - field = " << MagField[0] << " "  << MagField[1] << " "  << MagField[2] << G4endl; 
}

void BDSUniformMagField::SetFieldExtent(const G4ThreeVector& pmin, const G4ThreeVector& pmax){
  _pmin=pmin;
  _pmax=pmax;
}


