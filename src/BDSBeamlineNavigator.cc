#include "BDSBeamlineNavigator.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSDebug.hh"
#include "G4RotationMatrix.hh"

BDSBeamlineNavigator::BDSBeamlineNavigator():_s_total(0.0){
  _localX = new G4ThreeVector(1,0,0);
  _localY = new G4ThreeVector(0,1,0);
  _localZ = new G4ThreeVector(0,0,1);

  _rotationGlobal = new G4RotationMatrix();
  _rotation       = new G4RotationMatrix();

  _maximumExtentPositive = G4ThreeVector(1,1,1); //minimum world size
  _maximumExtentNegative = G4ThreeVector(1,1,1);
}

BDSBeamlineNavigator::~BDSBeamlineNavigator(){
  delete _localX;
  delete _localY;
  delete _localZ;
  delete _rotationGlobal;
  delete _rotation;

  // clear lists
  std::list<G4ThreeVector*>::iterator it = _positionList.begin();
  for (;it!=_positionList.end(); ++it) {delete *it;}
  it = _positionStartList.begin();
  for (;it!=_positionStartList.end(); ++it) {delete *it;}
  it = _positionEndList.begin();
  for (;it!=_positionEndList.end(); ++it) {delete *it;}
  it = _positionFromCurrentCenterList.begin();
  for (;it!=_positionFromCurrentCenterList.end(); ++it) {delete *it;}

  std::list<G4RotationMatrix*>::iterator rotit = _rotationList.begin();
  for (;rotit!=_rotationList.end(); ++rotit) {delete *rotit;}
  rotit = _rotationGlobalList.begin();
  for (;rotit!=_rotationGlobalList.end(); ++rotit) {delete *rotit;}
}

void BDSBeamlineNavigator::addComponent(BDSAcceleratorComponent* var){
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  
  G4double angle  = var->GetAngle();
  G4double theta  = var->GetTheta();
  G4double phi    = var->GetPhi();
  G4double psi    = var->GetPsi();
  G4double length = var->GetChordLength();
  G4double xsize  = var->GetXLength();
  G4double ysize  = var->GetYLength();
  G4ThreeVector size = G4ThreeVector(xsize,ysize,length*0.5); //all half lengths

  G4double spos   = _s_total + (length*0.5); //spos at centre of element
  _s_total       += length;
  
  if( var->GetType() == "transform3d"){
    _rotationGlobal->rotate(psi,_localZ);
    _rotationGlobal->rotate(phi,_localY);   
    _rotationGlobal->rotate(theta,_localX);
    _localX->rotate(psi,*_localZ);
    _localY->rotate(psi,*_localZ);
    _localX->rotate(phi,*_localY);
    _localZ->rotate(phi,*_localY);
    _localY->rotate(theta,*_localX);
    _localZ->rotate(theta,*_localX);
  }
  
  // define center of bended elements from the previous coordinate frame
  G4ThreeVector zHalfAngle;
  zHalfAngle.setX(_localZ->x());
  zHalfAngle.setY(_localZ->y());
  zHalfAngle.setZ(_localZ->z());
  if( fabs(angle) > 1e-12 )
    {
      zHalfAngle.rotate(0.5*angle,*_localY);
    }
  
  // target position - advance the coordinates
  G4ThreeVector positionStart(0,0,0);
  if (!_positionEndList.empty()) {
    positionStart = (*_positionEndList.back());
  }

  G4ThreeVector position = positionStart + zHalfAngle * ( length/2 );  // The target position is the centre of the component.
  G4ThreeVector positionEnd = position + zHalfAngle * ( length/2 );  // The end position of the component.
  G4ThreeVector positionFromCurrentCenter = position - positionEnd/2.0; //The position of the beam line component from the centre of the CURRENT beam line
  
  // rotate to the previous reference frame
  _rotation->transform(*_rotationGlobal);
  _rotation->invert();
  // recompute global rotation
  // define new coordinate system local frame	  
  
  // bends transform the coordinate system
  if( fabs(angle) > 1e-12 )
    {
      _rotationGlobal->rotate(angle,*_localY);
      _localX->rotate(angle,*_localY);
      _localZ->rotate(angle,*_localY);
      _rotationGlobal->rotate(theta,*_localX);
      _localY->rotate(theta,*_localX);
      _localZ->rotate(theta,*_localX);						
    }
  else if (var->GetType() != "transform3d" && 
	   var->GetMarkerLogicalVolume() && 
	   var->GetMarkerLogicalVolume()->GetSolid() && 
	   var->GetMarkerLogicalVolume()->GetSolid()->GetName().contains("trapezoid") )
    {
      _rotation->rotateY(-CLHEP::twopi/4); //Drift trapezoids defined along z axis 
    }

  // update extent of model for world volume definition
  // project size in global coordinates
  G4ThreeVector thisextentpos = position + size.transform(*_rotationGlobal); 
  G4ThreeVector thisextentneg = position - size.transform(*_rotationGlobal);
  // loop over each size and compare to cumulative extent
  for (int i=0; i<3; i++)
    {
      if (thisextentpos[i] > _maximumExtentPositive[i])
	{_maximumExtentPositive[i] = thisextentpos[i];}
      if (thisextentneg[i] < _maximumExtentNegative[i])
	{_maximumExtentNegative[i] = thisextentneg[i];}
    }
  //update S position information for this element
  var->SetSPos(spos);
  //update global s position max
  BDSGlobalConstants::Instance()->SetSMax(_s_total);
  
  _positionList.push_back(new G4ThreeVector(position));
  _positionStartList.push_back(new G4ThreeVector(positionStart));
  _positionEndList.push_back(new G4ThreeVector(positionEnd));
  _positionFromCurrentCenterList.push_back(new G4ThreeVector(positionFromCurrentCenter));
  _positionSList.push_back(_s_total);
  _rotationList.push_back(new G4RotationMatrix(*_rotation));
  _rotationGlobalList.push_back(new G4RotationMatrix(*_rotationGlobal));
  // update iterators
  _iterRotation=_rotationList.end();
  _iterRotationGlobal=_rotationGlobalList.end();
  _iterPosition=_positionList.end();
  _iterPositionStart=_positionStartList.end();
  _iterPositionEnd=_positionEndList.end();
  _iterPositionFromCurrentCenter=_positionFromCurrentCenterList.end();
  _iterPositionS=_positionSList.end();
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "finished." << G4endl;
  //print();
#endif
}

