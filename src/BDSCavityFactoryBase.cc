#include "BDSCavity.hh"
#include "BDSCavityFactoryBase.hh"
#include "BDSColours.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <vector>

class G4UserLimits;

BDSCavityFactoryBase::BDSCavityFactoryBase()
{
  lengthSafety       = BDSGlobalConstants::Instance()->LengthSafety();
  nSegmentsPerCircle = BDSGlobalConstants::Instance()->NSegmentsPerCircle();
  emptyMaterial      = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());
  checkOverlaps      = BDSGlobalConstants::Instance()->CheckOverlaps();

  CleanUp(); // initialise variables
}

void BDSCavityFactoryBase::CleanUp()
{
  // we don't delete any pointers as this factory doesn't own them.
  allSolids.clear();
  allLogicalVolumes.clear();
  allPhysicalVolumes.clear();
  allRotationMatrices.clear();
  allUserLimits.clear();
  allVisAttributes.clear();

  containerSolid = nullptr;
  vacuumLV       = nullptr;
  cavityLV       = nullptr;
  containerLV    = nullptr;
}

void BDSCavityFactoryBase::SetUserLimits(G4double                       length,
					 std::vector<G4LogicalVolume*>& lvs)
{
  auto defaultUL = BDSGlobalConstants::Instance()->GetDefaultUserLimits();
  //copy the default and update with the length of the object rather than the default 1m
  G4UserLimits* ul = BDS::CreateUserLimits(defaultUL, length);

  if (ul != defaultUL) // if it's not the default register it
    {allUserLimits.push_back(ul);}

  for (auto lv : lvs)
    {lv->SetUserLimits(ul);}
}

void BDSCavityFactoryBase::SetVisAttributes(G4String colourName)
{
  // visualisation attributes
  auto col = BDSColours::Instance()->GetColour(colourName);
  G4VisAttributes* cavityVis = new G4VisAttributes(*col);
  cavityVis->SetVisibility(true);
  cavityVis->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  cavityLV->SetVisAttributes(cavityVis);
  allVisAttributes.push_back(cavityVis);
  
  // vacuum
  vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
  // container
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());
}

void BDSCavityFactoryBase::BuildContainerLogicalVolume(G4String name,
						       G4double chordLength,
						       G4double outerRadius) 
{
  containerSolid = new G4Tubs(name + "_container_solid",   // name
			      0.0,                         // innerRadius
			      outerRadius,                 // outerRadius
			      chordLength*0.5,             // half length
			      0.0,                         // starting angle
			      CLHEP::twopi);               // sweep angle
  allSolids.push_back(containerSolid);
  
  containerLV = new G4LogicalVolume(containerSolid,
				    emptyMaterial,
				    name + "_container_lv");
  allLogicalVolumes.push_back(containerLV);
}

void BDSCavityFactoryBase::PlaceComponents(G4String name)
{
  G4PVPlacement* vacuumPV = new G4PVPlacement((G4RotationMatrix*)nullptr,               // rotation
					      G4ThreeVector(0,0,0),  // position
					      vacuumLV,              // logical Volume to be place
					      name + "_vacuum_pv",   // placement name
					      containerLV,           // mother volume
					      false,                 // pMany unused
					      0,                     // copy number
					      checkOverlaps);        // check overlaps
  allPhysicalVolumes.push_back(vacuumPV);
  
  G4PVPlacement* cavityPV = new G4PVPlacement((G4RotationMatrix*)nullptr,               // rotation
					      G4ThreeVector(0,0,0),  // position
					      cavityLV,              // logical Volume to be place
					      name + "_cavity_pv",   // placement name
					      containerLV,           // mother volume
					      false,                 // pMany unused
					      0,                     // copy number
					      checkOverlaps);        // check overlaps
  allPhysicalVolumes.push_back(cavityPV);
}

BDSCavity* BDSCavityFactoryBase::BuildCavityAndRegisterObjects(const BDSExtent& extent)
{
  BDSCavity* cavity = new BDSCavity(containerSolid, containerLV,
				    extent, vacuumLV);

  // register objects
  cavity->RegisterSolid(allSolids);
  cavity->RegisterLogicalVolume(allLogicalVolumes); //using geometry component base class method
  cavity->RegisterPhysicalVolume(allPhysicalVolumes);
  cavity->RegisterRotationMatrix(allRotationMatrices);
  cavity->RegisterUserLimits(allUserLimits);
  cavity->RegisterVisAttributes(allVisAttributes);
  
  return cavity;
}
