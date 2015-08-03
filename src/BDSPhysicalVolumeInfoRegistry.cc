#include "BDSDebug.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"

#include "globals.hh" // geant4 globals / types
#include "G4VPhysicalVolume.hh"

#include <map>

BDSPhysicalVolumeInfoRegistry* BDSPhysicalVolumeInfoRegistry::_instance = 0;

BDSPhysicalVolumeInfoRegistry* BDSPhysicalVolumeInfoRegistry::Instance()
{
  if (_instance == 0)
    {_instance = new BDSPhysicalVolumeInfoRegistry();}
  return _instance;
}

BDSPhysicalVolumeInfoRegistry::BDSPhysicalVolumeInfoRegistry()
{
  readOutSearch = readOutRegister.begin();
  backupSearch  = backupRegister.begin();
}

BDSPhysicalVolumeInfoRegistry::~BDSPhysicalVolumeInfoRegistry()
{
  BDSPVInfoIterator it = readOutRegister.begin();
  for (; it != readOutRegister.end(); ++it)
    {delete it->second;}
  it = backupRegister.begin();
  for (; it != backupRegister.end(); ++it)
    {delete it->second;}
  
  _instance = 0;
}

void BDSPhysicalVolumeInfoRegistry::RegisterInfo(G4VPhysicalVolume*     physicalVolume,
						 BDSPhysicalVolumeInfo* info,
						 G4bool                 isReadOutVolume,
						 G4bool                 isTunnel)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "registering volume " << physicalVolume->GetName() << G4endl;
  G4cout << __METHOD_NAME__ << "with info :        " << *info                     << G4endl;
  G4cout << __METHOD_NAME__ << "is read out volume " << isReadOutVolume           << G4endl;
#endif
  if (IsRegistered(physicalVolume))
    {//uh oh - we've found it somewhere - abort
      G4cerr << __METHOD_NAME__ << "this physical volume is already registered" << G4endl;
      G4cerr << __METHOD_NAME__ << "pv name is: " << physicalVolume->GetName() << G4endl;
      return;
    }

  // if it's a tunnel one, register and return
  if (isTunnel)
    {
      tunnelRegister[physicalVolume] = info;
      return;
    }
  // doesn't already exist so register it
  if (isReadOutVolume)
    {readOutRegister[physicalVolume] = info;}
  else
    {backupRegister[physicalVolume] = info;}
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "component registered" << G4endl;
#endif
}

BDSPhysicalVolumeInfo* BDSPhysicalVolumeInfoRegistry::GetInfo(G4VPhysicalVolume* physicalVolume,
							      G4bool             isTunnel)
{
  if (isTunnel)
    {
      tunnelSearch = tunnelRegister.find(physicalVolume);
      if (tunnelSearch == tunnelRegister.end())
	{return NULL;}
      else
	{return tunnelSearch->second;}
    }
  else if (IsRegisteredToReadOutRegister(physicalVolume))
    {return readOutSearch->second;}
  else if (IsRegisteredToBackupRegister(physicalVolume))
    {return backupSearch->second;}
  else
    {//uh oh - not found!
      G4cerr << __METHOD_NAME__ << "physical volume not found" << G4endl;
      G4cerr << __METHOD_NAME__ << "pv name is: " << physicalVolume->GetName() << G4endl;
      return nullptr;
    }
}

G4bool BDSPhysicalVolumeInfoRegistry::IsRegistered(G4VPhysicalVolume* physicalVolume)
{
  return (IsRegisteredToReadOutRegister(physicalVolume) || IsRegisteredToBackupRegister(physicalVolume));
}
  
G4bool BDSPhysicalVolumeInfoRegistry::IsRegisteredToReadOutRegister(G4VPhysicalVolume* physicalVolume)
{
  readOutSearch = readOutRegister.find(physicalVolume);
  if (readOutSearch == readOutRegister.end())
    {return false;}
  else
    {return true;}
}

G4bool BDSPhysicalVolumeInfoRegistry::IsRegisteredToBackupRegister(G4VPhysicalVolume* physicalVolume)
{
  backupSearch = backupRegister.find(physicalVolume);
  if (backupSearch == backupRegister.end())
    {return false;}
  else
    {return true;}
}

 G4bool BDSPhysicalVolumeInfoRegistry::IsRegisteredToTunnelRegister(G4VPhysicalVolume* physicalVolume)
 {
   tunnelSearch = tunnelRegister.find(physicalVolume);
   if (tunnelSearch == tunnelRegister.end())
     {return false;}
   else
     {return true;}
 }

std::ostream& operator<< (std::ostream& out, BDSPhysicalVolumeInfoRegistry const &r)
{
  out << "Physical Volume Registry:" << G4endl;
  out << "Read out volume register:" << G4endl;
  for (BDSPVInfoIteratorConst itRO = r.readOutRegister.begin(); itRO != r.readOutRegister.end(); ++itRO)
    {out << itRO->first->GetName() << "\t" << *(itRO->second) << G4endl;}
  out << G4endl << "General volume register:" << G4endl;
  for (BDSPVInfoIteratorConst itBU = r.backupRegister.begin(); itBU != r.backupRegister.end(); ++ itBU)
    {out << itBU->first->GetName() << "\t" << *(itBU->second) << G4endl;}
  return out;
}
