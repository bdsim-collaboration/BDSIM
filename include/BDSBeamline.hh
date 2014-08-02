#ifndef __BDSBEAMLINE_H
#define __BDSBEAMLINE_H

#include <list>

#include "globals.hh"

#include "BDSAcceleratorComponent.hh"

class BDSBeamline{
public:

  static BDSBeamline* Instance();
  ~BDSBeamline();
  
  void addComponent(BDSAcceleratorComponent* var);
  void print();
  void printNavigation();
  BDSAcceleratorComponent* currentItem();
  BDSAcceleratorComponent* lastItem();
  BDSAcceleratorComponent* lastItem2();
  BDSAcceleratorComponent* firstItem();
  G4bool isLast();
  void first();
  bool isDone();
  void next();

  G4double s_total();

  G4RotationMatrix* rotation();
  G4RotationMatrix* rotationGlobal();
  G4ThreeVector* position();
  G4double       positionS();
  G4ThreeVector* positionStart();
  G4ThreeVector* positionEnd();
  G4ThreeVector* positionFromCurrentCenter();
  G4ThreeVector* GetLastPosition();
  G4ThreeVector* GetFirstPosition();

  G4int size();

private: 
  BDSBeamline();

  static BDSBeamline* _instance;
  std::list <BDSAcceleratorComponent*> _componentList;

  std::list<BDSAcceleratorComponent*>::const_iterator _iterComponent;
  std::list<BDSAcceleratorComponent*>::const_iterator _iterLastComponent;
  void setRefTransform(BDSAcceleratorComponent* var);

  /// assignment and copy constructor not implemented nor used
  BDSBeamline& operator=(const BDSBeamline&);
  BDSBeamline(BDSBeamline&);

  //Navigation 
  void doNavigation();
  G4ThreeVector* _localX; 
  G4ThreeVector* _localY; 
  G4ThreeVector* _localZ; 
  G4ThreeVector* _position;
  G4ThreeVector* _positionStart;
  G4ThreeVector* _positionEnd;
  G4ThreeVector* _positionFromCurrentCenter;
  //  G4double _positionS;//Position along the curvilinear coordinate "s"
  G4ThreeVector* _zHalfAngle;
  G4RotationMatrix* _rotationLocal;
  G4RotationMatrix* _rotationGlobal;
  G4RotationMatrix* _rotation;

  std::list <G4RotationMatrix*> _rotationList;
  std::list <G4RotationMatrix*> _rotationGlobalList;
  std::list <G4ThreeVector*> _positionList;
  std::list <G4ThreeVector*> _positionStartList;
  std::list <G4ThreeVector*> _positionEndList;
  std::list <G4ThreeVector*> _positionFromCurrentCenterList;
  std::list <G4double> _positionSList; //Position along the curvilinear coordinate "s"

  std::list<G4RotationMatrix*>::const_iterator  _iterRotation;
  std::list<G4RotationMatrix*>::const_iterator  _iterRotationGlobal;
  std::list<G4ThreeVector*>::const_iterator  _iterPosition;
  std::list<G4ThreeVector*>::const_iterator  _iterPositionStart;
  std::list<G4ThreeVector*>::const_iterator  _iterPositionEnd;
  std::list<G4ThreeVector*>::const_iterator  _iterPositionFromCurrentCenter;
  std::list<G4double>::const_iterator _iterPositionS; //Position along the curvilinear coordinate "s"
  G4double _s_local, _s_total;
};

#endif
