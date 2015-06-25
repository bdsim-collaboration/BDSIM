#include "BDSDebug.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"

#include "globals.hh" // geant4 globals / types - used for G4cerr

#include <map>

BDSPhysicalVolumeInfoRegistry* BDSPhysicalVolumeInfoRegistry::_instance = 0;

BDSPhysicalVolumeInfoRegistry* BDSPhysicalVolumeInfoRegistry::Instance()
{
  if (_instance == 0)
    {
      _instance = new BDSPhysicalVolumeInfoRegistry();
    }
  return _instance;
}

BDSPhysicalVolumeInfoRegistry::BDSPhysicalVolumeInfoRegistry()
{;}

BDSPhysicalVolumeInfoRegistry::~BDSPhysicalVolumeInfoRegistry()
{
  _instance = 0;
}

void BDSPhysicalVolumeInfoRegistry::RegisterInfo(G4PhysicalVolume*      pvPointer,
						 BDSPhysicalVolumeInfo* info)
{
  // check we're not double registering something!
  BDSPVInfoIterator search = theRegister.find(pvPointer);
  if (search != theRegister.end())
    {//uh oh - we've found it somewhere - abort
      G4cerr << __METHOD_NAME__ << "this physical volume is already registered" << G4endl;
      G4cerr << __METHOD_NAME__ << "pv name is: " << pvPointer->GetName() << G4endl;
      return;
    }

  // doesn't already exist so register it
  theRegister[lvPointer] = info;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "component registered" << G4endl;
#endif
}

BDSPhysicalVolumeInfo* BDSPhysicalVolumeInfoRegistry::GetInfo(G4PhysicalVolume* physicalVolume)
{
  // search for the logical volume pointer in question
  BDSPVInfoIterator search = theRegister.find(physicalVolume);
  if (search == theRegister.end())
    {//uh oh - not found!
      G4cerr << __METHOD_NAME__ << "physical volume not found" << G4endl;
      G4cerr << __METHOD_NAME__ << "pv name is: " << physicalVolume->GetName() << G4endl;
      return NULL;
    }
  
  //must've found it so access the map (safely) and return the object
  return theRegister[logicalVolume];
}
