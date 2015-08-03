#include "BDSMagnetOuterFactoryPolesBase.hh"

#include "BDSBeamPipe.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetColours.hh"
#include "BDSMagnetOuterFactoryCylindrical.hh" // for default geometry
#include "BDSMaterials.hh"
#include "BDSSDManager.hh"
// #include "BDSUtilities.hh"                 // for calculateorientation - s and r bend

#include "globals.hh"                      // geant4 globals / types
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4EllipticalTube.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include <cmath>
#include <utility>                         // for std::pair
#include <algorithm>                       // for std::max

BDSMagnetOuterFactoryPolesBase::BDSMagnetOuterFactoryPolesBase()
{
  // geometrical parameters
  poleFraction        = 0.7;
  poleAngularFraction = 0.45;
  poleTipFraction     = 0.2;

  // now the base class constructor should be called first which
  // should call clean up (in the derived class) which should initialise
  // the variables I think, but doing here just to be sure.
  CleanUp();
}

void BDSMagnetOuterFactoryPolesBase::CleanUp()
{
  BDSMagnetOuterFactoryBase::CleanUp();
  
  poleStartRadius  = 0;
  poleFinishRadius = 0;
  yokeStartRadius  = 0;
  yokeFinishRadius = 0;
}

BDSGeometryComponent* BDSMagnetOuterFactoryPolesBase::CreateSectorBend(G4String      name,
								       G4double      length,
								       BDSBeamPipe*  beamPipe,
								       G4double      outerDiameter,
								       G4double      angle,
								       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CleanUp(); // doesn't use CommonConstructor so must do this manually
  
  return BDSMagnetOuterFactoryCylindrical::Instance()->CreateSectorBend(name,length,beamPipe,outerDiameter,
									angle,outerMaterial);
  /*

  // test input parameters - set global options as default if not specified
  TestInputParameters(beamPipe,outerDiameter,outerMaterial);

  G4int orientation   = BDS::CalculateOrientation(angle);
  G4double zcomponent = cos(fabs(angle*0.5)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double xcomponent = sin(fabs(angle*0.5)); // note full angle here as it's the exit angle
  G4ThreeVector inputface  = G4ThreeVector(-orientation*xcomponent, 0.0, -1.0*zcomponent); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(-orientation*xcomponent, 0.0, zcomponent);   // no output face angle
  
  if (beamPipe->ContainerIsCircular())
    {
      //circular beampipe so we can simply use its radius
      outerSolid = new G4CutTubs(name + "_outer_solid",       // name
				 beamPipe->GetContainerRadius() + 2*lengthSafety,  // inner radius
				 outerDiameter*0.5,                 // outer radius
				 length*0.5-2*lengthSafety,   // half length
				 0,                           // rotation start angle
				 CLHEP::twopi,                // rotation finish angle
				 inputface,                   // input face normal
				 outputface);                 // output face normal

      //container is similar but slightly wider and hollow (to allow placement of beampipe)
      containerSolid = new G4CutTubs(name + "_contiainer_solid",  // name
				     beamPipe->GetContainerRadius() + lengthSafety, // inner radius
				     outerDiameter*0.5 + lengthSafety,  // outer radius
				     length*0.5,                  // half length
				     0,                           // rotation start angle
				     CLHEP::twopi,                // rotation finish angle
				     inputface,                   // input face normal
				     outputface);                 // output face normal
    }
  else
    {
      G4VSolid* outerSolidCylinder = new G4CutTubs(name + "_outer_solid_cylinder",  // name
						   0,  // inner radius - for unambiguous subtraction
						   outerDiameter*0.5,                 // outer radius
						   length*0.5-2*lengthSafety,   // half length
						   0,                           // rotation start angle
						   CLHEP::twopi,                // rotation finish angle
						   inputface,                   // input face normal
						   outputface);                 // output face normal
      outerSolid = new G4SubtractionSolid(name + "_outer_solid",
					  outerSolidCylinder,
					  beamPipe->GetContainerSubtractionSolid());

      //container is similar but slightly wider
      G4VSolid* containerSolidCylinder = new G4CutTubs(name + "_container_solid_cylinder", // name
						       0,  // inner radius - for unambiguous subtraction
						       outerDiameter*0.5 + lengthSafety,  // outer radius
						       length*0.5,                  // half length
						       0,                           // rotation start angle
						       CLHEP::twopi,                // rotation finish angle
						       inputface,                   // input face normal
						       outputface);                 // output face normal
      containerSolid = new G4SubtractionSolid(name + "_container_solid",
					      containerSolidCylinder,
					      beamPipe->GetContainerSubtractionSolid());
    }
  return CommonFinalConstructor(name,length,outerDiameter,outerMaterial,BDSMagnetColours::Instance()->GetMagnetColour("sectorbend"));
  */
}

