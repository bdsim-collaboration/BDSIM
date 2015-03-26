#ifndef BDSACCELERATORMODEL_H
#define BDSACCELERATORMODEL_H

#include "globals.hh"         // geant4 globals / types
#include "G4VPhysicalVolume.hh"


class BDSAcceleratorModel {
public:
  static BDSAcceleratorModel* Instance();
  ~BDSAcceleratorModel();

  inline G4VPhysicalVolume* GetReadOutWorld();

private:
  BDSAcceleratorModel(); ///< default constructor is private as singleton

  static BDSAcceleratorModel* _instance;

  G4VPhysicalVolume* readOutWorld;
  
};

inline G4VPhysicalVolume* BDSAcceleratorModel::GetReadOutWorld()
{return readOutWorld;}

#endif
