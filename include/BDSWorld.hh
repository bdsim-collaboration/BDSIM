#ifndef __BDSWORLD_H
#define __BDSWORLD_H

#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4UserLimits.hh"

class BDSWorld {
public:

  //  static BDSWorld* Instance();
  BDSWorld();  
  ~BDSWorld();
  G4VPhysicalVolume* physicalVolume();
  G4LogicalVolume* logicalVolume();
  G4UserLimits* userLimits();

private: 
  //  static BDSWorld* _instance;
  void build();
  void place();

  void calcSize();

  void constructSolid();
  void constructLogical();
  void setVisAttributes();
  void setUserLimits();

  G4ThreeVector* _size;

  G4VSolid* _solidVolume;
  G4LogicalVolume* _logicalVolume;
  G4VPhysicalVolume* _physicalVolume;

  G4UserLimits* _userLimits;
  G4VisAttributes* _visAttributes;

  G4Material* _material;
};

#endif
