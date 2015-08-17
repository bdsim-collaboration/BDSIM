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
  std::vector<G4VSolid*>::iterator itS = allSolids.begin();
  for (; itS != allSolids.end(); ++itS)
    {delete (*itS);}

  std::vector<G4LogicalVolume*>::iterator itLV = allLogicalVolumes.begin();
  for (; itLV != allLogicalVolumes.end(); ++itLV)
    {delete (*itLV);}

  std::vector<G4VPhysicalVolume*>::iterator itPV = allPhysicalVolumes.begin();
  for (; itPV != allPhysicalVolumes.end(); ++itPV)
    {delete (*itPV);}

  std::vector<G4RotationMatrix*>::iterator itRM = allRotationMatrices.begin();
  for (; itRM != allRotationMatrices.end(); ++itRM)
    {delete (*itRM);}

  std::vector<G4VisAttributes*>::iterator itVis = allVisAttributes.begin();
  for (; itVis != allVisAttributes.end(); ++itVis)
    {delete (*itVis);}

  std::vector<G4UserLimits*>::iterator itUL = allUserLimits.begin();
  for (; itUL != allUserLimits.end(); ++itUL)
    {delete (*itUL);}
}

void BDSGeometryComponent::InheritExtents(BDSGeometryComponent* anotherComponent)
{
  SetExtentX(anotherComponent->GetExtentX());
  SetExtentY(anotherComponent->GetExtentY());
  SetExtentZ(anotherComponent->GetExtentZ());
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
  std::vector<G4VSolid*>::iterator it = solids.begin();
  for (; it != solids.end(); ++it)
    {RegisterSolid(*it);}
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
  std::vector<G4LogicalVolume*>::iterator it = logicalVolumes.begin();
  for (; it != logicalVolumes.end(); ++it)
    {RegisterLogicalVolume(*it);}
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
  std::vector<G4RotationMatrix*>::iterator it = rotationMatrices.begin();
  for (; it != rotationMatrices.end(); ++it)
    {RegisterRotationMatrix(*it);}
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
  std::vector<G4LogicalVolume*>::iterator it = sensitiveVolumes.begin();
  for (; it != sensitiveVolumes.end(); ++it)
    {
      RegisterSensitiveVolume(*it);
    }
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
  std::vector<G4VisAttributes*>::iterator it = visAttributes.begin();
  for (; it != visAttributes.end(); ++it)
    {RegisterVisAttributes(*it);}
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
  std::vector<G4UserLimits*>::iterator it = userLimits.begin();
  for (; it != userLimits.end(); ++it)
    {RegisterUserLimits(*it);}
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


