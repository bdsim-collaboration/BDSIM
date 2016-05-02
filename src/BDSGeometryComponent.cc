#include "BDSGeometryComponent.hh"

#include "BDSDebug.hh"

#include "globals.hh"              // geant4 globals / types
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
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
  SetInnerExtentX(0,0);
  SetInnerExtentY(0,0);
  SetInnerExtentZ(0,0);
}

BDSGeometryComponent::BDSGeometryComponent(G4VSolid*                    containerSolidIn,
					   G4LogicalVolume*             containerLVIn,
					   std::pair<G4double,G4double> extentXIn,
					   std::pair<G4double,G4double> extentYIn,
					   std::pair<G4double,G4double> extentZIn,
					   G4ThreeVector                placementOffset):
  containerSolid(containerSolidIn),
  containerLogicalVolume(containerLVIn),
  extentX(extentXIn),
  extentY(extentYIn),
  extentZ(extentZIn),
  placementOffset(placementOffset)
{
  SetInnerExtentX(0,0); // initialise only - unphysical - should be set by child class
  SetInnerExtentY(0,0);
  SetInnerExtentZ(0,0);
}

BDSGeometryComponent::BDSGeometryComponent(G4VSolid*                    containerSolidIn,
					   G4LogicalVolume*             containerLVIn,
					   std::pair<G4double,G4double> extentXIn,
					   std::pair<G4double,G4double> extentYIn,
					   std::pair<G4double,G4double> extentZIn,
					   std::pair<G4double,G4double> innerExtentXIn,
					   std::pair<G4double,G4double> innerExtentYIn,
					   std::pair<G4double,G4double> innerExtentZIn,
					   G4ThreeVector                placementOffset):
  containerSolid(containerSolidIn),
  containerLogicalVolume(containerLVIn),
  extentX(extentXIn),
  extentY(extentYIn),
  extentZ(extentZIn),
  innerExtentX(innerExtentXIn),
  innerExtentY(innerExtentYIn),
  innerExtentZ(innerExtentZIn),
  placementOffset(placementOffset)
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
  for (auto daughter : allDaughters)
    {delete daughter;}

  for (auto solid : allSolids)
    {delete solid;}

  for (auto lv : allLogicalVolumes)
    {delete lv;}

  for (auto pv : allPhysicalVolumes)
    {delete pv;}

  for (auto rm : allRotationMatrices)
    {delete rm;}

  for (auto vis : allVisAttributes)
    {delete vis;}

  for (auto ul : allUserLimits)
    {delete ul;}
}

void BDSGeometryComponent::InheritExtents(BDSGeometryComponent* anotherComponent)
{
  SetExtentX(anotherComponent->GetExtentX());
  SetExtentY(anotherComponent->GetExtentY());
  SetExtentZ(anotherComponent->GetExtentZ());
}

void BDSGeometryComponent::RegisterDaughter(BDSGeometryComponent* anotherComponent)
{
  if (std::find(allDaughters.begin(), allDaughters.end(), anotherComponent) == allDaughters.end())
    {allDaughters.push_back(anotherComponent);}
}

void BDSGeometryComponent::RegisterSolid(G4VSolid* solid, G4bool internalCheck)
{
  if (std::find(allSolids.begin(), allSolids.end(), solid) == allSolids.end())
    {allSolids.push_back(solid);} // not found so register it
  else if (internalCheck)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "just an internal to check that this solid was registered" << G4endl;
#endif
      return;
    }
#ifdef BDSDEBUG
  else
    {
      // found - so don't register it
      G4cout << __METHOD_NAME__ << "warning - solid \""
	     << solid->GetName()
	     << "\" already in this geometry component \"";
      if (containerSolid)
	{G4cout << containerSolid->GetName();}
      else
	{G4cout << " INVALID CONTAINER ";}
      G4cout << "\"" << G4endl;
    }
#endif
}

void BDSGeometryComponent::RegisterSolid(std::vector<G4VSolid*> solids)
{
  for (auto solid : solids)
    {RegisterSolid(solid);}
}

void BDSGeometryComponent::RegisterLogicalVolume(G4LogicalVolume* logicalVolume, G4bool internalCheck)
{
  // only register it if it doesn't exist already
  // note search the vector each time something is added is quite computationally expensive
  // but will protect against resetting sensitivity and possibly seg faults by doulby registered
  // logical volumes.  Also, the number of volumes should be < 20 (at maximum) and is only done
  // once at construction time so not as bad as it could be.
  if (std::find(allLogicalVolumes.begin(), allLogicalVolumes.end(), logicalVolume) == allLogicalVolumes.end())
    {allLogicalVolumes.push_back(logicalVolume);} // not found so register it
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

void BDSGeometryComponent::RegisterLogicalVolume(std::vector<G4LogicalVolume*> logicalVolumes)
{
  for (auto lv : logicalVolumes)
    {RegisterLogicalVolume(lv);}
}

void BDSGeometryComponent::RegisterPhysicalVolume(G4VPhysicalVolume* physicalVolume, G4bool internalCheck)
{
  // only register it if it doesn't exist already
  if (std::find(allPhysicalVolumes.begin(), allPhysicalVolumes.end(), physicalVolume) == allPhysicalVolumes.end())
    {allPhysicalVolumes.push_back(physicalVolume);} // not found so register it
  else if (internalCheck)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "just an internal to check that this physical volume was registered" << G4endl;
#endif
      return;
    }
#ifdef BDSDEBUG
  else
    {
      // found - so don't register it
      G4cout << __METHOD_NAME__ << "warning - physical volume \""
	     << physicalVolume->GetName()
	     << "\" already in this geometry component \"";
      if (containerSolid)
	{G4cout << containerSolid->GetName();}
      else
	{G4cout << " INVALID CONTAINER ";}
      G4cout << "\"" << G4endl;
    }
#endif
}

