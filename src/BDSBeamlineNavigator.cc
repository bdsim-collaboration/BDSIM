#include "BDSBeamlineNavigator.hh"
#include "BDSGlobalConstants.hh"
#include "G4AffineTransform.hh"
#include "G4RotationMatrix.hh"

BDSBeamlineNavigator::BDSBeamlineNavigator(){
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

BDSBeamlineNavigator::~BDSBeamlineNavigator(){
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

void BDSBeamlineNavigator::addComponent(BDSAcceleratorComponent* var){
  //Reset the local rotation matrix
  G4cout << "BDSBeamlineNavigator::addComponent: resetting rotation matrix" << G4endl;
  *_rotationLocal = G4RotationMatrix();

  G4cout << "BDSBeamlineNavigator::addComponent: component variables" << G4endl;
  _s_local   = var->GetArcLength();
  _s_total += _s_local;
  G4double angle=var->GetAngle();
  G4double theta=var->GetTheta();
  G4double phi=var->GetPhi();
  G4double psi=var->GetPsi();
  G4double length=var->GetZLength();
  
  if( var->GetType() == "transform3d"){
    G4cout << "BDSBeamlineNavigator::addComponent: doing transform3d" << G4endl;
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
  G4cout << "BDSBeamlineNavigator::addComponent: doing _zHalfAngle" << G4endl;
  if( var->GetType() == "sbend" || var->GetType() == "rbend"  )
    G4cout << "BDSBeamlineNavigator::addComponent: bend half angle" << G4endl;
    _zHalfAngle->rotate(angle/2,*_localY);

  
  // target position - advance the coordinates

    G4cout << "BDSBeamlineNavigator::addComponent: advancing coordinates" << G4endl;

    *_positionStart = (*_positionEnd);
    *_position = (*_positionEnd) + (*_zHalfAngle) * ( length/2 );  // The target position is the centre of the component.
    *_positionEnd = (*_position) + (*_zHalfAngle) * ( length/2 );  // The end position of the component.
    *_positionFromCurrentCenter = (*_position) - (*_positionEnd)/2.0; //The position of the beam line component from the centre of the CURRENT beam line
  

  // rotate to the previous reference frame
 G4cout << "BDSBeamlineNavigator::addComponent: rotating" << G4endl;
  _rotation->transform(*_rotationGlobal);
  _rotation->invert();
  // recompute global rotation
  // define new coordinate system local frame	  


  
  // bends transform the coordinate system
  if( var->GetType() == "sbend" || var->GetType() == "rbend"){
    G4cout << "BDSBeamlineNavigator::addComponent: bend transform" << G4endl;
    _rotationGlobal->rotate(angle,*_localY);
    _localX->rotate(angle,*_localY);
    _localZ->rotate(angle,*_localY);
    _rotationGlobal->rotate(theta,*_localX);
    _localY->rotate(theta,*_localX);
    _localZ->rotate(theta,*_localX);
    // bend trapezoids defined along z-axis
    _rotation->rotateY(-CLHEP::twopi/4-angle/2); 						
  } else {
    if (var->GetMarkerLogicalVolume()->GetSolid()->GetName().contains("trapezoid") ){
      G4cout << "BDSBeamlineNavigator::addComponent: trapezoid rotate" << G4endl;
      _rotation->rotateY(-CLHEP::twopi/4); //Drift trapezoids defined along z axis 
    }
  }



  G4cout << "BDSBeamlineNavigator::addComponent: appending lists" << G4endl;
  _positionList.push_back(new G4ThreeVector(*_position));
  _positionStartList.push_back(new G4ThreeVector(*_positionStart));
  _positionEndList.push_back(new G4ThreeVector(*_positionEnd));
  _positionFromCurrentCenterList.push_back(new G4ThreeVector(*_positionFromCurrentCenter));
  _rotationList.push_back(new G4RotationMatrix(*_rotation));
  _rotationGlobalList.push_back(new G4RotationMatrix(*_rotationGlobal));
  G4cout << "BDSBeamlineNavigator::addComponent: finished." << G4endl;
}


void BDSBeamlineNavigator::print(){
  G4cout << "BDSBeamlineNavigator: _position = " << *_position << G4endl;
  G4cout << "BDSBeamlineNavigator: _rotation = " << *_rotation << G4endl;
}

void BDSBeamlineNavigator::first(){
  _iterRotation=_rotationList.begin();
  _iterRotationGlobal=_rotationGlobalList.begin();
  _iterPosition=_positionList.begin();
  _iterPositionStart=_positionStartList.begin();
  _iterPositionEnd=_positionEndList.begin();
  _iterPositionFromCurrentCenter=_positionFromCurrentCenterList.begin();
}

bool BDSBeamlineNavigator::isDone(){
  return (_iterRotation==_rotationList.end());
}

void BDSBeamlineNavigator::next(){
  _iterRotation++;
  _iterRotationGlobal++;
  _iterPosition++;
  _iterPositionStart++;
  _iterPositionEnd++;
  _iterPositionFromCurrentCenter++;
}


G4RotationMatrix* BDSBeamlineNavigator::rotation(){
  return *_iterRotation;
}

G4RotationMatrix* BDSBeamlineNavigator::rotationGlobal(){
  return *_iterRotationGlobal;
}

G4ThreeVector* BDSBeamlineNavigator::position(){
  return *_iterPosition;
}

G4ThreeVector* BDSBeamlineNavigator::positionStart(){
  return *_iterPositionStart;
}

G4ThreeVector* BDSBeamlineNavigator::positionEnd(){
  return *_iterPositionEnd;
}

G4ThreeVector* BDSBeamlineNavigator::positionFromCurrentCenter(){
  return *_iterPositionFromCurrentCenter;
}

G4double BDSBeamlineNavigator::s_total(){
  return _s_total;
}
