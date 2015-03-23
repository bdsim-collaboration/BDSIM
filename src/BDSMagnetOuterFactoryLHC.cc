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
  
  /// Mass to the Right or to the Left
  G4ThreeVector dipolePosition;
  G4double      massShift = 97.26*CLHEP::mm;

  if (isLeftOffset)
    {
      dipolePosition = G4ThreeVector(massShift,0.,0.);
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "dipole to the Right" << G4endl;
#endif
    }
  else
    {
      dipolePosition = G4ThreeVector(-massShift,0.,0.);
      massShift *= -1;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "dipole to the Left" << G4endl;
#endif
    }
  /*
  G4cout << "Length = " << 0.5*length/CLHEP::m << G4endl;
  G4cout << "Length = " << beamPipe->GetExtentZ().second/CLHEP::m << G4endl;
  */
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
				 boxSize*0.5,                 // outer radius
				 length*0.5-2*lengthSafety,   // half length
				 0,                           // rotation start angle
				 CLHEP::twopi,                // rotation finish angle
				 inputface,                   // input face normal
				 outputface);                 // output face normal

      //container is similar but slightly wider and hollow (to allow placement of beampipe)
      containerSolid = new G4CutTubs(name + "_contiainer_solid",  // name
				     beamPipe->GetContainerRadius() + lengthSafety, // inner radius
				     boxSize*0.5 + lengthSafety,  // outer radius
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
						   boxSize*0.5,                 // outer radius
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
						       boxSize*0.5 + lengthSafety,  // outer radius
						       length*0.5,                  // half length
						       0,                           // rotation start angle
						       CLHEP::twopi,                // rotation finish angle
						       inputface,                   // input face normal
						       outputface);                 // output face normal
      containerSolid = new G4SubtractionSolid(name + "_container_solid",
					      containerSolidCylinder,
					      beamPipe->GetContainerSubtractionSolid());
    }

  // Superconducting coils
  G4double BPseparation = 2*97.26*CLHEP::mm;
  G4ThreeVector positionBeamPipeCoil1 = G4ThreeVector(0.,0.,0.); 
  G4ThreeVector positionBeamPipeCoil2 = G4ThreeVector(2*massShift,0.,0.); 

  G4double coilAngle = CLHEP::pi*3./4. * CLHEP::rad;
  G4double CoilOuterRadius = beamPipe->GetContainerRadius() + 2*lengthSafety + 118.6/2.0 * CLHEP::mm - 56.0/2.0*CLHEP::mm ;

  G4ThreeVector inputfaceCoil  = G4ThreeVector(-orientation*xcomponent, 0.0, -1.0*zcomponent); //-1 as pointing down in z for normal
  G4ThreeVector outputfaceCoil = G4ThreeVector(-orientation*xcomponent, 0.0, zcomponent);   // no output face angle

  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());
  G4LogicalVolume *containerLV = new G4LogicalVolume(containerSolid,
						     emptyMaterial,
						     name + "_container_lv");

  G4VSolid *coil1 = new G4CutTubs(name+"_coil_tmp_1",
				 beamPipe->GetContainerRadius() + 2*lengthSafety,
				 CoilOuterRadius,          
				 length*0.5-2*lengthSafety,             // length
				 -coilAngle/2.0,
				 coilAngle,
				 inputfaceCoil,                   // input face normal
				 outputfaceCoil);                 // output face normal

  G4LogicalVolume *CoilLV1 =  new G4LogicalVolume(coil1,
						 BDSMaterials::Instance()->GetMaterial("NbTi.1"),
						 name+"_coil");


  G4VSolid *coil2 = new G4CutTubs(name+"_coil_tmp_1",
				  beamPipe->GetContainerRadius() + 2*lengthSafety,
				  CoilOuterRadius,          
				  length*0.5-2*lengthSafety,             // length
				  CLHEP::pi-coilAngle/2.0,    
				  coilAngle,
				  inputfaceCoil,                   // input face normal
				  outputfaceCoil);                 // output face normal

  G4LogicalVolume *CoilLV2 =  new G4LogicalVolume(coil2,
						 BDSMaterials::Instance()->GetMaterial("NbTi.1"),
						 name+"_coil");


  G4VSolid *coil3 = new G4CutTubs(name+"_coil_tmp_1",
				 beamPipe->GetContainerRadius() + 2*lengthSafety,
				 CoilOuterRadius,          
				 length*0.5+BPseparation*tan(fabs(angle*0.5))-2*lengthSafety,             // length
				 -coilAngle/2.0,
				 coilAngle,
				 inputfaceCoil,                   // input face normal
				 outputfaceCoil);                 // output face normal


  G4LogicalVolume *CoilLV3 =  new G4LogicalVolume(coil3,
						 BDSMaterials::Instance()->GetMaterial("NbTi.1"),
						 name+"_coil");

  G4VSolid *coil4 = new G4CutTubs(name+"_coil_tmp_1",
				  beamPipe->GetContainerRadius() + 2*lengthSafety,
				  CoilOuterRadius,          
				  length*0.5+BPseparation*tan(fabs(angle*0.5))-2*lengthSafety,             // length
				  CLHEP::pi-coilAngle/2.0,    
				  coilAngle,
				  inputfaceCoil,                   // input face normal
				  outputfaceCoil);                 // output face normal

  G4LogicalVolume *CoilLV4 =  new G4LogicalVolume(coil4,
						 BDSMaterials::Instance()->GetMaterial("NbTi.1"),
						 name+"_coil");


  new G4PVPlacement(0,                 // rotation
		    positionBeamPipeCoil1,                      // at (0,0,0)
		    CoilLV1,  // its logical volume
		    name+"_solid_PV1",       // its name
		    containerLV, // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());   

  
  new G4PVPlacement(0,               // rotation
		    positionBeamPipeCoil1,                      // at (0,0,0)
		    CoilLV2,  // its logical volume
		    name+"_solid_PV2",       // its name
		    containerLV, // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());  

  
  new G4PVPlacement(0,                 // rotation
		    positionBeamPipeCoil2,                      // at (0,0,0)
		    CoilLV3,  // its logical volume
		    name+"_solid_PV3",       // its name
		    containerLV, // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());   
  
  new G4PVPlacement(0,               // rotation
		    positionBeamPipeCoil2,                      // at (0,0,0)
		    CoilLV4,  // its logical volume
		    name+"_solid_PV4",       // its name
		    containerLV, // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()); 

  
  // color-coding for the pole
  G4VisAttributes* VisAtt = 
    new G4VisAttributes(G4Colour(0.9, 0.75, 0.));
  VisAtt->SetForceSolid(true);
  CoilLV1->SetVisAttributes(VisAtt);
  CoilLV2->SetVisAttributes(VisAtt);
  CoilLV3->SetVisAttributes(VisAtt);
  CoilLV4->SetVisAttributes(VisAtt);
  
  // non-magnetic collars
  G4double collarOuterR = 101.18 * CLHEP::mm;
  
  G4VSolid *collar_up1 = new G4CutTubs(name+"_coilar_tmp_up",
				      beamPipe->GetContainerRadius() + 2*lengthSafety,
				      CoilOuterRadius-2*lengthSafety,
				      length*0.5-2*lengthSafety, 
				      coilAngle/2.,
				      (CLHEP::pi*CLHEP::rad - coilAngle),
				      inputfaceCoil,                   // input face normal
				      outputfaceCoil);                 // output face normal
  
  G4VSolid *collar_down1 = new G4CutTubs(name+"_coilar_tmp_down",
				     beamPipe->GetContainerRadius() + 2*lengthSafety,
				     CoilOuterRadius-2*lengthSafety,
				     length*0.5-2*lengthSafety,
				     CLHEP::pi * CLHEP::rad + coilAngle/2.,
				     (CLHEP::pi * CLHEP::rad - coilAngle),
				     inputfaceCoil,                   // input face normal
				     outputfaceCoil);                 // output face normal

  G4VSolid *collar_up2 = new G4CutTubs(name+"_coilar_tmp_up",
				      beamPipe->GetContainerRadius() + 2*lengthSafety,
				      CoilOuterRadius-2*lengthSafety,
				      length*0.5+BPseparation*tan(fabs(angle*0.5))-2*lengthSafety, 
				      coilAngle/2.,
				      (CLHEP::pi*CLHEP::rad - coilAngle),
				      inputfaceCoil,                   // input face normal
				      outputfaceCoil);                 // output face normal
  
  G4VSolid *collar_down2 = new G4CutTubs(name+"_coilar_tmp_down",
				     beamPipe->GetContainerRadius() + 2*lengthSafety,
				     CoilOuterRadius-2*lengthSafety,
				     length*0.5+BPseparation*tan(fabs(angle*0.5))-2*lengthSafety,
				     CLHEP::pi * CLHEP::rad + coilAngle/2.,
				     (CLHEP::pi * CLHEP::rad - coilAngle),
				     inputfaceCoil,                   // input face normal
				     outputfaceCoil);                 // output face normal

  G4VSolid *collar01 = new G4UnionSolid(name+"_solid_tmp_0",
					   collar_up1,
					   collar_down1);

  G4VSolid *collar02 = new G4UnionSolid(name+"_solid_tmp_0",
					   collar_up2,
					   collar_down2);
  
  G4VSolid *collarOuter1 = new G4CutTubs(name+"collar_volume",
					 CoilOuterRadius+2*lengthSafety,
					 collarOuterR-2*lengthSafety,          // outer R
					 length*0.5-2*lengthSafety,             // length
					 0*0.5-2*lengthSafety,                  // starting p
					 CLHEP::twopi * CLHEP::rad,
					 inputfaceCoil,                   // input face normal
					 outputfaceCoil);                 // output face normal

  G4VSolid *collarOuter2 = new G4CutTubs(name+"collar_volume",
					 CoilOuterRadius+2*lengthSafety,
					 collarOuterR-2*lengthSafety,          // outer R
					 length*0.5+BPseparation*tan(fabs(angle*0.5))-2*lengthSafety,             // length
					 0*0.5-2*lengthSafety,                  // starting p
					 CLHEP::twopi * CLHEP::rad,
					 inputfaceCoil,                   // input face normal
					 outputfaceCoil);                 // output face normal
    
  G4VSolid *collar1 = new G4UnionSolid(name+"_solid_tmp_0",
					   collar01,
					   collarOuter1);

  G4LogicalVolume *collarLV1 =  new G4LogicalVolume(collar1,
						   BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
						   name+"_collar");
  
  G4VSolid *collar2 = new G4UnionSolid(name+"_solid_tmp_0",
					   collar02,
					   collarOuter2);

  G4LogicalVolume *collarLV2 =  new G4LogicalVolume(collar2,
						   BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
						   name+"_collar");

  new G4PVPlacement(0,                 // rotation
		    positionBeamPipeCoil1,                      // at (0,0,0)
		    collarLV1,  // its logical volume
		    name+"_collar1",       // its name
		    containerLV, // its mother  volume
		    false,                  // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());   

  new G4PVPlacement(0,                 // rotation
		    positionBeamPipeCoil2,                      // at (0,0,0)
		    collarLV2,  // its logical volume
		    name+"_collar2",       // its name
		    containerLV, // its mother  volume
		    false,                  // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());   

  // color-coding for the pole
  G4VisAttributes* VisAtt2 = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9)); //
  VisAtt2->SetForceSolid(true);
  collarLV1->SetVisAttributes(VisAtt2);
  collarLV2->SetVisAttributes(VisAtt2);
  
  //G4double posBP = 97.26*CLHEP::mm;
  G4double posBP = massShift;

  G4ThreeVector positionBeamPipe1 = G4ThreeVector(-posBP,0.,0.); 
  G4ThreeVector positionBeamPipe2 = G4ThreeVector(posBP,0.,0.); 

  G4VSolid *magTubsEnv1 = new G4CutTubs(name+"_solid_tmp_1",
					0.,
					boxSize*0.5,          // outer R
					length*0.5+BPseparation*tan(fabs(angle*0.5))/2.0-2*lengthSafety,             // length
					0,                  // starting phi
					CLHEP::twopi * CLHEP::rad,
					inputfaceCoil,                   // input face normal
					outputfaceCoil);                 // output face normal

  G4VSolid *magTubsEnv2 = new G4SubtractionSolid(name+"_solid_env",
						magTubsEnv1,
						new G4CutTubs(name+"collar_volume",
							      0,
							      collarOuterR+2*lengthSafety,          // outer R
							      length,             // length
							      0,                  // starting p
							      CLHEP::twopi * CLHEP::rad,
							      inputfaceCoil,                   // input face normal
							      outputfaceCoil),                 // output face normal,
						 0,
						 positionBeamPipe1						
						 ); 

  G4VSolid *magTubs = new G4SubtractionSolid(name+"_solid_env",
					     magTubsEnv2,
					     new G4CutTubs(name+"collar_volume",
							   0,
							   collarOuterR+2*lengthSafety,          // outer R
							   length,             // length
							   0,                  // starting p
							   CLHEP::twopi * CLHEP::rad,
							   inputfaceCoil,                   // input face normal
							   outputfaceCoil),                 // output face normal
					     0,
					     positionBeamPipe2						
					     ); 
  
  G4LogicalVolume *yokeLV = new G4LogicalVolume(magTubs,
					      BDSMaterials::Instance()->GetMaterial("Iron"),
					      name+"_outer");

  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    positionBeamPipe2,             // position
		    yokeLV,                       // lv to be placed
		    name + "_outer_LHC_pv",           // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );

  G4VisAttributes* LHCblue = new G4VisAttributes(G4Colour(0.0, 0.5, 1.0));
  LHCblue->SetForceSolid(true);
  yokeLV->SetVisAttributes(LHCblue);

  G4String defaultMaterialName = BDSGlobalConstants::Instance()->GetBeamPipeMaterialName();
  G4Material* beamPipeMaterial = BDSMaterials::Instance()->GetMaterial(defaultMaterialName);

  G4Material* vacuumMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial());

  //use beampipe factories to create another beampipe (note no magnetic field for now...)
  BDSBeamPipe* secondBP = BDSBeamPipeFactory::Instance()->CreateBeamPipeAngledInOut(BDSBeamPipeType::lhcdetailed,
										    name,
										    2*(length*0.5+BPseparation*tan(fabs(angle*0.5))-2*lengthSafety),
										    -angle*0.5,        // entrane angle
										    -angle*0.5,        // exit angle
										    4.404*CLHEP::cm,   // aper1
										    3.428*CLHEP::cm,   // aper2
										    4.404*CLHEP::cm,   // aper3
										    0,                 // aper4
										    vacuumMaterial,    // vacuum material
										    1*CLHEP::mm,       // beampipeThickness
										    beamPipeMaterial); // beampipe material

  G4LogicalVolume *secondBPLV = secondBP->GetContainerLogicalVolume();


  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    2*positionBeamPipe2,             // position
		    secondBPLV,                       // lv to be placed
		    name + "_second_BP",           // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );
  

  

  // build the logical volumes
  G4LogicalVolume* outerLV   = new G4LogicalVolume(outerSolid,
						   outerMaterial,
						   name + "_outer_lv");

  /*
  G4LogicalVolume* containerLV = new G4LogicalVolume(containerSolid,
  						     emptyMaterial,
  						     name + "_container_lv");
  */
  // VISUAL ATTRIBUTES
  // set visual attributes
  // outer
  G4VisAttributes* outerVisAttr = new G4VisAttributes(*(BDSMagnetColours::Instance()->GetMagnetColour("sectorbend")));
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
  /*
  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    (G4ThreeVector)0,             // position
		    outerLV,                      // lv to be placed
		    name + "_outer_pv",           // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );
  */
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

  //return CommonFinalConstructor(name,length,boxSize,outerMaterial,);
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
  /*
  new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
		    (G4ThreeVector)0,             // position
		    outerLV,                      // lv to be placed
		    name + "_outer_pv",           // name
		    containerLV,                  // mother lv to be place in
		    false,                        // no boolean operation
		    0,                            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() // whether to check overlaps
		    );
  */
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
