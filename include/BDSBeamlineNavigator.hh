#ifndef __BDSBEAMLINENAVIGATOR_H
#define __BDSBEAMLINENAVIGATOR_H

#include <list>
#include <vector>

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

class BDSAcceleratorComponent;

class BDSBeamlineNavigator {
public:
  BDSBeamlineNavigator();
  ~BDSBeamlineNavigator();
  
  void addComponent(BDSAcceleratorComponent* var);
  void print();

  G4RotationMatrix* rotation();
  G4RotationMatrix* rotationGlobal();
  G4ThreeVector* position();
  G4double       positionS();
  G4ThreeVector* positionStart();
  G4ThreeVector* positionEnd();
  G4ThreeVector* positionFromCurrentCenter();
  G4ThreeVector* GetLastPosition();
  G4ThreeVector* GetFirstPosition();
  G4ThreeVector GetMaximumExtentPositive();
  G4ThreeVector GetMaximumExtentNegative();

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
  G4ThreeVector _maximumExtentPositive;
  G4ThreeVector _maximumExtentNegative;

  std::list<G4RotationMatrix*> _rotationList;
  std::list<G4RotationMatrix*> _rotationGlobalList;
  std::list<G4ThreeVector*> _positionList;
  std::list<G4ThreeVector*> _positionStartList;
  std::list<G4ThreeVector*> _positionEndList;
  std::list<G4ThreeVector*> _positionFromCurrentCenterList;
  std::list <G4double> _positionSList; //Position along the curvilinear coordinate "s"

  std::list<G4RotationMatrix*>::const_iterator  _iterRotation;
  std::list<G4RotationMatrix*>::const_iterator  _iterRotationGlobal;
  std::list<G4ThreeVector*>::const_iterator  _iterPosition;
  std::list<G4ThreeVector*>::const_iterator  _iterPositionStart;
  std::list<G4ThreeVector*>::const_iterator  _iterPositionEnd;
  std::list<G4ThreeVector*>::const_iterator  _iterPositionFromCurrentCenter;
  std::list<G4double>::const_iterator _iterPositionS; //Position along the curvilinear coordinate "s"

  G4double _s_total;
};

#endif
