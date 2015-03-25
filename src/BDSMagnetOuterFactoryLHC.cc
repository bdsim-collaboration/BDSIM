#include "BDSMagnetOuterFactoryLHC.hh"

#include "BDSBeamPipe.hh"
#include "BDSBeamPipeType.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSDebug.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetColours.hh"
#include "BDSMaterials.hh"
#include "BDSSDManager.hh"
#include "BDSUtilities.hh"                 // for calculateorientation

#include "globals.hh"                      // geant4 globals / types
#include "G4Colour.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4UnionSolid.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"
#include "G4IntersectionSolid.hh"
#include <cmath>
#include <utility>                         // for std::pair
#include <algorithm>                       // for std::max
#include <vector>


BDSMagnetOuterFactoryLHC::BDSMagnetOuterFactoryLHC(G4bool isLeftOffsetIn):
  isLeftOffset(isLeftOffsetIn)
{
  lengthSafety   = BDSGlobalConstants::Instance()->GetLengthSafety();
  outerSolid     = NULL;
  containerSolid = NULL;
}

BDSGeometryComponent* BDSMagnetOuterFactoryLHC::CreateSectorBend(G4String      name,
								 G4double      length,
								 BDSBeamPipe*  beamPipe,
								 G4double      boxSize,
								 G4double      angle,
								 G4Material*   outerMaterial)

