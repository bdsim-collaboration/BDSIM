#include "BDSGeometryComponent.hh"

#include "BDSDebug.hh"

#include "globals.hh"              // geant4 globals / types
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include <utility>                 // for std::pair
#include <vector>

BDSGeometryComponent::BDSGeometryComponent(G4VSolid*        containerSolidIn,
					   G4LogicalVolume* containerLVIn):
  containerSolid(containerSolidIn),
  containerLogicalVolume(containerLVIn),
  placementOffset(G4ThreeVector(0,0,0))
{
  SetExtentX(0,0); // initialise only - unphysical - should be set by child class
  SetExtentY(0,0);
  SetExtentZ(0,0);
}

BDSGeometryComponent::BDSGeometryComponent(G4VSolid*                    containerSolidIn,
					   G4LogicalVolume*             containerLVIn,
					   std::pair<G4double,G4double> extentXIn,
					   std::pair<G4double,G4double> extentYIn,
					   std::pair<G4double,G4double> extentZIn,
					   G4ThreeVector                placementOffsetIn):
  containerSolid(containerSolidIn),
  containerLogicalVolume(containerLVIn),
  extentX(extentXIn),
  extentY(extentYIn),
  extentZ(extentZIn),
  placementOffset(placementOffsetIn)
{;}

BDSGeometryComponent::BDSGeometryComponent(BDSGeometryComponent& component):
  containerSolid(component.containerSolid),
  containerLogicalVolume(component.containerLogicalVolume),
  extentX(component.extentX),
  extentY(component.extentY),
  extentZ(component.extentZ),
  placementOffset(component.placementOffset)
{;}

BDSGeometryComponent::~BDSGeometryComponent()
{
  delete containerSolid;
  delete containerLogicalVolume;
}

void BDSGeometryComponent::RegisterLogicalVolume(G4LogicalVolume* logicalVolume, G4bool internalCheck)
{
  // only register it if it doesn't exist already
  // note search the vector each time something is added is quite computationally expensive
  // but will protect against resetting sensitivity and possibly seg faults by doulby registered
  // logical volumes.  Also, the number of volumes should be < 20 (at maximum) and is only done
  // once at construction time so not as bad as it could be.
  if (std::find(allLogicalVolumes.begin(), allLogicalVolumes.end(), logicalVolume) == allLogicalVolumes.end())
	   {
	     // not found so register it
	     allLogicalVolumes.push_back(logicalVolume);
	   }
  else if (internalCheck)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "just an internal to check that this logical volume was registered" << G4endl;
#endif
      return;
    }
#ifdef BDSDEBUG
  else
    {
      // found - so don't register it
      G4cout << __METHOD_NAME__ << "warning - logical volume \""
	     << logicalVolume->GetName()
	     << "\" already in this geometry component \"";
      if (containerSolid)
	{G4cout << containerSolid->GetName();}
      else
	{G4cout << " INVALID CONTAINER ";}
      G4cout << "\"" << G4endl;
    }
#endif
}

void BDSGeometryComponent::RegisterLogicalVolumes(std::vector<G4LogicalVolume*> logicalVolumes)
{
  std::vector<G4LogicalVolume*>::iterator it = logicalVolumes.begin();
  for (; it != logicalVolumes.end(); ++it)
    {
      RegisterLogicalVolume(*it);
    }
}

void BDSGeometryComponent::RegisterSensitiveVolume(G4LogicalVolume* sensitiveVolume)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // check the logical volume to which it pertains is registered in this component
  // otherwise, register it
  RegisterLogicalVolume(sensitiveVolume, true);
  
  // only register it if it doesn't exist already
  // note search the vector each time something is added is quite computationally expensive
  // but will protect against resetting sensitivity and possibly seg faults by doulby registered
  // logical volumes.  Also, the number of volumes should be < 20 (at maximum) and is only done
  // once at construction time so not as bad as it could be.
  if (std::find(allSensitiveVolumes.begin(), allSensitiveVolumes.end(), sensitiveVolume) == allSensitiveVolumes.end())
	   {
	     // not found so register it
	     allSensitiveVolumes.push_back(sensitiveVolume);
	   }
  else
    {
      // found - so don't register it
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "warning - sensitive volume \""
	     << sensitiveVolume->GetName()
	     << "\" alreay in this geometry component \"";
      if (containerSolid)
	{G4cout << containerSolid->GetName();}
      else
	{G4cout << " INVALID CONTAINER ";}
      G4cout << "\"" << G4endl;
#endif
    }
}

void BDSGeometryComponent::RegisterSensitiveVolumes(std::vector<G4LogicalVolume*> sensitiveVolumes)
{
  std::vector<G4LogicalVolume*>::iterator it = sensitiveVolumes.begin();
  for (; it != sensitiveVolumes.end(); ++it)
    {
      RegisterSensitiveVolume(*it);
    }
}