void BDSBeamlineNavigator::print()const{
  G4cout << "BDSBeamlineNavigator: _position = " << *position() << G4endl;
  G4cout << "BDSBeamlineNavigator: _rotation = " << *rotation() << G4endl;
}

void BDSBeamlineNavigator::first(){
  _iterRotation=_rotationList.begin();
  _iterRotationGlobal=_rotationGlobalList.begin();
  _iterPosition=_positionList.begin();
  _iterPositionStart=_positionStartList.begin();
  _iterPositionEnd=_positionEndList.begin();
  _iterPositionFromCurrentCenter=_positionFromCurrentCenterList.begin();
  _iterPositionS=_positionSList.begin();
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
  _iterPositionS++;
}

G4RotationMatrix* BDSBeamlineNavigator::rotation()const{
  return *_iterRotation;
}

G4RotationMatrix* BDSBeamlineNavigator::rotationGlobal()const{
  return *_iterRotationGlobal;
}

G4ThreeVector* BDSBeamlineNavigator::position()const{
  return *_iterPosition;
}

G4ThreeVector* BDSBeamlineNavigator::positionStart()const{
  return *_iterPositionStart;
}

G4ThreeVector* BDSBeamlineNavigator::positionEnd()const{
  return *_iterPositionEnd;
}

G4ThreeVector* BDSBeamlineNavigator::positionFromCurrentCenter()const{
  return *_iterPositionFromCurrentCenter;
}

G4double BDSBeamlineNavigator::positionS()const{
  return *_iterPositionS;
}

G4double BDSBeamlineNavigator::s_total()const{
  return _s_total;
}

G4ThreeVector* BDSBeamlineNavigator::GetLastPosition()const{
  return _positionEndList.back();
}

G4ThreeVector* BDSBeamlineNavigator::GetFirstPosition()const{
  return _positionStartList.front();
}

G4ThreeVector BDSBeamlineNavigator::GetMaximumExtentPositive()const{
  return _maximumExtentPositive;
}

G4ThreeVector BDSBeamlineNavigator::GetMaximumExtentNegative()const{
  return _maximumExtentNegative;
}