{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  // test input parameters - set global options as default if not specified
  TestInputParameters(beamPipe,boxSize,outerMaterial);

  // geometrical constants
  // mass to the right or to the left
  G4ThreeVector dipolePosition;
  G4double      massShift    = 97.26*CLHEP::mm;
  G4double BPseparation      = 2*massShift;
  G4double collarOuterRadius = 101.18*CLHEP::mm;

  if (isLeftOffset)
    {
      dipolePosition = G4ThreeVector(massShift,0.,0.);
      BPseparation *= -1;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "dipole to the Right" << G4endl;
#endif
    }
  else
    {
      dipolePosition = G4ThreeVector(-massShift,0.,0.);
      massShift    *= -1;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "dipole to the Left" << G4endl;
#endif
    }

  // calculate some geometrical parameters
  G4int orientation   = BDS::CalculateOrientation(angle);
  G4double zcomponent = cos(fabs(angle*0.5)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double xcomponent = sin(fabs(angle*0.5)); // note full angle here as it's the exit angle
  G4ThreeVector inputface  = G4ThreeVector(-orientation*xcomponent, 0.0, -1.0*zcomponent); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(-orientation*xcomponent, 0.0, zcomponent);   // no output face angle

  // lengths at different points
  G4double centralHalfLength  = length*0.5 - orientation*0.5*BPseparation*tan(fabs(angle*0.5)); // central axis of outer cylinder
  G4double secondBPHalfLength = length*0.5 - orientation*BPseparation*tan(fabs(angle*0.5));     // central axis of second beampipe
  
  G4double coilAngle = CLHEP::pi*0.75;
  G4double CoilOuterRadius = beamPipe->GetContainerRadius() + 2*lengthSafety + 118.6/2.0 * CLHEP::mm - 56.0/2.0*CLHEP::mm ;

  G4ThreeVector inputfaceCoil  = G4ThreeVector(-orientation*xcomponent, 0.0, -1.0*zcomponent); //-1 as pointing down in z for normal
  G4ThreeVector outputfaceCoil = G4ThreeVector(-orientation*xcomponent, 0.0, zcomponent);   // no output face angle

  // we make a lot of volumes in this class - keep a record for easily attaching properties to all
  std::vector<G4LogicalVolume*> allLogicalVolumes;
  
  if (beamPipe->ContainerIsCircular())
    {
      //circular beampipe so we can simply use its radius
      //container is similar but slightly wider and hollow (to allow placement of beampipe)
      //have to do subtraction as cuttubs aperture is central and the beampipe (active one) is not here
      G4VSolid* containerSolidOuter = new G4CutTubs(name + "_contiainer_solid_outer",  // name
						    0,                           // inner radius
						    boxSize*0.5,                 // outer radius
						    length*0.5,                  // half length
						    0,                           // rotation start angle
						    CLHEP::twopi,                // rotation finish angle
						    inputface,                   // input face normal
						    outputface);                 // output face normal
      G4VSolid* containerSolidInner = new G4Tubs(name + "_contiainer_solid_inner",  // name
						 0,                           // inner radius
						 beamPipe->GetContainerRadius() + lengthSafety, // outer radius
						 length,                      // full length for unambiguous subtraction
						 0,                           // rotation start angle
						 CLHEP::twopi);
      containerSolid = new G4SubtractionSolid(name + "_container_solid",   // name
					      containerSolidOuter,         // outer bit
					      containerSolidInner,         // subtract this from it
					      0,                           // rotation
					      -dipolePosition);            // translation
    }
  else
    {
      //container is similar but slightly wider
      G4VSolid* containerSolidOuter = new G4CutTubs(name + "_contiainer_solid_outer",  // name
						    0,                           // inner radius
						    boxSize*0.5,                 // outer radius
						    length*0.5,                  // half length
						    0,                           // rotation start angle
						    CLHEP::twopi,                // rotation finish angle
						    inputface,                   // input face normal
						    outputface);                 // output face normal
      containerSolid = new G4SubtractionSolid(name + "_container_solid",
					      containerSolidOuter,
					      beamPipe->GetContainerSubtractionSolid(),
					      0,                // rotation
					      -dipolePosition); // translation
    }

  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());
  G4LogicalVolume* containerLV = new G4LogicalVolume(containerSolid,
						     emptyMaterial,
						     name + "_container_lv");

  allLogicalVolumes.push_back(containerLV); //register it locally

  // coil solids
  G4VSolid *coil1 = new G4CutTubs(name+"_coil1_solid",            // name
				  beamPipe->GetContainerRadius() + 2*lengthSafety, // inner radius
				  CoilOuterRadius,                // outer radius
				  length*0.5-2*lengthSafety,      // length
				  -coilAngle/2.0,                 // start angle
				  coilAngle,                      // sweep angle
				  inputfaceCoil,                  // input face normal
				  outputfaceCoil);                // output face normal
  G4VSolid *coil2 = new G4CutTubs(name+"_coil2_solid",            // name
				  beamPipe->GetContainerRadius() + 2*lengthSafety, // inner radius
				  CoilOuterRadius,                // outer radius
				  length*0.5-2*lengthSafety,      // length
				  CLHEP::pi-coilAngle/2.0,        // start angle
				  coilAngle,                      // sweep angle
				  inputfaceCoil,                  // input face normal
				  outputfaceCoil);                // output face normal
  G4VSolid *coil3 = new G4CutTubs(name+"_coil3_solid",            // name
				  beamPipe->GetContainerRadius() + 2*lengthSafety, // inner radius
				  CoilOuterRadius,                // outer radius
				  secondBPHalfLength-2*lengthSafety, // length
				  -coilAngle/2.0,                 // start angle
				  coilAngle,                      // sweep angle
				  inputfaceCoil,                  // input face normal
				  outputfaceCoil);                // output face normal
  G4VSolid *coil4 = new G4CutTubs(name+"_coil4_solid",            // name
				  beamPipe->GetContainerRadius() + 2*lengthSafety, // inner radius
				  CoilOuterRadius,                // outer radius
				  secondBPHalfLength-2*lengthSafety, // length
				  CLHEP::pi-coilAngle/2.0,        // start angle
				  coilAngle,                      // sweep angle
				  inputfaceCoil,                  // input face normal
				  outputfaceCoil);                // output face normal

  // coil logical volumes
  G4Material* nbti = BDSMaterials::Instance()->GetMaterial("NbTi.1");
  G4LogicalVolume* coilLV1 =  new G4LogicalVolume(coil1,
						  nbti,
						  name+"_coil1_lv");
  G4LogicalVolume* coilLV2 =  new G4LogicalVolume(coil2,
						  nbti,
						  name+"_coil2_lv");
  G4LogicalVolume* coilLV3 =  new G4LogicalVolume(coil3,
						  nbti,
						  name+"_coil3_lv");
  G4LogicalVolume* coilLV4 =  new G4LogicalVolume(coil4,
						  nbti,
						  name+"_coil4_lv");

  // coil visualisation
  G4VisAttributes* coilVisAtt = new G4VisAttributes(G4Colour(0.9, 0.75, 0.)); //gold-ish colour
  coilVisAtt->SetForceSolid(true);
  coilLV1->SetVisAttributes(coilVisAtt);
  coilLV2->SetVisAttributes(coilVisAtt);
  coilLV3->SetVisAttributes(coilVisAtt);
  coilLV4->SetVisAttributes(coilVisAtt);

  allLogicalVolumes.push_back(coilLV1); // register locally
  allLogicalVolumes.push_back(coilLV2);
  allLogicalVolumes.push_back(coilLV3);
  allLogicalVolumes.push_back(coilLV4);

  // coil placement
  new G4PVPlacement(0,                      // rotation
		    -dipolePosition,        // position
		    coilLV1,                // its logical volume
		    name+"_coil1_pv",       // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());   
  
  new G4PVPlacement(0,                      // rotation
		    -dipolePosition,        // position
		    coilLV2,                // its logical volume
		    name+"_coil2_pv",       // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());  
  
  new G4PVPlacement(0,                      // rotation
		    dipolePosition,         // position
		    coilLV3,                // its logical volume
		    name+"_coil3_pv",       // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());   
  
  new G4PVPlacement(0,                      // rotation
		    dipolePosition,         // position
		    coilLV4,                // its logical volume
		    name+"_coil4_pv",       // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()); 
  
  // non-magnetic collars
  // collar pole solids
  G4VSolid* collar1PoleTopSolid    = new G4CutTubs(name+"_collar1_pole_top",
						   beamPipe->GetContainerRadius() + lengthSafety,
						   CoilOuterRadius-lengthSafety,
						   length*0.5 - 2*lengthSafety, 
						   coilAngle/2.,
						   CLHEP::pi - coilAngle,
						   inputfaceCoil,                   // input face normal
						   outputfaceCoil);                 // output face normal
  
  G4VSolid* collar1PoleBottomSolid = new G4CutTubs(name+"_collar1_pole_bottom",
						   beamPipe->GetContainerRadius() + lengthSafety,
						   CoilOuterRadius-lengthSafety,
						   length*0.5 - 2*lengthSafety,
						   CLHEP::pi + coilAngle/2.,
						   CLHEP::pi - coilAngle,
						   inputfaceCoil,                   // input face normal
						   outputfaceCoil);                 // output face normal
  
  G4VSolid* collar2PoleTopSolid    = new G4CutTubs(name+"_collar2_pole_top",
						   beamPipe->GetContainerRadius() + lengthSafety,
						   CoilOuterRadius-lengthSafety,
						   secondBPHalfLength-2*lengthSafety, 
						   coilAngle/2.,
						   CLHEP::pi - coilAngle,
						   inputfaceCoil,                   // input face normal
						   outputfaceCoil);                 // output face normal
  
  G4VSolid* collar2PoleBottomSolid = new G4CutTubs(name+"_collar2_pole_bottom",
						   beamPipe->GetContainerRadius() + lengthSafety,
						   CoilOuterRadius-lengthSafety,
						   secondBPHalfLength-2*lengthSafety,
						   CLHEP::pi + coilAngle/2.,
						   CLHEP::pi - coilAngle,
						   inputfaceCoil,                   // input face normal
						   outputfaceCoil);                 // output face normal
  
  // collar pole logical volumes
  G4LogicalVolume* collar1PoleTopLV    = new G4LogicalVolume(collar1PoleTopSolid,
							     BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
							     name+"_collar1_pole_top_lv");
  G4LogicalVolume* collar1PoleBottomLV = new G4LogicalVolume(collar1PoleBottomSolid,
							     BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
							     name+"_collar1_pole_bottom_lv");
  G4LogicalVolume* collar2PoleTopLV    = new G4LogicalVolume(collar2PoleTopSolid,
							     BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
							     name+"_collar2_pole_top_lv");
  G4LogicalVolume* collar2PoleBottomLV = new G4LogicalVolume(collar2PoleBottomSolid,
							     BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
							     name+"_collar2_pole_bottom_lv");

  // collar pole visualisation
  G4VisAttributes* collarVisAtt = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9)); //
  collarVisAtt->SetForceSolid(true);
  collar1PoleTopLV->SetVisAttributes(collarVisAtt);
  collar1PoleBottomLV->SetVisAttributes(collarVisAtt);
  collar2PoleTopLV->SetVisAttributes(collarVisAtt);
  collar2PoleBottomLV->SetVisAttributes(collarVisAtt);
  
  allLogicalVolumes.push_back(collar1PoleTopLV); // register locally
  allLogicalVolumes.push_back(collar1PoleBottomLV);
  allLogicalVolumes.push_back(collar2PoleTopLV);
  allLogicalVolumes.push_back(collar2PoleBottomLV);
  
  // collar pole placement
  new G4PVPlacement(0,                          // rotation
		    -dipolePosition,            // position
		    collar1PoleTopLV,           // its logical volume
		    name+"_collar1_pole_top_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                             // rotation
		    -dipolePosition,               // position
		    collar1PoleBottomLV,           // its logical volume
		    name+"_collar1_pole_bottom_pv",// its name
		    containerLV,                   // its mother  volume
		    false,                         // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                          // rotation
		    dipolePosition,             // position
		    collar2PoleTopLV,           // its logical volume
		    name+"_collar2_pole_top_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                             // rotation
		    dipolePosition,                // position
		    collar2PoleBottomLV,           // its logical volume
		    name+"_collar2_pole_bottom_pv",// its name
		    containerLV,                   // its mother  volume
		    false,                         // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps()); 
  
  // outer annulus of collar - two as slightly different lengths
  G4VSolid* collarAnnulus1 = new G4CutTubs(name+"_collar1_annulus_solid",      // name
					   CoilOuterRadius + lengthSafety,     // inner radius
					   collarOuterRadius - lengthSafety,   // outer radius
					   length*0.5-2*lengthSafety,          // length
					   0,                                  // starting angle
					   CLHEP::twopi,                       // angle of sweep
					   inputfaceCoil,                      // input face normal
					   outputfaceCoil);                    // output face normal
  
  G4VSolid* collarAnnulus2 = new G4CutTubs(name+"_collar_annulus_solid",       // name
					   CoilOuterRadius + lengthSafety,     // inner radius
					   collarOuterRadius - lengthSafety,   // outer radius
					   secondBPHalfLength-2*lengthSafety,  // length
					   0,                                  // starting angle
					   CLHEP::twopi,                       // angle of sweep
					   inputfaceCoil,                      // input face normal
					   outputfaceCoil);                    // output face normal

  // prepare a solid to cut a hole in the outer yoke volume (can just use one twice)
  // can't use the collarAnnuli as they're not solid - need them to be solid
  G4VSolid* collarSubtractionCylinder = new G4Tubs(name+"_collar_subtraction_solid",   // name
						   0,                                   // inner radius
						   collarOuterRadius,                   // outer radius
						   length,                              // double length for unambiguous subtraction
						   0,                                   // starting angle
						   CLHEP::twopi);                       // sweep angle

  G4VSolid* collarSubtractionCylinders = new G4UnionSolid(name + "_collar_subtraction_cylinders", // name
							  collarSubtractionCylinder,              // solid1
							  collarSubtractionCylinder,              // solid2 (here = solid1)
							  0,                                      // rotation
							  2*dipolePosition);                      // translation

  // made of two rings, but they overlap slightly in the middle
  G4VSolid* collarsAnnuliTogether = new G4UnionSolid(name+"_collars_annulus_solid",
						     collarAnnulus1,
						     collarAnnulus2,
						     0,
						     2*dipolePosition);

  G4LogicalVolume *collarsAnnuliTogetherLV =  new G4LogicalVolume(collarsAnnuliTogether,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collars_annuli_lv");

  // collar annulus visualisation attributes
  collarsAnnuliTogetherLV->SetVisAttributes(collarVisAtt);
  collarsAnnuliTogetherLV->SetVisAttributes(collarVisAtt);

  allLogicalVolumes.push_back(collarsAnnuliTogetherLV); // register locally
  
  new G4PVPlacement(0,                        // rotation
		    -dipolePosition,          // position
		    collarsAnnuliTogetherLV,  // its logical volume
		    name+"_collars_annlui_pv",// its name
		    containerLV,              // its mother  volume
		    false,                    // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());  
 
  // outer iron yoke
  G4VSolid* yokeCylinder = new G4CutTubs(name+"_yoke_cylinder_solid",     // name
					 0.,                              // inner radius
					 boxSize*0.5,                     // outer radius
					 centralHalfLength-2*lengthSafety, // length
					 0,                               // starting angle
					 CLHEP::twopi * CLHEP::rad,       // sweep angle
					 inputfaceCoil,                   // input face normal
					 outputfaceCoil);                 // output face normal

  G4VSolid* yoke = new G4SubtractionSolid(name+"_yoke_solid",             // name
					  yokeCylinder,                   // from this
					  collarSubtractionCylinders,     // subtract this
					  0,
					  -dipolePosition);               

  
  G4LogicalVolume* yokeLV = new G4LogicalVolume(yoke,
						BDSMaterials::Instance()->GetMaterial("Iron"),
						name+"_yoke_lv");

  // yoke visualisation
  G4VisAttributes* LHCblue = new G4VisAttributes(G4Colour(0.0, 0.5, 1.0));
  LHCblue->SetForceSolid(true);
  yokeLV->SetVisAttributes(LHCblue);
  
  allLogicalVolumes.push_back(yokeLV); // register locally

  // yoke placement
  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    G4ThreeVector(0,0,0),         // position
		    yokeLV,                       // lv to be placed
		    name + "_yoke_pv",            // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );

  G4String defaultMaterialName = BDSGlobalConstants::Instance()->GetBeamPipeMaterialName();
  G4Material* beamPipeMaterial = BDSMaterials::Instance()->GetMaterial(defaultMaterialName);
  G4Material* vacuumMaterial   = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial());

  //use beampipe factories to create another beampipe (note no magnetic field for now...)
  BDSBeamPipe* secondBP = BDSBeamPipeFactory::Instance()->CreateBeamPipeAngledInOut(BDSBeamPipeType::lhcdetailed,
										    name,
										    2*secondBPHalfLength-2*lengthSafety,
										    -angle*0.5,        // entrane angle
										    -angle*0.5,        // exit angle
										    4.404*CLHEP::cm,   // aper1
										    3.428*CLHEP::cm,   // aper2
										    4.404*CLHEP::cm,   // aper3
										    0,                 // aper4
										    vacuumMaterial,    // vacuum material
										    1*CLHEP::mm,       // beampipeThickness
										    beamPipeMaterial); // beampipe material

  G4LogicalVolume* secondBPLV = secondBP->GetContainerLogicalVolume();
  allLogicalVolumes.push_back(secondBPLV);
  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    dipolePosition,               // position
		    secondBPLV,                   // lv to be placed
		    name + "_second_beampipe_pv", // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );
  
  // visual attributes for container
#ifdef BDSDEBUG
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());
#else
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
#endif

  // USER LIMITS and SENSITIVITY for all components
#ifndef NOUSERLIMITS
  G4UserLimits* userLimits = new G4UserLimits("outer_cuts");
  G4double maxStepFactor = 0.5; // fraction of length for maximum step size
  userLimits->SetMaxAllowedStep( length * maxStepFactor );
  userLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  userLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
#endif
  for (std::vector<G4LogicalVolume*>::iterator i = allLogicalVolumes.begin(); i != allLogicalVolumes.end(); ++i)
    {
      (*i)->SetSensitiveDetector(BDSSDManager::Instance()->GetEnergyCounterOnAxisSD());
#ifndef NOUSERLIMITS
      (*i)->SetUserLimits(userLimits);
#endif
    }
  
  // record extents
  // container radius is the same for all methods as all cylindrical
  G4double containerRadius = boxSize + lengthSafety;
  // massShift defined at very beginning of this function
  std::pair<double,double> extX = std::make_pair(-containerRadius+massShift,containerRadius+massShift); 
  std::pair<double,double> extY = std::make_pair(-containerRadius,containerRadius);
  std::pair<double,double> extZ = std::make_pair(-length*0.5,length*0.5);
  
  // build the BDSGeometryComponent instance and return it
  BDSGeometryComponent* outer = new BDSGeometryComponent(containerSolid,
							 containerLV,
							 extX, extY, extZ,
							 dipolePosition);
  // REGISTER all lvs
  outer->RegisterLogicalVolumes(secondBP->GetAllLogicalVolumes());
  outer->RegisterLogicalVolumes(allLogicalVolumes);
  
  return outer;
}

BDSGeometryComponent* BDSMagnetOuterFactoryLHC::CreateRectangularBend(G4String      name,
									      G4double      length,
									      BDSBeamPipe*  beamPipe,
									      G4double      boxSize,
									      G4double      /*angle*/,
									      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  //rectangular bends currently just make a shorter straight volume, so ignore angle for now
  CreateCylindricalSolids(name, length, beamPipe, boxSize);
  return CommonFinalConstructor(name, length, boxSize, outerMaterial, BDSMagnetColours::Instance()->GetMagnetColour("rectangularbend"));
}

BDSGeometryComponent* BDSMagnetOuterFactoryLHC::CreateQuadrupole(G4String      name,
									 G4double      length,
									 BDSBeamPipe*  beamPipe,
									 G4double      boxSize,
									 G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CreateCylindricalSolids(name, length, beamPipe, boxSize);
  return CommonFinalConstructor(name, length, boxSize, outerMaterial, BDSMagnetColours::Instance()->GetMagnetColour("quadrupole"));
}

BDSGeometryComponent* BDSMagnetOuterFactoryLHC::CreateSextupole(G4String      name,
									G4double      length,
									BDSBeamPipe*  beamPipe,
									G4double      boxSize,
									G4Material*   outerMaterial)
{
  CreateCylindricalSolids(name, length, beamPipe, boxSize);
  return CommonFinalConstructor(name, length, boxSize, outerMaterial, BDSMagnetColours::Instance()->GetMagnetColour("sextupole"));
}

BDSGeometryComponent* BDSMagnetOuterFactoryLHC::CreateOctupole(G4String      name,
								       G4double      length,
								       BDSBeamPipe*  beamPipe,
								       G4double      boxSize,
								       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CreateCylindricalSolids(name, length, beamPipe, boxSize);
  return CommonFinalConstructor(name, length, boxSize, outerMaterial, BDSMagnetColours::Instance()->GetMagnetColour("octuupole"));
}

BDSGeometryComponent* BDSMagnetOuterFactoryLHC::CreateDecapole(G4String      name,
								       G4double      length,
								       BDSBeamPipe*  beamPipe,
								       G4double      boxSize,
								       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CreateCylindricalSolids(name, length, beamPipe, boxSize);
  return CommonFinalConstructor(name, length, boxSize, outerMaterial, BDSMagnetColours::Instance()->GetMagnetColour("decapole"));
}

BDSGeometryComponent* BDSMagnetOuterFactoryLHC::CreateSolenoid(G4String      name,
								       G4double      length,
								       BDSBeamPipe*  beamPipe,
								       G4double      boxSize,
								       G4Material*   outerMaterial)
{
  CreateCylindricalSolids(name, length, beamPipe, boxSize);
  return CommonFinalConstructor(name, length, boxSize, outerMaterial, BDSMagnetColours::Instance()->GetMagnetColour("solenoid"));
}

BDSGeometryComponent* BDSMagnetOuterFactoryLHC::CreateMultipole(G4String      name,
									G4double      length,
									BDSBeamPipe*  beamPipe,
									G4double      boxSize,
									G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CreateCylindricalSolids(name, length, beamPipe, boxSize);
  return CommonFinalConstructor(name, length, boxSize, outerMaterial, BDSMagnetColours::Instance()->GetMagnetColour("multipole"));
}

BDSGeometryComponent* BDSMagnetOuterFactoryLHC::CreateRfCavity(G4String      name,
								       G4double      length,
								       BDSBeamPipe*  beamPipe,
								       G4double      boxSize,
								       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CreateCylindricalSolids(name, length, beamPipe, boxSize);
  return CommonFinalConstructor(name, length, boxSize, outerMaterial, BDSMagnetColours::Instance()->GetMagnetColour("rfcavity"));
}

BDSGeometryComponent* BDSMagnetOuterFactoryLHC::CreateMuSpoiler(G4String      name,
								       G4double      length,
								       BDSBeamPipe*  beamPipe,
								       G4double      boxSize,
								       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CreateCylindricalSolids(name, length, beamPipe, boxSize);
  return CommonFinalConstructor(name, length, boxSize, outerMaterial, BDSMagnetColours::Instance()->GetMagnetColour("muspoiler"));
}

BDSGeometryComponent* BDSMagnetOuterFactoryLHC::CreateKicker(G4String      name,
								     G4double      length,
								     BDSBeamPipe*  beamPipe,
								     G4double      boxSize,
								     G4bool        /*vertical*/,
								     G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // in this factory, h and v kickers will look the same so ignore bool vertical
  // have to retain it though for virtual base class compatability
  CreateCylindricalSolids(name, length, beamPipe, boxSize);
  return CommonFinalConstructor(name, length, boxSize, outerMaterial, BDSMagnetColours::Instance()->GetMagnetColour("hkicker"));
}

/// functions below here are private to this particular factory

void BDSMagnetOuterFactoryLHC::CreateCylindricalSolids(G4String     name,
							       G4double     length,
							       BDSBeamPipe* beamPipe,
							       G4double     boxSize)
{
  if (beamPipe->ContainerIsCircular())
    {
      //circular beampipe so we can simply use its radius
      outerSolid = new G4Tubs(name + "_outer_solid",       // name
			      beamPipe->GetContainerRadius() + 2*lengthSafety,  // inner radius
			      boxSize*0.5,                 // outer radius
			      length*0.5-2*lengthSafety,   // half length
			      0,                           // rotation start angle
			      CLHEP::twopi);               // rotation finish angle

      //container is similar but slightly wider and hollow (to allow placement of beampipe)
      containerSolid = new G4Tubs(name + "_contiainer_solid",  // name
				  beamPipe->GetContainerRadius() + lengthSafety, // inner radius
				  boxSize*0.5 + lengthSafety,  // outer radius
				  length*0.5,                  // half length
				  0,                           // rotation start angle
				  CLHEP::twopi);               // rotation finish angle
    }
  else
    {
      G4VSolid* outerSolidCylinder = new G4Tubs(name + "_outer_solid_cylinder",  // name
						0,  // inner radius - for unambiguous subtraction
						boxSize*0.5,                 // outer radius
						length*0.5-2*lengthSafety,   // half length
						0,                           // rotation start angle
						CLHEP::twopi);               // rotation finish angle
      outerSolid = new G4SubtractionSolid(name + "_outer_solid",
					  outerSolidCylinder,
					  beamPipe->GetContainerSubtractionSolid());

      //container is similar but slightly wider
      G4VSolid* containerSolidCylinder = new G4Tubs(name + "_container_solid_cylinder", // name
						    0,  // inner radius - for unambiguous subtraction
						    boxSize*0.5 + lengthSafety,  // outer radius
						    length*0.5,                  // half length
						    0,                           // rotation start angle
						    CLHEP::twopi);               // rotation finish angle
      containerSolid = new G4SubtractionSolid(name + "_container_solid",
					      containerSolidCylinder,
					      beamPipe->GetContainerSubtractionSolid());
    }
}

void BDSMagnetOuterFactoryLHC::TestInputParameters(BDSBeamPipe* beamPipe,
							   G4double&    boxSize,
							   G4Material*& outerMaterial)// reference to a pointer
{
  //function arguments by reference to they can be modified in place
  //check outer material is something
  if (!outerMaterial)
    {outerMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());}

  // ensure box size is bigger than the beampipe
  if (beamPipe->ContainerIsCircular()) {
    // if it's circular, just check radius
    if (boxSize < 2*(beamPipe->GetContainerRadius()) )
      {boxSize = 2*(beamPipe->GetContainerRadius()) + 1*CLHEP::mm;}
  } else {
    // it's not circular - have a look at extents
    // +ve - -ve
    G4double extentX = beamPipe->GetExtentX().second - beamPipe->GetExtentX().first;
    G4double extentY = beamPipe->GetExtentY().second - beamPipe->GetExtentY().first;
    if ( (boxSize < extentX) || (boxSize < extentY) ) {
      // boxSize isn't sufficient for range in x or y
      boxSize = std::max(extentX,extentY) + 1*CLHEP::mm;
    }
  }
    
}

/// only the solids are unique, once we have those, the logical volumes and placement in the
/// container are the same.  group all this functionality together
BDSGeometryComponent* BDSMagnetOuterFactoryLHC::CommonFinalConstructor(G4String    name,
									       G4double    length,
									       G4double    boxSize,
									       G4Material* outerMaterial,
									       G4Colour*   colour)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  
  // build the logical volumes
  G4LogicalVolume* outerLV   = new G4LogicalVolume(outerSolid,
						   outerMaterial,
						   name + "_outer_lv");

  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());
  G4LogicalVolume* containerLV = new G4LogicalVolume(containerSolid,
  						     emptyMaterial,
  						     name + "_container_lv");
  
  // VISUAL ATTRIBUTES
  // set visual attributes
  // outer
  G4VisAttributes* outerVisAttr = new G4VisAttributes(*colour);
  outerVisAttr->SetVisibility(true);
  outerVisAttr->SetForceSolid(true);
  outerLV->SetVisAttributes(outerVisAttr);
  // container
#ifdef BDSDEBUG
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());
#else
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
#endif

  // SENSITIVITY
  // make the outer sensitive if required (attachd Sensitive Detector Class)
  outerLV->SetSensitiveDetector(BDSSDManager::Instance()->GetEnergyCounterOnAxisSD());

  // USER LIMITS
  // set user limits based on bdsim user specified parameters

