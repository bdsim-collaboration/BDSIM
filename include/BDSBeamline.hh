#ifndef __BDSBEAMLINE_H
#define __BDSBEAMLINE_H

#include <list>

#include "globals.hh"

#include "BDSAcceleratorComponent.hh"

class BDSBeamlineNavigator;

class BDSBeamline{
public:

  static BDSBeamline* Instance();
  ~BDSBeamline();
  
  void addComponent(BDSAcceleratorComponent* var);
  void print();
  BDSAcceleratorComponent* currentItem();
  BDSAcceleratorComponent* lastItem();
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

  /// navigator object
  BDSBeamlineNavigator* _navigator;

  void setRefTransform(BDSAcceleratorComponent* var);

  /// assignment and copy constructor not implemented nor used
  BDSBeamline& operator=(const BDSBeamline&);
  BDSBeamline(BDSBeamline&);
};

#endif
