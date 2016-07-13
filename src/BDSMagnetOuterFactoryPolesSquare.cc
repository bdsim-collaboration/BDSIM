#include "BDSMagnetOuterFactoryPolesSquare.hh"

#include "BDSBeamPipe.hh"
#include "BDSDebug.hh"
#include "BDSMagnetOuter.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"

#include "globals.hh"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include <vector>

BDSMagnetOuterFactoryPolesSquare* BDSMagnetOuterFactoryPolesSquare::_instance = nullptr;

BDSMagnetOuterFactoryPolesSquare* BDSMagnetOuterFactoryPolesSquare::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSMagnetOuterFactoryPolesSquare();}
  return _instance;
}

BDSMagnetOuterFactoryPolesSquare::BDSMagnetOuterFactoryPolesSquare():
  BDSMagnetOuterFactoryPolesBase(/*poleStopFactor=*/1.5)
{
  CleanUp();
}

BDSMagnetOuterFactoryPolesSquare::~BDSMagnetOuterFactoryPolesSquare()
{
  _instance = nullptr;
}

void BDSMagnetOuterFactoryPolesSquare::CleanUp()
{
  BDSMagnetOuterFactoryPolesBase::CleanUp();
  poleRotations.clear();
  poleSolids.clear();
  poleLVs.clear();
  order = 0;
}

void BDSMagnetOuterFactoryPolesSquare::CreateYokeAndContainerSolid(G4String name,
								   G4double length,
								   G4int    /*order*/,
								   G4double magnetContainerLength)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // square yoke - have to do subtraction between two solid boxes
  G4VSolid* yokeOuter = new G4Box(name + "_yoke_outer_solid", // name
				  yokeFinishRadius,           // x half width
				  yokeFinishRadius,           // y half width
				  length*0.5 - lengthSafety); // z half length

  G4VSolid* yokeInner = new G4Box(name + "_yoke_inner_solid", // name
				  yokeStartRadius,            // x half width
				  yokeStartRadius,            // y half width
				  length);                    // z half length
  // inner length is 2x as long for unambiguous subtraction
  allSolids.push_back(yokeOuter);
  allSolids.push_back(yokeInner);

  yokeSolid = new G4SubtractionSolid(name + "_yoke_solid",    // name
				     yokeOuter,
				     yokeInner);

  // note container must have hole in it for the beampipe to fit in!
  // poled geometry doesn't fit tightly to beampipe so can alays use a circular aperture

  G4VSolid* containerOuter = new G4Box(name + "_container_outer_solid", // name
				       yokeFinishRadius,                // x half width
				       yokeFinishRadius,                // y half width
				       length*0.5);                     // x half width

  G4VSolid* containerInner = new G4Tubs(name + "_container_inner_solid", // name
					0,                               // inner radius
					poleStartRadius,                 // finish radius
					length,                          // z half length
					0,                               // start angle
					CLHEP::twopi);                   // sweep angle
  // length of inner is long for unambiguous subtraction
  allSolids.push_back(containerOuter);
  allSolids.push_back(containerInner);

  // pole intersection solid
  G4double  croppingBoxRadius = yokeStartRadius - lengthSafety;
  poleIntersectionSolid = new G4Box(name + "_pole_intersection_solid", // name
				    croppingBoxRadius,                 // x half width
				    croppingBoxRadius,                 // y half width
				    length);                           // z length
  allSolids.push_back(poleIntersectionSolid);
  // z length long for unambiguous intersection
  
  containerSolid = new G4SubtractionSolid(name + "_outer_container_solid", // name
					  containerOuter,
					  containerInner);

  magnetContainerSolid = new G4Box(name + "_container_solid", // name
				   magnetContainerRadius,     // x half length
				   magnetContainerRadius,     // y half length
				   magnetContainerLength*0.5);// z half length

  magContExtentX = std::make_pair(-magnetContainerRadius, magnetContainerRadius);
  magContExtentY = std::make_pair(-magnetContainerRadius, magnetContainerRadius);
  magContExtentX = std::make_pair(-magnetContainerLength*0.5, magnetContainerLength*0.5);
}


void BDSMagnetOuterFactoryPolesSquare::IntersectPoleWithYoke(G4String name,
                                                             G4double /*length*/,
                                                             G4int    orderIn)
{
  order = orderIn; // copy to member variable - this is the first function to be called with order
  
  G4int nPoles = 2*orderIn;
  // create different poles to fit inside square yoke
  G4RotationMatrix* iPoleRM;
  for (G4int i = 0; i < nPoles; ++i)
    {
      iPoleRM = new G4RotationMatrix();
      G4double segmentAngle = CLHEP::twopi/nPoles; // angle per pole
      G4double rotationAngle = (0.5-i)*segmentAngle + CLHEP::pi*0.5;
      iPoleRM->rotateZ(rotationAngle);
      allRotationMatrices.push_back(iPoleRM);
      // crop the singlepolesolid with the cropping box so it'll fit inside the outer square yoke
      G4IntersectionSolid* aSolid = new G4IntersectionSolid(name + "_pole_solid", // name
							    poleSolid,            // solid 1 - the pole
							    poleIntersectionSolid,     // solid 2 - the one to be shifted
							    iPoleRM,              // rotation matrix
							    (G4ThreeVector)0);    // translation vector
      
      poleSolids.push_back(aSolid);
    }
}