#ifndef NOUSERLIMITS
  G4UserLimits* outerUserLimits = new G4UserLimits("outer_cuts");
  G4double maxStepFactor = 0.5; // fraction of length for maximum step size
  outerUserLimits->SetMaxAllowedStep( length * maxStepFactor );
  outerUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  outerUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  //attach cuts to volumes
  outerLV->SetUserLimits(outerUserLimits);
  containerLV->SetUserLimits(outerUserLimits);
#endif

  // PLACEMENT
  // place the components inside the container
  // note we don't need the pointer for anything - it's registered upon construction with g4
  
  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    (G4ThreeVector)0,             // position
		    outerLV,                      // lv to be placed
		    name + "_outer_pv",           // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );
  
  // record extents
  // container radius is the same for all methods as all cylindrical
  G4double containerRadius = boxSize + lengthSafety;
  std::pair<double,double> extX = std::make_pair(-containerRadius,containerRadius);
  std::pair<double,double> extY = std::make_pair(-containerRadius,containerRadius);
  std::pair<double,double> extZ = std::make_pair(-length*0.5,length*0.5);
  
  // build the BDSGeometryComponent instance and return it
  BDSGeometryComponent* outer = new BDSGeometryComponent(containerSolid,
							 containerLV,
							 extX, extY, extZ);
  // REGISTER all lvs
  outer->RegisterLogicalVolume(outerLV); //using geometry component base class method
  outer->RegisterLogicalVolume(containerLV);
  
  return outer;
}
