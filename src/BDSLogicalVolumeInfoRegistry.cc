#include "BDSDebug.hh"
#include "BDSLogicalVolumeInfoRegistry.hh"

#include "globals.hh" // geant4 globals / types - used for G4cerr

#include <map>

BDSLogicalVolumeInfoRegistry* BDSLogicalVolumeInfoRegistry::_instance = 0;

BDSLogicalVolumeInfoRegistry* BDSLogicalVolumeInfoRegistry::Instance()
{
  if (_instance == 0)
    {
      _instance = new BDSLogicalVolumeInfoRegistry();
    }
  return _instance;
}

BDSLogicalVolumeInfoRegistry::BDSLogicalVolumeInfoRegistry()
{;}

BDSLogicalVolumeInfoRegistry::~BDSLogicalVolumeInfoRegistry()
{
  _instance = 0;
}

void BDSLogicalVolumeInfoRegistry::RegisterInfo(G4LogicalVolume* lvPointer,
						BDSLogicalVolumeInfo* info)
{
  // check we're not double registering something!
  BDSLVInfoIterator search = theRegister.find(lvPointer);
  if (search != theRegister.end())
    {//uh oh - we've found it somewhere - abort
      G4cerr << __METHOD_NAME__ << "this logical volume is already registered" << G4endl;
      G4cerr << __METHOD_NAME__ << "lv name is: " << lvPointer->GetName() << G4endl;
      return;
    }

  // doesn't already exist so register it
  theRegister[lvPointer] = info;
}

BDSLogicalVolumeInfo* BDSLogicalVolumeInfoRegistry::GetInfo(G4LogicalVolume* logicalVolume)
{
  // search for the logical volume pointer in question
  BDSLVInfoIterator search = theRegister.find(logicalVolume);
  if (search == theRegister.end())
    {//uh oh - not found!
      G4cerr << __METHOD_NAME__ << "logical volume not found" << G4endl;
      G4cerr << __METHOD_NAME__ << "lv name is: " << logicalVolume->GetName() << G4endl;
      return NULL;
    }
  
  //must've found it so access the map (safely) and return the object
  return theRegister[logicalVolume];
}