BDSGeometryComponent* BDSMagnetOuterFactoryPolesBase::CreateRectangularBend(G4String      name,
									    G4double      length,
									    BDSBeamPipe*  beamPipe,
									    G4double      outerDiameter,
									    G4double      angle,
									    G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CleanUp(); // doesn't use CommonConstructor so must do this manually
  
  return BDSMagnetOuterFactoryCylindrical::Instance()->CreateRectangularBend(name,length,beamPipe,outerDiameter,
									     angle,outerMaterial);
  /*
  //rectangular bends currently just make a shorter straight volume, so ignore angle for now
  CreateCylindricalSolids(name, length, beamPipe, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial, BDSMagnetColours::Instance()->GetMagnetColour("rectangularbend"));
  */
}

BDSGeometryComponent* BDSMagnetOuterFactoryPolesBase::CreateQuadrupole(G4String      name,
								       G4double      length,
								       BDSBeamPipe*  beamPipe,
								       G4double      outerDiameter,
								       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return CommonConstructor(name, length, beamPipe, 2, outerDiameter, outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactoryPolesBase::CreateSextupole(G4String      name,
								      G4double      length,
								      BDSBeamPipe*  beamPipe,
								      G4double      outerDiameter,
								      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return CommonConstructor(name, length, beamPipe, 3, outerDiameter, outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactoryPolesBase::CreateOctupole(G4String      name,
								     G4double      length,
								     BDSBeamPipe*  beamPipe,
								     G4double      outerDiameter,
								     G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return CommonConstructor(name, length, beamPipe, 4, outerDiameter, outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactoryPolesBase::CreateDecapole(G4String      name,
								     G4double      length,
								     BDSBeamPipe*  beamPipe,
								     G4double      outerDiameter,
								     G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return CommonConstructor(name, length, beamPipe, 5, outerDiameter, outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactoryPolesBase::CreateSolenoid(G4String      name,
								     G4double      length,
								     BDSBeamPipe*  beamPipe,
								     G4double      outerDiameter,
								     G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return BDSMagnetOuterFactoryCylindrical::Instance()->CreateSolenoid(name,length,beamPipe,outerDiameter,outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactoryPolesBase::CreateMultipole(G4String      name,
								      G4double      length,
								      BDSBeamPipe*  beamPipe,
								      G4double      outerDiameter,
								      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return BDSMagnetOuterFactoryCylindrical::Instance()->CreateMultipole(name,length,beamPipe,outerDiameter,outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactoryPolesBase::CreateRfCavity(G4String      name,
								     G4double      length,
								     BDSBeamPipe*  beamPipe,
								     G4double      outerDiameter,
								     G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return BDSMagnetOuterFactoryCylindrical::Instance()->CreateRfCavity(name,length,beamPipe,outerDiameter,outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactoryPolesBase::CreateMuSpoiler(G4String      name,
								      G4double      length,
								      BDSBeamPipe*  beamPipe,
								      G4double      outerDiameter,
								      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return BDSMagnetOuterFactoryCylindrical::Instance()->CreateMuSpoiler(name,length,beamPipe,outerDiameter,outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactoryPolesBase::CreateKicker(G4String      name,
								   G4double      length,
								   BDSBeamPipe*  beamPipe,
								   G4double      outerDiameter,
								   G4bool        vertical,
								   G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return BDSMagnetOuterFactoryCylindrical::Instance()->CreateKicker(name,length,beamPipe,outerDiameter,
								    vertical,outerMaterial);
}

/// functions below here are private to this particular factory
BDSGeometryComponent* BDSMagnetOuterFactoryPolesBase::CommonConstructor(G4String     name,
									G4double     length,
									BDSBeamPipe* beamPipe,
									G4int        order,
									G4double     outerDiameter,
									G4Material*  outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // reset all pointers and variables to protect against bugs using previous use of factory
  CleanUp();
  
  CalculatePoleAndYoke(outerDiameter, beamPipe, order);
  CreatePoleSolid(name, length, order);
  CreateYokeAndContainerSolid(name, length, order);
  CreateLogicalVolumes(name, length, order, outerMaterial);
  PlaceComponents(name, order); //returns vector of PVs
  
  // record extents
  // container radius is just outerDiamter as yoke is circular
  G4double containerRadius = outerDiameter + lengthSafety;
  std::pair<double,double> extX = std::make_pair(-containerRadius,containerRadius);
  std::pair<double,double> extY = std::make_pair(-containerRadius,containerRadius);
  std::pair<double,double> extZ = std::make_pair(-length*0.5,length*0.5);
  
  // build the BDSGeometryComponent instance and return it
  BDSGeometryComponent* outer = new BDSGeometryComponent(containerSolid,
							 containerLV,
							 extX, extY, extZ);

  outer->RegisterRotationMatrix(allRotationMatrices);
  outer->RegisterPhysicalVolume(allPhysicalVolumes);
  outer->RegisterVisAttributes(allVisAttributes);
  outer->RegisterUserLimits(allUserLimits);
  
  // Register logical volumes and set sensitivity
  // test if poleLV exists first as some derived classes use their own vector of
  // poles and don't use this one - therefore it'll be null
  if (poleLV)
    {
      outer->RegisterLogicalVolume(poleLV);
      outer->RegisterSensitiveVolume(poleLV);
    }
  outer->RegisterLogicalVolume(yokeLV);
  outer->RegisterSensitiveVolume(yokeLV);
  
  return outer;
}

void BDSMagnetOuterFactoryPolesBase::CalculatePoleAndYoke(G4double     outerDiameter,
							  BDSBeamPipe* beamPipe,
							  G4double     /*order*/)
{
  G4double bpRadius = beamPipe->GetContainerRadius();
  // check parameters are valid
  if (outerDiameter*0.5 < bpRadius)
    {
      G4cerr << "outerDiameter (" << outerDiameter << ") must be greater than 2*beampipe radius ("
	     << 2*bpRadius << ")" << G4endl;
      exit(1);
    }

  // layout markers for the pole and yoke - radially out from centre
  poleStartRadius       = bpRadius + lengthSafety;
  yokeFinishRadius      = outerDiameter*0.5;
  G4double totalLength  = yokeFinishRadius - poleStartRadius;
  poleFinishRadius      = poleStartRadius + poleFraction*totalLength;
  yokeStartRadius       = poleFinishRadius + lengthSafety;
}

void BDSMagnetOuterFactoryPolesBase::CreatePoleSolid(G4String     name,
						     G4double     length,
						     G4int        order)
{
  G4int nPoles = 2*order;
  
  // full circle is divided into segments for each pole
  G4double segmentAngleWidth = CLHEP::twopi / (G4double)nPoles;
  G4double poleAngle = segmentAngleWidth * poleAngularFraction;

  // compose pole of an annulus of a certain angle plus
  // an ellipsoid at the inner side. make the union of the
  // two to give a typical pole shape - curved inner surface
  // with increasing width outwards
  
  // make some % of pole length curved ellipsoidal surface
  G4double poleLength          = poleFinishRadius - poleStartRadius - 2*lengthSafety;
  G4double ellipsoidHeight     = poleTipFraction*poleLength;
  //G4double annulusHeight       = poleLength - ellipsoidHeight;
  G4double ellipsoidCentre     = poleStartRadius + ellipsoidHeight*0.5;

  // work out the chord length
  G4double ellipsoidWidth      = 2*(poleStartRadius + 0.5*ellipsoidHeight) * tan(poleAngle*0.5);
  
  G4double annulusStartRadius  = ellipsoidCentre / cos(poleAngle*0.5); //so edges line up with ellipsoid edges
  G4double annulusFinishRadius = poleFinishRadius;
  
  G4VSolid* poleTip = new G4EllipticalTube(name + "_pole_tip_solid",   // name
					   ellipsoidHeight*0.5,        // x half width
					   ellipsoidWidth*0.5,         // y half width
					   length*0.5 - lengthSafety); // z half width

  G4VSolid* poleAnnulus = new G4Tubs(name + "_pole_annulus_solid", // name
				     annulusStartRadius,           // start radius
				     annulusFinishRadius,          // finish radius
				     length*0.5 - lengthSafety,    // z half length
				     -poleAngle*0.5,               // start angle
				     poleAngle);                   // sweep angle

  // note annulus is defined from centre of rotation
  G4ThreeVector ellipsoidTranslation(poleStartRadius + ellipsoidHeight*0.5, 0, 0);
  // the centre of the union solid will be that of the 1st solid - the annulus - ie 0,0,0
  poleSolid = new G4UnionSolid(name + "_pole_solid", // name
			       poleAnnulus,          // solid 1
			       poleTip,              // solid 2
			       0,                    // rotation matrix
			       ellipsoidTranslation);// translation of poleTip

  allSolids.push_back(poleTip);
  allSolids.push_back(poleAnnulus);
  allSolids.push_back(poleSolid);
}

void BDSMagnetOuterFactoryPolesBase::CreateYokeAndContainerSolid(G4String      name,
								 G4double      length,
								 G4int         /*order*/)
{
  // circular yoke so pretty easy
  yokeSolid = new G4Tubs(name + "_yoke_solid",      // name
			 yokeStartRadius,           // start radius
			 yokeFinishRadius,          // finish radius
			 length*0.5 - lengthSafety, // z half length
			 0,                         // start angle
			 CLHEP::twopi);             // sweep angle

  // note container must have hole in it for the beampipe to fit in!
  // poled geometry doesn't fit tightly to beampipe so can alays use a circular aperture
  containerSolid = new G4Tubs(name + "_container_solid",       // name
			      poleStartRadius,                 // start radius
			      yokeFinishRadius + lengthSafety, // finish radius
			      length*0.5,                      // z half length
			      0,                               // start angle
			      CLHEP::twopi);                   // sweep angle
  allSolids.push_back(yokeSolid);
}

void BDSMagnetOuterFactoryPolesBase::TestInputParameters(BDSBeamPipe* beamPipe,
							 G4double&    outerDiameter,
							 G4Material*& outerMaterial)// reference to a pointer
{
  //function arguments by reference to they can be modified in place
  //check outer material is something
  if (!outerMaterial)
    {outerMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());}

  // ensure box size is bigger than the beampipe
  if (beamPipe->ContainerIsCircular()) {
    // if it's circular, just check radius
    if (outerDiameter < 2*(beamPipe->GetContainerRadius()) )
      {outerDiameter = 2*(beamPipe->GetContainerRadius()) + 1*CLHEP::mm;}
  } else {
    // it's not circular - have a look at extents
    // +ve - -ve
    G4double extentX = beamPipe->GetExtentX().second - beamPipe->GetExtentX().first;
    G4double extentY = beamPipe->GetExtentY().second - beamPipe->GetExtentY().first;
    if ( (outerDiameter < extentX) || (outerDiameter < extentY) ) {
      // outerDiameter isn't sufficient for range in x or y
      outerDiameter = std::max(extentX,extentY) + 1*CLHEP::mm;
    }
  }
}

void BDSMagnetOuterFactoryPolesBase::CreateLogicalVolumes(G4String   name,
							  G4double   length,
							  G4int      order,
							  G4Material* outerMaterial)
{  
  // build the logical volumes
  poleLV   = new G4LogicalVolume(poleSolid,
				 outerMaterial,
				 name + "_pole_lv");

  yokeLV   = new G4LogicalVolume(yokeSolid,
				 outerMaterial,
				 name + "_yoke_lv");

  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());
  containerLV = new G4LogicalVolume(containerSolid,
				    emptyMaterial,
				    name + "_container_lv");
  
  // VISUAL ATTRIBUTES
  // set visual attributes
  G4Colour* magnetColour = BDSMagnetColours::Instance()->GetMagnetColour(order);
  G4VisAttributes* outerVisAttr = new G4VisAttributes(*magnetColour);
  outerVisAttr->SetVisibility(true);
  outerVisAttr->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.push_back(outerVisAttr);
  poleLV->SetVisAttributes(outerVisAttr);
  yokeLV->SetVisAttributes(outerVisAttr);
  // container
  if (BDSExecOptions::Instance()->GetVisDebug())
    {containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());}
  else
    {containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());}

  // USER LIMITS
  // set user limits based on bdsim user specified parameters
#ifndef NOUSERLIMITS
  G4UserLimits* outerUserLimits = new G4UserLimits("outer_cuts");
  outerUserLimits->SetMaxAllowedStep( length * maxStepFactor );
  outerUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  allUserLimits.push_back(outerUserLimits);
  //attach cuts to volumes
  poleLV->SetUserLimits(outerUserLimits);
  yokeLV->SetUserLimits(outerUserLimits);
  containerLV->SetUserLimits(outerUserLimits);
#endif
}

void BDSMagnetOuterFactoryPolesBase::PlaceComponents(G4String name,
						     G4int    order)
{
  // PLACEMENT
  // place the components inside the container
  // note we don't need the pointer for placements - it's registered upon construction with g4
  yokePV = new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
			     (G4ThreeVector)0,             // position
			     yokeLV,                       // lv to be placed
			     name + "_yoke_pv",            // name
			     containerLV,                  // mother lv to be placed in
			     false,                        // no boolean operation
			     0,                            // copy number
			     checkOverlaps);               // whether to check overlaps

  allPhysicalVolumes.push_back(yokePV);
  // pole placement
  G4PVPlacement* aPolePV = nullptr;
  for (G4int n = 0; n < 2*order; ++n)
    {
      // prepare a new rotation matrix - must be new and can't reuse the same one
      // as the placement doesn't own it - changing the existing one will affect all
      // previously placed objects
      G4RotationMatrix* rm  = new G4RotationMatrix();
      allRotationMatrices.push_back(rm);
      G4double segmentAngle = CLHEP::twopi/(G4double)(2*order); // angle per pole
      rm->rotateZ((n+0.5)*segmentAngle + CLHEP::pi*0.5);

      aPolePV = new G4PVPlacement(rm,                 // rotation
				  (G4ThreeVector)0,   // position
				  poleLV,             // logical volume
				  name + "_pv",       // name      
				  containerLV,        // mother lv to be placed in
				  false,              // no boolean operation
				  n,                  // copy number
				  checkOverlaps);     // check overlaps
      allPhysicalVolumes.push_back(aPolePV);
      //name + "_pole_" + printf("_%d_pv", n), // name
    }
}

BDSGeometryComponent* BDSMagnetOuterFactoryPolesBase::KickerConstructor(G4String     name,
									G4double     length,
									G4double     angle,
									BDSBeamPipe* beamPipe,
									G4double     outerDiameter,
									G4Material*  outerMaterial,
									G4bool       isVertical)
{
  CleanUp();
  
  // test input parameters
  
  // geometrical design parameters
  G4double yokeWidthFraction    = 0.2; // the fraction of the full width that the yoke thickness is
  G4double coilWidthFraction    = 0.3; // the fraction of the full width that the coil full width is
  G4double coilHeightFraction   = 0.4; // the fraction of the full height that the coil full height is

  // calculate geometrical parameters
  G4double beamPipeContainerRadius = beamPipe->GetContainerRadius(); 

  G4double coilHalfWidth        = coilWidthFraction  * outerDiameter * 0.5;
  G4double coilHalfHeight       = coilHeightFraction * outerDiameter * 0.5;
  
  G4double containerHalfWidth   = outerDiameter * 0.5;
  G4double containerHalfHeight  = outerDiameter * 0.5;
  
  G4double yokeHalfWidth        = containerHalfWidth  - coilHalfWidth  - lengthSafety; // - lengthSafety to fit inside container
  G4double yokeHalfHeight       = containerHalfHeight - lengthSafety;
  
  G4double yokeCutOutHalfWidth  = yokeHalfWidth  * (1 - yokeWidthFraction);
  G4double yokeCutOutHalfHeight = yokeHalfHeight * (1 - yokeWidthFraction);

  G4double poleHalfWidth        = beamPipeContainerRadius;
  G4double poleHalfHeight       = 0.5 * (yokeCutOutHalfHeight - beamPipeContainerRadius);
  
  // calculate offsets and placement vectors
  G4int massOffset = 1;
  if (angle > 0)
    {massOffset = -1;}
  G4ThreeVector containerTranslation = G4ThreeVector((containerHalfWidth - beamPipeContainerRadius), 0, 0);
  G4ThreeVector yokeTranslation      = G4ThreeVector(yokeHalfWidth - beamPipeContainerRadius, 0, 0);
  G4ThreeVector upperPoleTranslation = G4ThreeVector(0, beamPipeContainerRadius + poleHalfHeight,    0);
  G4ThreeVector lowerPoleTranslation = G4ThreeVector(0, -(beamPipeContainerRadius + poleHalfHeight), 0);
  G4ThreeVector outerCoilTranslation = G4ThreeVector(yokeHalfWidth + coilHalfWidth, 0, 0);
  G4double      innerCoilXOffset     = yokeHalfWidth - ( (yokeHalfWidth - yokeCutOutHalfWidth)*2.) - coilHalfWidth;
  G4ThreeVector innerCoilTranslation = G4ThreeVector(innerCoilXOffset, 0, 0);

  // build container
  containerSolid  = new G4Box(name + "_container_solid",           // name
			      containerHalfWidth,                  // x half width
			      containerHalfHeight,                 // y half width
			      length*0.5);                         // z half length
  
  // build yoke & pole piece
  G4VSolid* yokeBox    = new G4Box(name + "_yoke_box_solid",            // name
				   yokeHalfWidth,                       // x half width
				   yokeHalfHeight,                      // y half width
				   length*0.5 - lengthSafety);          // z half width

  G4VSolid* yokeCutOut = new G4Box(name + "_yoke_cut_out_solid",        // name
				   yokeCutOutHalfWidth,                 // x half width
				   yokeCutOutHalfHeight,                // y half width
				   length);                             // z half width (long for unambiguous subtraction)

  G4double      yokeSubtractionXOffset = yokeHalfWidth * yokeWidthFraction * massOffset;
  G4ThreeVector yokeSubtractionOffset  = G4ThreeVector(yokeSubtractionXOffset,0,0);
  yokeSolid = new G4SubtractionSolid(name + "_yoke_solid",   // name
				     yokeBox,                // solid 1
				     yokeCutOut,             // minus solid 2
				     0,                      // rotation
				     yokeSubtractionOffset); // translation
  
  poleSolid  = new G4Box(name + "_pole_solid",                // name
			 poleHalfWidth,                       // x half width
			 poleHalfHeight,                      // y half width
			 length*0.5 - lengthSafety);          // z half width  
  // build coils
  G4VSolid* coil = new G4Box(name + "_coil_solid",            // name
			     coilHalfWidth,                   // x half width
			     coilHalfHeight,                  // y half width
			     length*0.5 - lengthSafety);      // z half width

  allSolids.push_back(yokeBox);
  allSolids.push_back(yokeCutOut);
  allSolids.push_back(yokeSolid);
  allSolids.push_back(poleSolid);
  allSolids.push_back(coil);

  // logical volumes
  G4Material* copper = BDSMaterials::Instance()->GetMaterial("copper");
  G4Material* empty  = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());
  containerLV = new G4LogicalVolume(containerSolid,          // solid
				    empty,                   // material
				    name + "_container_lv"); // name
  yokeLV      = new G4LogicalVolume(yokeSolid,               // solid
				    outerMaterial,           // material
				    name + "_yoke_lv");      // name
  poleLV      = new G4LogicalVolume(poleSolid,               // solid
				    outerMaterial,           // material
				    name + "_pole_lv");      // name
  G4LogicalVolume* coilLV      = new G4LogicalVolume(coil,               // solid
						     copper,             // material
						     name + "_coil_lv"); // name

  allLogicalVolumes.push_back(yokeLV); //not container as done separately in BDSGeometryComponent constructor
  allLogicalVolumes.push_back(poleLV);

  // VISUAL ATTRIBUTES
  // set visual attributes
  std::string magnetType;
  if (isVertical)
    {magnetType = "vkicker";}
  else
    {magnetType = "hkicker";}

  // yoke and pole
  G4Colour* magnetColour = BDSMagnetColours::Instance()->GetMagnetColour(magnetType);
  G4VisAttributes* outerVisAttr = new G4VisAttributes(*magnetColour);
  outerVisAttr->SetVisibility(true);
  allVisAttributes.push_back(outerVisAttr);
  // no curved surfaces so don't need to set nsegments
  yokeLV->SetVisAttributes(outerVisAttr);
  poleLV->SetVisAttributes(outerVisAttr);

  // coil
  G4Colour* coilColour  = BDSMagnetColours::Instance()->GetMagnetColour("coil");
  G4VisAttributes* coilVisAttr = new G4VisAttributes(*coilColour);
  coilVisAttr->SetVisibility(true);
  allVisAttributes.push_back(coilVisAttr);
  coilLV->SetVisAttributes(coilVisAttr);

  // container
  if (BDSExecOptions::Instance()->GetVisDebug())
    {containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());}
  else
    {containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());}

  // user limits
#ifndef NOUSERLIMITS
  G4UserLimits* outerUserLimits = new G4UserLimits("outer_cuts");
  outerUserLimits->SetMaxAllowedStep( length * maxStepFactor );
  outerUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  allUserLimits.push_back(outerUserLimits);
  // attach cuts to volumes
  yokeLV->SetUserLimits(outerUserLimits);
  poleLV->SetUserLimits(outerUserLimits);
  coilLV->SetUserLimits(outerUserLimits);
  containerLV->SetUserLimits(outerUserLimits);
#endif

  // place components
  yokePV       = new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
				   yokeTranslation,              // position
				   yokeLV,                       // lv to be placed
				   name + "_yoke_pv",            // name
				   containerLV,                  // mother lv to be place in
				   false,                        // no boolean operation
				   0,                            // copy number
				   checkOverlaps);

  G4PVPlacement* poleLowerPV = new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
						 lowerPoleTranslation,         // position
						 poleLV,                       // lv to be placed
						 name + "_lower_pole_pv",      // name
						 containerLV,                  // mother lv to be place in
						 false,                        // no boolean operation
						 0,                            // copy number
						 checkOverlaps);

  G4PVPlacement* poleUpperPV = new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
						 upperPoleTranslation,         // position
						 poleLV,                       // lv to be placed
						 name + "_upper_pole_pv",      // name
						 containerLV,                  // mother lv to be place in
						 false,                        // no boolean operation
						 0,                            // copy number
						 checkOverlaps);

  G4PVPlacement* coilOuterPV = new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
						 outerCoilTranslation,         // position
						 coilLV,                       // lv to be placed
						 name + "_outer_coil_pv",      // name
						 containerLV,                  // mother lv to be place in
						 false,                        // no boolean operation
						 0,                            // copy number
						 checkOverlaps);

  G4PVPlacement* coilInnerPV = new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
						 innerCoilTranslation,         // position
						 coilLV,                       // lv to be placed
						 name + "_inner_coil_pv",      // name
						 containerLV,                  // mother lv to be place in
						 false,                        // no boolean operation
						 0,                            // copy number
						 checkOverlaps);

  allPhysicalVolumes.push_back(yokePV);
  allPhysicalVolumes.push_back(poleLowerPV);
  allPhysicalVolumes.push_back(poleUpperPV);
  allPhysicalVolumes.push_back(coilOuterPV);
  allPhysicalVolumes.push_back(coilInnerPV);
  
  // record extents
  // container radius is just outerDiamter as yoke is circular
  G4double containerRadius = 0.5*outerDiameter;
  G4double extXPos = outerDiameter - beamPipeContainerRadius;
  G4double extXNeg = -beamPipeContainerRadius;
  std::pair<double,double> extX = std::make_pair(extXNeg,extXPos);
  std::pair<double,double> extY = std::make_pair(-containerRadius,containerRadius);
  std::pair<double,double> extZ = std::make_pair(-length*0.5,length*0.5);
  
  // construct geometry component
  BDSGeometryComponent* kicker = new BDSGeometryComponent(containerSolid,
							  containerLV,
							  extX, extY, extZ,
							  containerTranslation);

  // register everything
  kicker->RegisterLogicalVolume(allLogicalVolumes);
  kicker->RegisterSensitiveVolume(allLogicalVolumes);
  kicker->RegisterPhysicalVolume(allPhysicalVolumes);
  kicker->RegisterVisAttributes(allVisAttributes);
  kicker->RegisterUserLimits(allUserLimits);
  
  return kicker;
}
