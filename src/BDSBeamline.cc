#include "BDSBeamline.hh"
#include "G4AffineTransform.hh"

BDSBeamline* BDSBeamline::_instance = 0;

BDSBeamline* BDSBeamline::Instance(){
  if(_instance==0) {
    _instance = new BDSBeamline();
  }
  return _instance;

}

BDSBeamline::BDSBeamline(){
  _localX = new G4ThreeVector(1,0,0);
  _localY = new G4ThreeVector(0,1,0);
  _localZ = new G4ThreeVector(0,0,1);
  
  _rotationLocal = new G4RotationMatrix();
  _rotationGlobal = new G4RotationMatrix();
  _rotation = new G4RotationMatrix();
  
  _position = new G4ThreeVector(0,0,0);
  _positionStart = new G4ThreeVector(0,0,0);
  _positionEnd = new G4ThreeVector(0,0,0);
  _positionFromCurrentCenter = new G4ThreeVector(0,0,0);
  _zHalfAngle = new G4ThreeVector(0,0,0); 
}

BDSBeamline::~BDSBeamline(){
  for(first(); !isDone(); next()){
    delete *_iterComponent;
  }
  _componentList.clear();

  delete _localX;
  delete _localY;
  delete _localZ;
  delete _rotationLocal;
  delete _rotationGlobal;
  delete _rotation;
  delete _position;
  delete _positionStart;
  delete _positionEnd;
  delete _positionFromCurrentCenter;
  delete _zHalfAngle;
}

void BDSBeamline::doNavigation(){
  //Reset the local rotation matrix
  *_rotationLocal = G4RotationMatrix();
  _s_local   = lastItem()->GetArcLength();
  _s_total += _s_local;
  G4double angle=lastItem()->GetAngle();
  G4double theta=lastItem()->GetTheta();
  G4double phi=lastItem()->GetPhi();
  G4double psi=lastItem()->GetPsi();
  G4double length=lastItem()->GetZLength();
  
  if( lastItem()->GetType() == "transform3d"){
    _rotationGlobal->rotate(psi,_localZ);
    _rotationLocal->rotate(psi,_localZ);
    _rotationGlobal->rotate(phi,_localY);   
    _rotationLocal->rotate(phi,_localY);
    _rotationGlobal->rotate(theta,_localX);
    _rotationLocal->rotate(theta,_localX);
    _localX->rotate(psi,*_localZ);
    _localY->rotate(psi,*_localZ);
    _localX->rotate(phi,*_localY);
    _localZ->rotate(phi,*_localY);
    _localY->rotate(theta,*_localX);
    _localZ->rotate(theta,*_localX);
  }
  
  // define center of bended elements from the previos coordinate frame
  _zHalfAngle->setX(_localZ->x());
  _zHalfAngle->setY(_localZ->y());
  _zHalfAngle->setZ(_localZ->z());
  if( lastItem()->GetType() == "sbend" || lastItem()->GetType() == "rbend"  )
    _zHalfAngle->rotate(angle/2,*_localY);

  
  // target position - advance the coordinates

    *_positionStart = (*_positionEnd);
    *_position = (*_positionEnd) + (*_zHalfAngle) * ( length/2 );  // The target position is the centre of the component.
    *_positionEnd = (*_position) + (*_zHalfAngle) * ( length/2 );  // The end position of the component.
    *_positionFromCurrentCenter = (*_position) - (*_positionEnd)/2.0; //The position of the beam line component from the centre of the CURRENT beam line
  

  // rotate to the previous reference frame
  _rotation->transform(*_rotationGlobal);
  _rotation->invert();
  // recompute global rotation
  // define new coordinate system local frame	  


  
  // bends transform the coordinate system
  if( lastItem()->GetType() == "sbend" || lastItem()->GetType() == "rbend"){
    _rotationGlobal->rotate(angle,*_localY);
    _localX->rotate(angle,*_localY);
    _localZ->rotate(angle,*_localY);
    _rotationGlobal->rotate(theta,*_localX);
    _localY->rotate(theta,*_localX);
    _localZ->rotate(theta,*_localX);
    // bend trapezoids defined along z-axis
    _rotation->rotateY(-twopi/4-angle/2); 						
  } else {
    if (lastItem()->GetMarkerLogicalVolume()->GetSolid()->GetName().contains("trapezoid") ){
      _rotation->rotateY(-twopi/4); //Drift trapezoids defined along z axis 
    }
  }



  _positionList.push_back(new G4ThreeVector(*_position));
  _positionStartList.push_back(new G4ThreeVector(*_positionStart));
  _positionEndList.push_back(new G4ThreeVector(*_positionEnd));
  _positionFromCurrentCenterList.push_back(new G4ThreeVector(*_positionFromCurrentCenter));
  _positionSList.push_back(_s_total);
  _rotationList.push_back(new G4RotationMatrix(*_rotation));
  _rotationGlobalList.push_back(new G4RotationMatrix(*_rotationGlobal));
  G4cout << "BDSBeamline::addComponent: finished." << G4endl;

  printNavigation();
}