void BDSMagnetOuterFactoryPolesSquare::CreateLogicalVolumes(G4String    name,
							    G4double    length,
							    G4Colour*   colour,
							    G4Material* outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4VisAttributes* outerVisAttr = new G4VisAttributes(*colour);
  outerVisAttr->SetVisibility(true);
  outerVisAttr->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.push_back(outerVisAttr);

  for (G4int n = 0; n < 2*order; ++n)
    {
      G4LogicalVolume* thisPole = new G4LogicalVolume(poleSolids[n],
						      outerMaterial,
						      name + "_pole_lv");
      thisPole->SetVisAttributes(outerVisAttr);
      poleLVs.push_back(thisPole);
      allLogicalVolumes.push_back(thisPole);
    }

  // yoke
  yokeLV   = new G4LogicalVolume(yokeSolid,
				 outerMaterial,
				 name + "_yoke_lv");
  yokeLV->SetVisAttributes(outerVisAttr);

  // container
  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());
  containerLV = new G4LogicalVolume(containerSolid,
				    emptyMaterial,
				    name + "_container_lv");
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());

  // USER LIMITS
  // set user limits based on bdsim user specified parameters
#ifndef NOUSERLIMITS
  G4UserLimits* outerUserLimits = new G4UserLimits("outer_cuts");
  outerUserLimits->SetMaxAllowedStep( length * maxStepFactor );
  outerUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->MaxTime());
  allUserLimits.push_back(outerUserLimits);
  //attach cuts to volumes
  yokeLV->SetUserLimits(outerUserLimits);
  containerLV->SetUserLimits(outerUserLimits);
  std::vector<G4LogicalVolume*>::iterator j;
  for(j = poleLVs.begin(); j != poleLVs.end(); ++j)
    {(*j)->SetUserLimits(outerUserLimits);}
#endif

  // create logical volumes for the coils using base class method
  CreateLogicalVolumesCoil(name);
}

void BDSMagnetOuterFactoryPolesSquare::PlaceComponents(G4String name,
						       G4int    order)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // PLACEMENT
  // place the components inside the container
  // note we don't need the pointer for placements - it's registered upon construction with g4
  yokePV = new G4PVPlacement((G4RotationMatrix*)nullptr,   // no rotation
			     (G4ThreeVector)0,             // position
			     yokeLV,                       // lv to be placed
			     name + "_yoke_pv",            // name
			     containerLV,                  // mother lv to be placed in
			     false,                        // no boolean operation
			     0,                            // copy number
			     checkOverlaps);               // whether to check overlaps
  
  // place poles
  if (!buildPole)
    {return;}
  G4double nPoles = 2*order;
  G4PVPlacement* aPlacement = nullptr;
  for (G4int n = 0; n < 2*order; ++n)
    {
      G4RotationMatrix* rm = new G4RotationMatrix();
      G4double segmentAngle = CLHEP::twopi/nPoles; // angle per pole
      G4double rotationAngle = (0.5-n)*segmentAngle + CLHEP::pi*0.5;
      rm->rotateZ(-rotationAngle);
      allRotationMatrices.push_back(rm);
      // only need to test the end of one iterator as both should be the same length
      aPlacement = new G4PVPlacement(rm,                 // rotation
				     (G4ThreeVector)0,   // position
				     poleLVs[n],         // logical volume
				     name + "_pole_pv",  // name      
				     containerLV,        // mother lv to be placed in
				     false,              // no boolean operation
				     n,                  // copy number
				     checkOverlaps);     // check overlaps
      allPhysicalVolumes.push_back(aPlacement);
    }
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesSquare::CommonConstructor(G4String     name,
								    G4double     length,
								    BDSBeamPipe* beamPipe,
								    G4int        order,
								    G4double     outerDiameter,
								    G4Material*  outerMaterial,
								    G4double     magnetContainerRadius)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuter* outer = BDSMagnetOuterFactoryPolesBase::CommonConstructor(name, length, beamPipe,
									    order, outerDiameter,
									    outerMaterial, magnetContainerRadius);
  
  outer->RegisterLogicalVolume(poleLVs);
  outer->RegisterSensitiveVolume(poleLVs);
  
  return outer;
}
  
