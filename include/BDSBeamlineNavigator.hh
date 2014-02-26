#ifndef __BDSBEAMLINENAVIGATOR_H
#define __BDSBEAMLINENAVIGATOR_H

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

#include "BDSAcceleratorComponent.hh"

class BDSBeamlineNavigator : public std::vector<G4Transform3D*> {
public:
  BDSBeamlineNavigator();
  ~BDSBeamlineNavigator();
  
  void addComponent(BDSAcceleratorComponent* var);
  void print();

  G4RotationMatrix* rotation();
  G4RotationMatrix* rotationGlobal();
  G4ThreeVector* position();
  G4ThreeVector* positionStart();
  G4ThreeVector* positionEnd();
  G4ThreeVector* positionFromCurrentCenter();

  G4double s_total();

  void first();
  bool isDone();
  void next();

private:  
  /// assignment and copy constructor not implemented nor used
  BDSBeamlineNavigator& operator=(const BDSBeamlineNavigator&);
  BDSBeamlineNavigator(BDSBeamlineNavigator&);

  G4ThreeVector* _localX; 
  G4ThreeVector* _localY; 
  G4ThreeVector* _localZ; 
  G4ThreeVector* _position;
  G4ThreeVector* _positionStart;
  G4ThreeVector* _positionEnd;
  G4ThreeVector* _positionFromCurrentCenter;
  G4ThreeVector* _zHalfAngle;
  G4RotationMatrix* _rotationLocal;
  G4RotationMatrix* _rotationGlobal;
  G4RotationMatrix* _rotation;

  std::list<G4RotationMatrix*> _rotationList;
  std::list<G4RotationMatrix*> _rotationGlobalList;
  std::list<G4ThreeVector*> _positionList;
  std::list<G4ThreeVector*> _positionStartList;
  std::list<G4ThreeVector*> _positionEndList;
  std::list<G4ThreeVector*> _positionFromCurrentCenterList;
  
  std::list<G4RotationMatrix*>::const_iterator  _iterRotation;
  std::list<G4RotationMatrix*>::const_iterator  _iterRotationGlobal;
  std::list<G4ThreeVector*>::const_iterator  _iterPosition;
  std::list<G4ThreeVector*>::const_iterator  _iterPositionStart;
  std::list<G4ThreeVector*>::const_iterator  _iterPositionEnd;
  std::list<G4ThreeVector*>::const_iterator  _iterPositionFromCurrentCenter;

  G4double _s_local, _s_total;
};

#endif