void BDSBeamline::addComponent(BDSAcceleratorComponent* var){
  //Add component to the beamline
  G4cout << "BDSBeamline: adding component " << G4endl;
  _componentList.push_back(var);
  G4cout << "BDSBeamline: finished adding component. Setting as current item. " << G4endl;
  _iterComponent=_componentList.end();
  G4cout << "BDSBeamline: Set current item. " << G4endl;
  //Do the navigation
  doNavigation();
    G4cout << "Printing name. " << G4endl;
  G4cout << "BDSBeamline: " << Instance()->lastItem()->GetName() << G4endl;
  //Update the reference transform
  setRefTransform(var);
}

void BDSBeamline::setRefTransform(BDSAcceleratorComponent* var){
  if(BDSGlobalConstants::Instance()->GetRefVolume()==var->GetName() && 
     BDSGlobalConstants::Instance()->GetRefCopyNo()==var->GetCopyNumber()){
    G4cout << "Setting new transform" <<G4endl;
    G4AffineTransform tf(rotationGlobal(),*positionStart());
    BDSGlobalConstants::Instance()->SetRefTransform(tf);
  }
}

void BDSBeamline::print(){
  for(first(); !isDone(); next()){
    G4cout << (*_iterComponent)->GetName() << G4endl;
    printNavigation();
  }


}

void BDSBeamline::printNavigation(){
  G4cout << "BDSBeamline: _position = " << *_position << G4endl;
  G4cout << "BDSBeamline: _rotation = " << *_rotation << G4endl;
}

BDSAcceleratorComponent* BDSBeamline::currentItem(){
  return *_iterComponent;
}

BDSAcceleratorComponent* BDSBeamline::lastItem(){
  _iterLastComponent=_componentList.end();
  _iterLastComponent--;
  return *_iterLastComponent;
}

G4int BDSBeamline::size(){
  return _componentList.size();
}

void BDSBeamline::first(){
  _iterComponent=_componentList.begin();
  //Navigation
  _iterRotation=_rotationList.begin();
  _iterRotationGlobal=_rotationGlobalList.begin();
  _iterPosition=_positionList.begin();
  _iterPositionStart=_positionStartList.begin();
  _iterPositionEnd=_positionEndList.begin();
  _iterPositionFromCurrentCenter=_positionFromCurrentCenterList.begin();
  _iterPositionS=_positionSList.begin();
}

bool BDSBeamline::isDone(){
  return (_iterComponent==_componentList.end());
}

void BDSBeamline::next(){
  _iterComponent++;
  //Navigation
  _iterRotation++;
  _iterRotationGlobal++;
  _iterPosition++;
  _iterPositionStart++;
  _iterPositionEnd++;
  _iterPositionFromCurrentCenter++;
  _iterPositionS++;

}

G4RotationMatrix* BDSBeamline::rotation(){
  return *_iterRotation;
}

G4RotationMatrix* BDSBeamline::rotationGlobal(){
  return *_iterRotationGlobal;
}

G4ThreeVector* BDSBeamline::position(){
  return *_iterPosition;
}

G4ThreeVector* BDSBeamline::positionStart(){
  return *_iterPositionStart;
}

G4ThreeVector* BDSBeamline::positionEnd(){
  return *_iterPositionEnd;
}

G4ThreeVector* BDSBeamline::positionFromCurrentCenter(){
  return *_iterPositionFromCurrentCenter;
}

G4double BDSBeamline::positionS(){
  return _positionS;
}

G4double BDSBeamline::s_total(){
  return _s_total;
}
