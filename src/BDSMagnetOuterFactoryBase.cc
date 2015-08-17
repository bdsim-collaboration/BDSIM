#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetOuterFactoryBase.hh"
#include "BDSMaterials.hh"

#include "globals.hh" // geant4 types / globals

#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include <utility>

G4double const BDSMagnetOuterFactoryBase::lengthSafetyLarge = 1*CLHEP::um;

BDSMagnetOuterFactoryBase::BDSMagnetOuterFactoryBase()
{
  lengthSafety       = BDSGlobalConstants::Instance()->GetLengthSafety();
  checkOverlaps      = BDSGlobalConstants::Instance()->GetCheckOverlaps();
  visDebug           = BDSExecOptions::Instance()->GetVisDebug();
  nSegmentsPerCircle = 50;
  maxStepFactor      = 0.5;

  // initialise variables and pointers that'll be used by the factory
  CleanUp();
}

void BDSMagnetOuterFactoryBase::CleanUp()
{
  poleSolid            = nullptr;
  yokeSolid            = nullptr;
  containerSolid       = nullptr;
  magnetContainerSolid = nullptr;
  poleLV               = nullptr;
  yokeLV               = nullptr;
  containerLV          = nullptr;
  magnetContainerLV    = nullptr;
  yokePV               = nullptr;
  outerVisAttributes   = nullptr;
  outerUserLimits      = nullptr;

  allLogicalVolumes.clear();
  allPhysicalVolumes.clear();
  allRotationMatrices.clear();
  allSolids.clear();
  allVisAttributes.clear();
  allUserLimits.clear();

  magContExtentX = std::make_pair(0,0);
  magContExtentY = std::make_pair(0,0);
  magContExtentZ = std::make_pair(0,0);

  magnetContainer = nullptr;
}

void BDSMagnetOuterFactoryBase::CreateLogicalVolumes(G4String    name,
						     G4double    length,
						     G4Colour*   colour,
						     G4Material* outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  if (poleSolid)
    {
      poleLV   = new G4LogicalVolume(poleSolid,
				     outerMaterial,
				     name + "_pole_lv");
    }
  
  yokeLV   = new G4LogicalVolume(yokeSolid,
				 outerMaterial,
				 name + "_yoke_lv");

  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());
  containerLV = new G4LogicalVolume(containerSolid,
				    emptyMaterial,
				    name + "_outer_container_lv");

  magnetContainerLV = new G4LogicalVolume(magnetContainerSolid,
					  emptyMaterial,
					  name + "_container_lv");
  
  // VISUAL ATTRIBUTES
  G4VisAttributes* outerVisAttr = new G4VisAttributes(*colour);
  outerVisAttr->SetVisibility(true);
  outerVisAttr->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.push_back(outerVisAttr);
  if (poleLV)
    {poleLV->SetVisAttributes(outerVisAttr);}
  yokeLV->SetVisAttributes(outerVisAttr);
  // container
  if (BDSExecOptions::Instance()->GetVisDebug())
    {
      containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());
      magnetContainerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());
    }
  else
    {
      containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
      magnetContainerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
    }

  // USER LIMITS
#ifndef NOUSERLIMITS
  G4UserLimits* outerUserLimits = new G4UserLimits("outer_cuts");
  outerUserLimits->SetMaxAllowedStep( length * maxStepFactor );
  outerUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  allUserLimits.push_back(outerUserLimits);
  //attach cuts to volumes
  if (poleLV)
    {poleLV->SetUserLimits(outerUserLimits);}
  yokeLV->SetUserLimits(outerUserLimits);
  containerLV->SetUserLimits(outerUserLimits);
  magnetContainerLV->SetUserLimits(outerUserLimits);
#endif
}

void BDSMagnetOuterFactoryBase::BuildMagnetContainerSolidAngled(G4String      name,
								G4double      magnetContainerLength,
								G4double      magnetContainerRadius,
								G4ThreeVector inputFace,
								G4ThreeVector outputFace)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  magnetContainerSolid = new G4CutTubs(name + "_container_solid",   // name
				       0,                           // inner radius
				       magnetContainerRadius,       // outer radius
				       magnetContainerLength * 0.5, // z half length
				       0,                           // starting angle
				       CLHEP::twopi,                // sweep angle
				       inputFace,                   // input face normal vector
				       outputFace);                 // output fae normal vector

  magContExtentX = std::make_pair(-magnetContainerRadius, magnetContainerRadius);
  magContExtentY = std::make_pair(-magnetContainerRadius, magnetContainerRadius);
  magContExtentX = std::make_pair(-magnetContainerLength*0.5, magnetContainerLength*0.5);
}


void BDSMagnetOuterFactoryBase::BuildMagnetContainerSolidStraight(G4String name,
								  G4double magnetContainerLength,
								  G4double magnetContainerRadius)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  magnetContainerSolid = new G4Tubs(name + "_container_solid",   // name
				    0,                           // inner radius
				    magnetContainerRadius,       // outer radius
				    magnetContainerLength * 0.5, // z half length
				    0,                           // starting angle
				    CLHEP::twopi);               // sweep angle
  
  magContExtentX = std::make_pair(-magnetContainerRadius, magnetContainerRadius);
  magContExtentY = std::make_pair(-magnetContainerRadius, magnetContainerRadius);
  magContExtentX = std::make_pair(-magnetContainerLength*0.5, magnetContainerLength*0.5);
}
  
void BDSMagnetOuterFactoryBase::CreateMagnetContainerComponent()
{
  magnetContainer = new BDSGeometryComponent(magnetContainerSolid,
					     magnetContainerLV,
					     magContExtentX,
					     magContExtentY,
					     magContExtentZ);
}