void BDSGeometryComponent::RegisterPhysicalVolume(std::vector<G4VPhysicalVolume*> physicalVolumes)
{
  std::vector<G4VPhysicalVolume*>::iterator it = physicalVolumes.begin();
  for (; it != physicalVolumes.end(); ++it)
    {RegisterPhysicalVolume(*it);}
}

void BDSGeometryComponent::RegisterRotationMatrix(G4RotationMatrix* rotationMatrix, G4bool internalCheck)
{
  // only register it if it doesn't exist already
  if (std::find(allRotationMatrices.begin(), allRotationMatrices.end(), rotationMatrix) == allRotationMatrices.end())
    {allRotationMatrices.push_back(rotationMatrix);} // not found so register it
  else if (internalCheck)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "just an internal to check that this rotation Matrix was registered" << G4endl;
#endif
      return;
    }
#ifdef BDSDEBUG
  else
    {
      // found - so don't register it
      G4cout << __METHOD_NAME__ << "warning - rotation matrix:"
	     << *rotationMatrix << "already registered in this geometry component \"";
      if (containerSolid)
	{G4cout << containerSolid->GetName();}
      else
	{G4cout << " INVALID CONTAINER ";}
      G4cout << "\"" << G4endl;
    }
#endif
}

void BDSGeometryComponent::RegisterRotationMatrix(std::vector<G4RotationMatrix*> rotationMatrices)
{
  for (auto rm : rotationMatrices)
    {RegisterRotationMatrix(rm);}
}

void BDSGeometryComponent::RegisterSensitiveVolume(G4LogicalVolume* sensitiveVolume)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__;
  if (sensitiveVolume)
    {G4cout << sensitiveVolume->GetName();}
  G4cout << G4endl;
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
    {allSensitiveVolumes.push_back(sensitiveVolume);} // not found so register it
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

void BDSGeometryComponent::RegisterSensitiveVolume(std::vector<G4LogicalVolume*> sensitiveVolumes)
{
  for (auto sv : sensitiveVolumes)
    {RegisterSensitiveVolume(sv);}
}

void BDSGeometryComponent::RegisterVisAttributes(G4VisAttributes* visAttribute, G4bool internalCheck)
{
  if (std::find(allVisAttributes.begin(), allVisAttributes.end(), visAttribute) == allVisAttributes.end())
    {allVisAttributes.push_back(visAttribute);} // not found so register it
  else if (internalCheck)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "just an internal to check that this visualisation attribute was registered" << G4endl;
#endif
      return;
    }
#ifdef BDSDEBUG
  else
    {
      // found - so don't register it
      G4cout << __METHOD_NAME__ << "warning - visualisation attribute: "
	     << visAttribute
	     << " already in this geometry component \"";
      if (containerSolid)
	{G4cout << containerSolid->GetName();}
      else
	{G4cout << " INVALID CONTAINER ";}
      G4cout << "\"" << G4endl;
    }
#endif
}

void BDSGeometryComponent::RegisterVisAttributes(std::vector<G4VisAttributes*> visAttributes)
{
  for (auto va : visAttributes)
    {RegisterVisAttributes(va);}
}

void BDSGeometryComponent::RegisterUserLimits(G4UserLimits* userLimit, G4bool internalCheck)
{
  if (std::find(allUserLimits.begin(), allUserLimits.end(), userLimit) == allUserLimits.end())
    {allUserLimits.push_back(userLimit);} // not found so register it
  else if (internalCheck)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "just an internal to check that this user limit was registered" << G4endl;
#endif
      return;
    }
#ifdef BDSDEBUG
  else
    {
      // found - so don't register it
      G4cout << __METHOD_NAME__ << "warning - user limits: "
	     << userLimit
	     << " already in this geometry component \"";
      if (containerSolid)
	{G4cout << containerSolid->GetName();}
      else
	{G4cout << " INVALID CONTAINER ";}
      G4cout << "\"" << G4endl;
    }
#endif
}

void BDSGeometryComponent::RegisterUserLimits(std::vector<G4UserLimits*> userLimits)
{
  for (auto ul : userLimits)
    {RegisterUserLimits(ul);}
}

void BDSGeometryComponent::InheritObjects(BDSGeometryComponent* component)
{
  RegisterSolid(component->GetAllSolids());
  RegisterLogicalVolume(component->GetAllLogicalVolumes());
  RegisterSensitiveVolume(component->GetAllSensitiveVolumes());
  RegisterPhysicalVolume(component->GetAllPhysicalVolumes());
  RegisterRotationMatrix(component->GetAllRotationMatrices());
  RegisterVisAttributes(component->GetAllVisAttributes());
  RegisterUserLimits(component->GetAllUserLimits());
}

std::vector<G4LogicalVolume*> BDSGeometryComponent::GetAllLogicalVolumes() const
{
  std::vector<G4LogicalVolume*> result(allLogicalVolumes);
  for (auto it : allDaughters)
    {
      auto dLVs = it->GetAllLogicalVolumes();
      result.insert(result.end(), dLVs.begin(), dLVs.end());
    }
  return result;
}

std::vector<G4LogicalVolume*> BDSGeometryComponent::GetAllSensitiveVolumes() const
{
  std::vector<G4LogicalVolume*> result(allSensitiveVolumes);
  for (auto it : allDaughters)
    {
      auto dSVs = it->GetAllSensitiveVolumes();
      result.insert(result.end(), dSVs.begin(), dSVs.end());
    }
  return result;
}

