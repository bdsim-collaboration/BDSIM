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
  void print()const;

  G4RotationMatrix* rotation()const;
  G4RotationMatrix* rotationGlobal()const;
  G4ThreeVector* position()const;
  G4double       positionS()const;
  G4ThreeVector* positionStart()const;
  G4ThreeVector* positionEnd()const;
  G4ThreeVector* positionFromCurrentCenter()const;
  G4ThreeVector* GetLastPosition()const;
  G4ThreeVector* GetFirstPosition()const;
  G4ThreeVector GetMaximumExtentPositive()const;
  G4ThreeVector GetMaximumExtentNegative()const;

  G4double s_total()const;

  /// for iterators. should be separate class perhaps
  void first();
  bool isDone();
  void next();

private:  
  /// assignment and copy constructor not implemented nor used
  BDSBeamlineNavigator& operator=(const BDSBeamlineNavigator&);
  BDSBeamlineNavigator(BDSBeamlineNavigator&);

  G4ThreeVector* _localX; ///> current local x direction
  G4ThreeVector* _localY; ///> current local y direction
  G4ThreeVector* _localZ; ///> current local z direction

  G4RotationMatrix* _rotationGlobal;
  G4RotationMatrix* _rotation;
  ///@{ maximum and minimum position, for world dimensions
  G4ThreeVector _maximumExtentPositive;
  G4ThreeVector _maximumExtentNegative;
  ///@}
  
  std::list<G4RotationMatrix*> _rotationList;
  std::list<G4RotationMatrix*> _rotationGlobalList;
  std::list<G4ThreeVector*> _positionList; ///> Centre global positions
  std::list<G4ThreeVector*> _positionStartList; ///> Start positions
  std::list<G4ThreeVector*> _positionEndList; ///> End positions
  /// The position of the beam line component from the centre of the CURRENT beam line
  std::list<G4ThreeVector*> _positionFromCurrentCenterList;
  std::list <G4double> _positionSList; ///> Position along the curvilinear coordinate "s"

  ///@{ iterator for corresponding list
  std::list<G4RotationMatrix*>::const_iterator  _iterRotation;
  std::list<G4RotationMatrix*>::const_iterator  _iterRotationGlobal;
  std::list<G4ThreeVector*>::const_iterator  _iterPosition;
  std::list<G4ThreeVector*>::const_iterator  _iterPositionStart;
  std::list<G4ThreeVector*>::const_iterator  _iterPositionEnd;
  std::list<G4ThreeVector*>::const_iterator  _iterPositionFromCurrentCenter;
  std::list<G4double>::const_iterator _iterPositionS;
  ///@}
  
  G4double _s_total; ///> beamline length
};

#endif
