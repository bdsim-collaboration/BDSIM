#include "BDSDebug.hh"
#include "BDSBeamline.hh"
#include "G4AffineTransform.hh"
#include "BDSBeamlineNavigator.hh"

BDSBeamline* BDSBeamline::_instance = 0;

BDSBeamline* BDSBeamline::Instance(){
  if(_instance==0) {
    _instance = new BDSBeamline();
  }
  return _instance;
}

BDSBeamline::BDSBeamline(){
  _navigator = new BDSBeamlineNavigator();
}

BDSBeamline::~BDSBeamline(){
  for(first(); !isDone(); next()){
    delete *_iterComponent;
  }
  _componentList.clear();
  delete _navigator;
  _instance = 0;
}

void BDSBeamline::addComponent(BDSAcceleratorComponent* var){
  //Add component to the beamline
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " adding component" << G4endl;
#endif

  _componentList.push_back(var);
  _iterComponent=_componentList.end();

  //Do the navigation
  _navigator->addComponent(var);

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " last item is " << lastItem()->GetName() << G4endl;
#endif
  //Update the reference transform
  setRefTransform(var);
}

void BDSBeamline::setRefTransform(BDSAcceleratorComponent* var){
  if(BDSGlobalConstants::Instance()->GetRefVolume()==var->GetName() && 
     BDSGlobalConstants::Instance()->GetRefCopyNo()==var->GetCopyNumber()){
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "setting new transform" <<G4endl;
#endif
    G4AffineTransform tf(rotationGlobal(),*positionStart());
    BDSGlobalConstants::Instance()->SetRefTransform(tf);
  }
}

void BDSBeamline::print(){
  for(first(); !isDone(); next()){
    G4cout << (*_iterComponent)->GetName() << G4endl;
    _navigator->print();
  }
}

BDSAcceleratorComponent* BDSBeamline::currentItem(){
  return *_iterComponent;
}

BDSAcceleratorComponent* BDSBeamline::lastItem(){
  if (_componentList.empty()) return NULL;
  return _componentList.back();
}

BDSAcceleratorComponent* BDSBeamline::firstItem(){
  return _componentList.front();
}

G4bool BDSBeamline::isLast(){
  return (*_iterComponent == _componentList.back());
}

G4int BDSBeamline::size(){
  return _componentList.size();
}

void BDSBeamline::first(){
  _iterComponent=_componentList.begin();
  //Navigation
  _navigator->first();
}

bool BDSBeamline::isDone(){
  return (_iterComponent==_componentList.end());
}

void BDSBeamline::next(){
  _iterComponent++;
  //Navigation
  _navigator->next();
}

G4RotationMatrix* BDSBeamline::rotation(){
  return _navigator->rotation();
}

G4RotationMatrix* BDSBeamline::rotationGlobal(){
  return _navigator->rotationGlobal();
}

G4ThreeVector* BDSBeamline::position(){
  return _navigator->position();
}

G4ThreeVector* BDSBeamline::positionStart(){
  return _navigator->positionStart();
}

G4ThreeVector* BDSBeamline::positionEnd(){
  return _navigator->positionEnd();
}

G4ThreeVector* BDSBeamline::positionFromCurrentCenter(){
  return _navigator->positionFromCurrentCenter();
}

G4double BDSBeamline::positionS(){
  return _navigator->positionS();
}

G4double BDSBeamline::s_total(){
  return _navigator->s_total();
}

G4ThreeVector* BDSBeamline::GetLastPosition(){
  return _navigator->GetLastPosition();
}

G4ThreeVector* BDSBeamline::GetFirstPosition(){
  return _navigator->GetFirstPosition();
}

G4ThreeVector BDSBeamline::GetMaximumExtentPositive(){
  return _navigator->GetMaximumExtentPositive();
}

G4ThreeVector BDSBeamline::GetMaximumExtentNegative(){
  return _navigator->GetMaximumExtentNegative();
}
