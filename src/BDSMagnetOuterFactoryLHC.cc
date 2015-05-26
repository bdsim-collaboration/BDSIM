#include "BDSMagnetOuterFactoryLHC.hh"

#include "BDSBeamPipe.hh"
#include "BDSBeamPipeType.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetColours.hh"
#include "BDSMaterials.hh"
#include "BDSSDManager.hh"
#include "BDSUtilities.hh"                 // for calculateorientation

#include "globals.hh"                      // geant4 globals / types
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4CutTubs.hh"
#include "G4IntersectionSolid.hh"
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
#include <cmath>
#include <utility>                         // for std::pair
#include <algorithm>                       // for std::max
#include <vector>


BDSMagnetOuterFactoryLHC::BDSMagnetOuterFactoryLHC(G4bool isLeftOffsetIn):
  isLeftOffset(isLeftOffsetIn)
{
  lengthSafety       = BDSGlobalConstants::Instance()->GetLengthSafety();
  nSegmentsPerCircle = 50;
  outerSolid         = NULL;
  containerSolid     = NULL;
}

BDSGeometryComponent* BDSMagnetOuterFactoryLHC::CreateSectorBend(G4String      name,
								 G4double      length,
								 BDSBeamPipe*  beamPipe,
								 G4double      outerDiameter,
								 G4double      angle,
								 G4Material*   outerMaterial)

{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  // test input parameters - set global options as default if not specified
  TestInputParameters(beamPipe,outerDiameter,outerMaterial);

  // nominal lhc beampipe parameters for reference
  // aper1 = 4.404cm / 2
  // aper2 = 3.428cm / 2
  // aper3 = 4.404cm / 2
  // containerRadius -> 24.599mm

  // geometrical constants
  // [1] LHC design report - Chapter 7, fig 7.3
  // [2] LHC design report - Chapter 7, fig 7.1
  G4double beamPipeAxisSeparation = 194.00*CLHEP::mm;             // at 1.9K
  G4double massShift              = 0.5 * beamPipeAxisSeparation; // central shift to geometry
  G4double collarBoxHalfHeight    = 60*CLHEP::mm;                 // [1] by visual inspection
  G4double collarBoxHalfWidth     = 22*CLHEP::mm;                 // fits between outer coils

  // radii
  G4double containerInnerRadius   = beamPipe->GetContainerRadius()+1*CLHEP::um;
  G4double innerCoilInnerRadius   = containerInnerRadius + lengthSafety;
  G4double innerCoilInnerRadiusF  = 24.601*CLHEP::mm; // the fixed radius for the second pipe - F for fixed
  G4double innerCoilOuterRadius   = 42*CLHEP::mm;                        // [2] by visual inspection
  G4double outerCoilInnerRadius   = innerCoilOuterRadius + lengthSafety;
  G4double outerCoilInnerRadiusF  = innerCoilOuterRadius + lengthSafety; // doesn't change
  G4double outerCoilOuterRadius   = 60*CLHEP::mm;                        // [2] by visual inspection
  G4double collarInnerRadius      = outerCoilOuterRadius + lengthSafety;
  G4double collarInnerRadiusF     = outerCoilOuterRadius + lengthSafety;
  G4double collarOuterRadius      = 101.18*CLHEP::mm;                    // [1] - at 293K but don't have 1.9K measurement

  // angular setup of coils
  // these angles were calculated by visually analysing the coil layout graph in [2]
  G4double poleInnerFullAngle = 33./180.*2; //33 degrees half angle in radians
  G4double poleOuterFullAngle = 100./180.*2; //80 degrees half angle in radians
  G4double coilInnerFullAngle = CLHEP::pi - poleInnerFullAngle - 0.001; // 0.001 is small margin to avoid overlap
  G4double coilOuterFullAngle = CLHEP::pi - poleOuterFullAngle - 0.001;

  // whether to build various components around active beam pipe depending on how wide it is
  // these ONLY apply to the components around the active beampipe
  G4bool buildInnerCoil           = true;
  G4bool buildOuterCoil           = true;
  G4bool buildCollar              = true; // collar around the active beam pipe
  if (innerCoilInnerRadius > innerCoilOuterRadius)
    {buildInnerCoil = false;}
  if ((innerCoilInnerRadius > outerCoilInnerRadius) && (innerCoilInnerRadius < outerCoilOuterRadius))
    {outerCoilInnerRadius = containerInnerRadius + lengthSafety;}
  if (innerCoilInnerRadius > outerCoilOuterRadius)
    {buildOuterCoil = false;}
  if ((innerCoilInnerRadius > collarInnerRadius) && (innerCoilInnerRadius < (massShift - collarBoxHalfWidth)))
    {collarInnerRadius = containerInnerRadius + lengthSafety;}
  if (innerCoilInnerRadius > (massShift - collarBoxHalfWidth))
    {buildCollar = false;}
  if (innerCoilInnerRadius > collarOuterRadius)
    {
      // pipe is too big to use with this geometry!
      G4cerr << __METHOD_NAME__ << "this beam pipe is too big to use with the LHC dipole geometry" << G4endl;
      G4cerr << "Please consider using a different magnet geometry for this particular magnet" << G4endl;
      exit(1);
    }
  
  
  /*
  G4double massShift          = 97*CLHEP::mm; // at 1.9K
  G4double beamPipeAxisSeparation       = 2*massShift;
  G4double coilFullThickness  = 118.6/2.0 - 56.0/2.0; // 41.3mm for two rows of coils, mm by default (for dipole)
  G4double innerCoilThickness = 0.5*coilFullThickness;
  G4double outerCoilThickness = 0.5*coilFullThickness;
  G4double collarThickness    = 0.9*coilFullThickness;
  
  // mass to the right or to the left
  G4ThreeVector dipolePosition;

  // inner radius of container cut out for active beam pipe
  G4double containerInnerRadius = beamPipe->GetContainerRadius() + 1*CLHEP::um;

  

  // radial geometrical parameters for the coils & collars  
  G4double innerCoilInnerRadius = containerInnerRadius + 1*CLHEP::um;
  G4double innerCoilOuterRadius = innerCoilInnerRadius + innerCoilThickness;
  G4double outerCoilInnerRadius = innerCoilOuterRadius + 1*CLHEP::um;
  G4double outerCoilOuterRadius = outerCoilInnerRadius + outerCoilThickness;
  G4double collarInnerRadius    = outerCoilOuterRadius + 1*CLHEP::um;
  G4double collarOuterRadius    = collarInnerRadius    + collarThickness;

  
  // ensure that the collars touch each other - set minimum for outer radius
  if (collarOuterRadius < massShift)
    {collarOuterRadius = 1.1*massShift;}

  if (collarInnerRadius > (2*massShift - collarOuterRadius))
    {// this means there may be overlapping volumes..
      // would be even more complicated to be tolerant of this and optionally build coils to
      // fit it all in
      // this will be used primarily for lhc models within certain bounds - just print warning

      std::string theException = "\nWarning the beam pipe is sufficiently large to cause ";
      theException += "  overlapping volumes with the  non-magnetic collars - tracking problems";
      theException += " will occur for secondaires in this magnet. consider using another";
      theException += " type of magnet geometry.\n";
      G4cerr << theException << G4endl;
    }
  */

  G4ThreeVector dipolePosition; // translation of whole assembly relative to centre of active pipe
  if (isLeftOffset)
    {
      dipolePosition = G4ThreeVector(massShift,0.,0.);
      beamPipeAxisSeparation  *= -1;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "dipole to the left" << G4endl;
#endif
    }
  else
    {
      dipolePosition = G4ThreeVector(-massShift,0.,0.);
      //massShift     *= -1;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "dipole to the right" << G4endl;
#endif
    }

    // calculate some geometrical parameters
  G4int orientation   = BDS::CalculateOrientation(angle);
  G4double zcomponent = cos(fabs(angle*0.5)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double xcomponent = sin(fabs(angle*0.5)); // note full angle here as it's the exit angle
  G4ThreeVector inputface  = G4ThreeVector(-orientation*xcomponent, 0.0, -1.0*zcomponent); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(-orientation*xcomponent, 0.0, zcomponent);   // no output face angle

  // lengths at different points transversely - dependent on left or right geometry as well as angle +ve or -ve
  G4double centralHalfLength  = length*0.5 - orientation*0.5*beamPipeAxisSeparation*tan(fabs(angle*0.5)); // central axis of outer cylinder
  G4double secondBPHalfLength = length*0.5 - orientation*beamPipeAxisSeparation*tan(fabs(angle*0.5));     // central axis of second beampipe
  
  // we make a lot of volumes in this class - keep a record for easily attaching properties to all
  std::vector<G4LogicalVolume*> allLogicalVolumes;
  
  if (beamPipe->ContainerIsCircular())
    {
      //circular beampipe so we can simply use its radius
      //container is similar but slightly wider and hollow (to allow placement of beampipe)
      //have to do subtraction as cuttubs aperture is central and the beampipe (active one) is not here
      G4VSolid* containerSolidOuter = new G4CutTubs(name + "_contiainer_solid_outer",  // name
						    0,                           // inner radius
						    outerDiameter*0.5,           // outer radius
						    centralHalfLength,           // half length
						    0,                           // rotation start angle
						    CLHEP::twopi,                // rotation finish angle
						    inputface,                   // input face normal
						    outputface);                 // output face normal
      G4VSolid* containerSolidInner = new G4Tubs(name + "_contiainer_solid_inner",  // name
						 0,                                 // inner radius
						 containerInnerRadius,              // outer radius
						 length,                            // full length for unambiguous subtraction
						 0,                                 // rotation start angle
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
						    0,                                 // inner radius
						    outerDiameter*0.5,                 // outer radius
						    centralHalfLength,                 // half length
						    0,                                 // rotation start angle
						    CLHEP::twopi,                      // rotation finish angle
						    inputface,                         // input face normal
						    outputface);                       // output face normal
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
  G4VSolid*        coil1Inner   = NULL;
  G4VSolid*        coil1Outer   = NULL;
  G4VSolid*        coil2Inner   = NULL;
  G4VSolid*        coil2Outer   = NULL;
  G4LogicalVolume* coil1InnerLV = NULL;
  G4LogicalVolume* coil1OuterLV = NULL;
  G4LogicalVolume* coil2InnerLV = NULL;
  G4LogicalVolume* coil2OuterLV = NULL;
  G4Material*      nbti         = BDSMaterials::Instance()->GetMaterial("NbTi.1");
  G4VisAttributes* coilVisAtt   = new G4VisAttributes(G4Colour(0.9, 0.75, 0.)); //gold-ish colour
  coilVisAtt->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  G4VSolid*        collar1PoleTopInnerSolid     = NULL;
  G4VSolid*        collar1PoleBottomInnerSolid  = NULL;
  G4VSolid*        collar1PoleTopOuterSolid     = NULL;
  G4VSolid*        collar1PoleBottomOuterSolid  = NULL;
  G4LogicalVolume* collar1PoleTopInnerLV        = NULL;
  G4LogicalVolume* collar1PoleBottomInnerLV     = NULL;
  G4LogicalVolume* collar1PoleTopOuterLV        = NULL;
  G4LogicalVolume* collar1PoleBottomOuterLV     = NULL;
  G4VisAttributes* collarVisAtt = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9)); // grey
  collarVisAtt->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);

  if (buildInnerCoil)
    {
      coil1Inner = new G4CutTubs(name+"_coil1_inner_solid",            // name
				 innerCoilInnerRadius,                 // inner radius
				 innerCoilOuterRadius,                 // outer radius
				 length*0.5-2*lengthSafety,            // length
				 -coilInnerFullAngle*0.5,              // start angle
				 coilInnerFullAngle,                   // sweep angle
				 inputface,                            // input face normal
				 outputface);                          // output face normal
      coil2Inner = new G4CutTubs(name+"_coil2_inner_solid",            // name
				 innerCoilInnerRadius,                 // inner radius
				 innerCoilOuterRadius,                 // outer radius
				 length*0.5-2*lengthSafety,            // length
				 CLHEP::pi-coilInnerFullAngle*0.5,     // start angle
				 coilInnerFullAngle,                   // sweep angle
				 inputface,                            // input face normal
				 outputface);                          // output face normal
      coil1InnerLV =  new G4LogicalVolume(coil1Inner,
					  nbti,
					  name+"_coil1_Inner_lv");
      coil2InnerLV =  new G4LogicalVolume(coil2Inner,
					  nbti,
					  name+"_coil2_Inner_lv");
      coil1InnerLV->SetVisAttributes(coilVisAtt);
      coil2InnerLV->SetVisAttributes(coilVisAtt);
      allLogicalVolumes.push_back(coil1InnerLV);
      allLogicalVolumes.push_back(coil2InnerLV);

      new G4PVPlacement(0,                      // rotation
			-dipolePosition,        // position
			coil1InnerLV,           // its logical volume
			name+"_coil1_inner_pv", // its name
			containerLV,            // its mother  volume
			false,                  // no boolean operation
			0, 
			BDSGlobalConstants::Instance()->GetCheckOverlaps());
      new G4PVPlacement(0,                      // rotation
			-dipolePosition,        // position
			coil2InnerLV,           // its logical volume
			name+"_coil2_inner_pv", // its name
			containerLV,            // its mother  volume
			false,                  // no boolean operation
			0, 
			BDSGlobalConstants::Instance()->GetCheckOverlaps());

      collar1PoleTopInnerSolid    = new G4CutTubs(name+"_collar1_pole_inner_top",      // name
						  innerCoilInnerRadius,                // inner radius
						  innerCoilOuterRadius,                // outer radius
						  length*0.5 - 2*lengthSafety,         // length
						  CLHEP::pi*0.5-poleInnerFullAngle*0.5,// start angle
						  poleInnerFullAngle,                  // sweep angle
						  inputface,                           // input face normal
						  outputface);                         // output face normal
      collar1PoleBottomInnerSolid = new G4CutTubs(name+"_collar1_pole_inner_bottom",   // name
						  innerCoilInnerRadius,                // inner radius
						  innerCoilOuterRadius,                // outer radius
						  length*0.5 - 2*lengthSafety,         // length
						  CLHEP::pi*1.5-poleInnerFullAngle*0.5,// start angle
						  poleInnerFullAngle,                  // sweep angle
						  inputface,                           // input face normal
						  outputface);                         // output face normal
      collar1PoleTopInnerLV    = new G4LogicalVolume(collar1PoleTopInnerSolid,
						     BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
						     name+"_collar1_pole_top_inner_lv");
      collar1PoleBottomInnerLV = new G4LogicalVolume(collar1PoleBottomInnerSolid,
						     BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
						     name+"_collar1_pole_bottom_inner_lv");
      
      collar1PoleTopInnerLV->SetVisAttributes(collarVisAtt);
      collar1PoleBottomInnerLV->SetVisAttributes(collarVisAtt);
      
      allLogicalVolumes.push_back(collar1PoleTopInnerLV); // register locally
      allLogicalVolumes.push_back(collar1PoleBottomInnerLV);

      new G4PVPlacement(0,                          // rotation
			-dipolePosition,            // position
			collar1PoleTopInnerLV,      // its logical volume
			name+"_collar1_pole_top_inner_pv",// its name
			containerLV,                // its mother  volume
			false,                      // no boolean operation
			0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
      new G4PVPlacement(0,                          // rotation
			-dipolePosition,            // position
			collar1PoleBottomInnerLV,   // its logical volume
			name+"_collar1_pole_top_inner_pv",// its name
			containerLV,                // its mother  volume
			false,                      // no boolean operation
			0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
    }
  
  if (buildOuterCoil)
    {
      coil1Outer = new G4CutTubs(name+"_coil1_outer_solid",            // name
				 outerCoilInnerRadius,                 // inner radius
				 outerCoilOuterRadius,                 // outer radius
				 length*0.5-2*lengthSafety,            // length
				 -coilOuterFullAngle*0.5,              // start angle
				 coilOuterFullAngle,                   // sweep angle
				 inputface,                            // input face normal
				 outputface);                          // output face normal
      coil2Outer = new G4CutTubs(name+"_coil2_outer_solid",            // name
				 outerCoilInnerRadius,                 // inner radius
				 outerCoilOuterRadius,                 // outer radius
				 length*0.5-2*lengthSafety,            // length
				 CLHEP::pi-coilOuterFullAngle*0.5,     // start angle
				 coilOuterFullAngle,                   // sweep angle
				 inputface,                            // input face normal
				 outputface);                          // output face normal
      coil1OuterLV =  new G4LogicalVolume(coil1Outer,
					  nbti,
					  name+"_coil1_Inner_lv");
      coil2OuterLV =  new G4LogicalVolume(coil2Outer,
					  nbti,
					  name+"_coil2_Inner_lv");
      coil1OuterLV->SetVisAttributes(coilVisAtt);
      coil2OuterLV->SetVisAttributes(coilVisAtt);
      allLogicalVolumes.push_back(coil1OuterLV);
      allLogicalVolumes.push_back(coil2OuterLV);

      new G4PVPlacement(0,                      // rotation
			-dipolePosition,        // position
			coil1OuterLV,           // its logical volume
			name+"_coil1_outer_pv", // its name
			containerLV,            // its mother  volume
			false,                  // no boolean operation
			0, 
			BDSGlobalConstants::Instance()->GetCheckOverlaps());
      new G4PVPlacement(0,                      // rotation
			-dipolePosition,        // position
			coil2OuterLV,           // its logical volume
			name+"_coil2_outer_pv", // its name
			containerLV,            // its mother  volume
			false,                  // no boolean operation
			0, 
			BDSGlobalConstants::Instance()->GetCheckOverlaps());

      collar1PoleTopOuterSolid    = new G4CutTubs(name+"_collar1_pole_outer_top",      // name
						  outerCoilInnerRadius,                // inner radius
						  outerCoilOuterRadius,                // outer radius
						  length*0.5 - 2*lengthSafety,         // length
						  CLHEP::pi*0.5-poleOuterFullAngle*0.5,// start angle
						  poleOuterFullAngle,                  // sweep angle
						  inputface,                           // input face normal
						  outputface);                         // output face normal
      collar1PoleBottomOuterSolid = new G4CutTubs(name+"_collar1_pole_outer_bottom",   // name
						  outerCoilInnerRadius,                // inner radius
						  outerCoilOuterRadius,                // outer radius
						  length*0.5 - 2*lengthSafety,         // length
						  CLHEP::pi*1.5-poleOuterFullAngle*0.5,// start angle
						  poleOuterFullAngle,                  // sweep angle
						  inputface,                           // input face normal
						  outputface);                         // output face normal

      collar1PoleTopOuterLV    = new G4LogicalVolume(collar1PoleTopOuterSolid,
						     BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
						     name+"_collar1_pole_top_outer_lv");
      collar1PoleBottomOuterLV = new G4LogicalVolume(collar1PoleBottomOuterSolid,
						     BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
						     name+"_collar1_pole_bottom_outer_lv");
      
      collar1PoleTopOuterLV->SetVisAttributes(collarVisAtt);
      collar1PoleBottomOuterLV->SetVisAttributes(collarVisAtt);
      
      allLogicalVolumes.push_back(collar1PoleTopOuterLV);
      allLogicalVolumes.push_back(collar1PoleBottomOuterLV);

      new G4PVPlacement(0,                                // rotation
			-dipolePosition,                  // position
			collar1PoleTopOuterLV,            // its logical volume
			name+"_collar1_pole_top_inner_pv",// its name
			containerLV,                      // its mother  volume
			false,                            // no boolean operation
			0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
      new G4PVPlacement(0,                                // rotation
			-dipolePosition,                  // position
			collar1PoleBottomOuterLV,         // its logical volume
			name+"_collar1_pole_top_inner_pv",// its name
			containerLV,                      // its mother  volume
			false,                            // no boolean operation
			0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
    }
  
  // coils on inactive beam pipe - always built
  G4VSolid *coil3Inner = new G4CutTubs(name+"_coil3_inner_solid",            // name
				       innerCoilInnerRadiusF,                // inner radius
				       innerCoilOuterRadius,                 // outer radius
				       secondBPHalfLength-2*lengthSafety,    // length
				       -coilInnerFullAngle*0.5,              // start angle
				       coilInnerFullAngle,                   // sweep angle
				       inputface,                            // input face normal
				       outputface);                          // output face normal
  G4VSolid *coil3Outer = new G4CutTubs(name+"_coil3_outer_solid",            // name
				       outerCoilInnerRadiusF,                // inner radius
				       outerCoilOuterRadius,                 // outer radius
				       secondBPHalfLength-2*lengthSafety,    // length
				       -coilOuterFullAngle*0.5,              // start angle
				       coilOuterFullAngle,                   // sweep angle
				       inputface,                            // input face normal
				       outputface);                          // output face normal
  G4VSolid *coil4Inner = new G4CutTubs(name+"_coil4_inner_solid",            // name
				       innerCoilInnerRadiusF,                // inner radius
				       innerCoilOuterRadius,                 // outer radius
				       secondBPHalfLength-2*lengthSafety,    // length
				       CLHEP::pi-coilInnerFullAngle*0.5,     // start angle
				       coilInnerFullAngle,                   // sweep angle
				       inputface,                            // input face normal
				       outputface);                          // output face normal
  G4VSolid *coil4Outer = new G4CutTubs(name+"_coil4_outer_solid",            // name
				       outerCoilInnerRadiusF,                // inner radius
				       outerCoilOuterRadius,                 // outer radius
				       secondBPHalfLength-2*lengthSafety,    // length
				       CLHEP::pi-coilOuterFullAngle*0.5,     // start angle
				       coilOuterFullAngle,                   // sweep angle
				       inputface,                            // input face normal
				       outputface);                          // output face normal
  
  G4LogicalVolume* coil3InnerLV =  new G4LogicalVolume(coil3Inner,
						       nbti,
						       name+"_coil3_Inner_lv");
  G4LogicalVolume* coil3OuterLV =  new G4LogicalVolume(coil3Outer,
						       nbti,
						       name+"_coil3_Inner_lv");
  G4LogicalVolume* coil4InnerLV =  new G4LogicalVolume(coil4Inner,
						       nbti,
						       name+"_coil4_Inner_lv");
  G4LogicalVolume* coil4OuterLV =  new G4LogicalVolume(coil4Outer,
						       nbti,
						       name+"_coil4_Inner_lv");
  
  coil3InnerLV->SetVisAttributes(coilVisAtt);
  coil3OuterLV->SetVisAttributes(coilVisAtt);
  coil4InnerLV->SetVisAttributes(coilVisAtt);
  coil4OuterLV->SetVisAttributes(coilVisAtt);
  
  allLogicalVolumes.push_back(coil3InnerLV);
  allLogicalVolumes.push_back(coil3OuterLV);
  allLogicalVolumes.push_back(coil4InnerLV);
  allLogicalVolumes.push_back(coil4OuterLV);

  // coil placement  
  new G4PVPlacement(0,                      // rotation
		    dipolePosition,         // position
		    coil3InnerLV,           // its logical volume
		    name+"_coil3_inner_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());

  new G4PVPlacement(0,                      // rotation
		    dipolePosition,         // position
		    coil3OuterLV,           // its logical volume
		    name+"_coil3_outer_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    dipolePosition,         // position
		    coil4InnerLV,           // its logical volume
		    name+"_coil4_inner_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    dipolePosition,         // position
		    coil4OuterLV,           // its logical volume
		    name+"_coil4_outer_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  // non-magnetic collars
  // collar pole solids  
  G4VSolid* collar2PoleTopInnerSolid    = new G4CutTubs(name+"_collar2_pole_inner_top",      // name
							innerCoilInnerRadiusF,               // inner radius
							innerCoilOuterRadius,                // outer radius
							secondBPHalfLength-2*lengthSafety,   // length
							CLHEP::pi*0.5-poleInnerFullAngle*0.5,// start angle
							poleInnerFullAngle,                  // sweep angle
							inputface,                           // input face normal
							outputface);                         // output face normal
  G4VSolid* collar2PoleTopOuterSolid    = new G4CutTubs(name+"_collar2_pole_outer_top",      // name
							outerCoilInnerRadiusF,               // inner radius
							outerCoilOuterRadius,                // outer radius
							secondBPHalfLength-2*lengthSafety,   // length
							CLHEP::pi*0.5-poleOuterFullAngle*0.5,// start angle
							poleOuterFullAngle,                  // sweep angle
							inputface,                           // input face normal
							outputface);                         // output face normal
  G4VSolid* collar2PoleBottomInnerSolid = new G4CutTubs(name+"_collar2_pole_inner_bottom",   // name
							innerCoilInnerRadiusF,               // inner radius
							innerCoilOuterRadius,                // outer radius
							secondBPHalfLength-2*lengthSafety,   // length
							CLHEP::pi*1.5-poleInnerFullAngle*0.5,// start angle
							poleInnerFullAngle,                  // sweep angle
							inputface,                           // input face normal
							outputface);                         // output face normal
  G4VSolid* collar2PoleBottomOuterSolid = new G4CutTubs(name+"_collar2_pole_outer_bottom",   // name
							outerCoilInnerRadiusF,               // inner radius
							outerCoilOuterRadius,                // outer radius
							secondBPHalfLength-2*lengthSafety,   // length
							CLHEP::pi*1.5-poleOuterFullAngle*0.5,// start angle
							poleOuterFullAngle,                  // sweep angle
							inputface,                           // input face normal
							outputface);                         // output face normal
  
  // collar pole logical volumes
  G4LogicalVolume* collar2PoleTopInnerLV    = new G4LogicalVolume(collar2PoleTopInnerSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar2_pole_top_inner_lv");
  G4LogicalVolume* collar2PoleTopOuterLV    = new G4LogicalVolume(collar2PoleTopOuterSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar2_pole_top_outer_lv");
  G4LogicalVolume* collar2PoleBottomInnerLV = new G4LogicalVolume(collar2PoleBottomInnerSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar2_pole_bottom_inner_lv");
  G4LogicalVolume* collar2PoleBottomOuterLV = new G4LogicalVolume(collar2PoleBottomOuterSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar2_pole_bottom_outer_lv");

  // collar pole visualisation
  collar2PoleTopInnerLV->SetVisAttributes(collarVisAtt);
  collar2PoleTopOuterLV->SetVisAttributes(collarVisAtt);
  collar2PoleBottomInnerLV->SetVisAttributes(collarVisAtt);
  collar2PoleBottomOuterLV->SetVisAttributes(collarVisAtt);
  
  allLogicalVolumes.push_back(collar2PoleTopInnerLV);
  allLogicalVolumes.push_back(collar2PoleTopOuterLV);
  allLogicalVolumes.push_back(collar2PoleBottomInnerLV);
  allLogicalVolumes.push_back(collar2PoleBottomOuterLV);
  
  // collar pole placement
  new G4PVPlacement(0,                                // rotation
		    dipolePosition,                   // position
		    collar2PoleTopInnerLV,            // its logical volume
		    name+"_collar2_pole_top_inner_pv",// its name
		    containerLV,                      // its mother  volume
		    false,                            // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                                // rotation
		    dipolePosition,                   // position
		    collar2PoleTopOuterLV,            // its logical volume
		    name+"_collar2_pole_top_inner_pv",// its name
		    containerLV,                      // its mother  volume
		    false,                            // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                                // rotation
		    dipolePosition,                   // position
		    collar2PoleBottomInnerLV,         // its logical volume
		    name+"_collar2_pole_top_inner_pv",// its name
		    containerLV,                      // its mother  volume
		    false,                            // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                                // rotation
		    dipolePosition,                   // position
		    collar2PoleBottomOuterLV,         // its logical volume
		    name+"_collar2_pole_top_inner_pv",// its name
		    containerLV,                      // its mother  volume
		    false,                            // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  // outer annulus of collar - two as slightly different lengths
  G4VSolid* collarAnnulus2 = new G4CutTubs(name+"_collar2_annulus_solid",    // name
					   collarInnerRadiusF,               // inner radius
					   collarOuterRadius,                // outer radius
					   secondBPHalfLength-lengthSafety,  // length
					   0,                                // starting angle
					   CLHEP::twopi,                     // angle of sweep
					   inputface,                        // input face normal
					   outputface);                      // output face normal
  
  // make final solid pointer as collar round active beam pipe optional depending on how big active beam pipe is
  G4VSolid* collars = collarAnnulus2;
  
  if (buildCollar)
    {
      // building collar around active pipe
      G4VSolid* collarAnnulus1 = new G4CutTubs(name+"_collar1_annulus_solid",      // name
					       collarInnerRadius,                  // inner radius
					       collarOuterRadius,                  // outer radius
					       length*0.5-2*lengthSafety,          // length
					       0,                                  // starting angle
					       CLHEP::twopi,                       // angle of sweep
					       inputface,                          // input face normal
					       outputface);                        // output face normal

      collars = new G4UnionSolid(name + "_collars_solid", // name
				 collarAnnulus2,          // solid1
				 collarAnnulus1,          // solid2
				 0,                       // rotation
				 -2*dipolePosition);       // translation
    }

  /*
  // This part seems to not produce any overlapping volumes and no errors but won't render
  // in anything but the raytracer.
  // ommitted for now
  
  G4VSolid* collarBox      = new G4Box(name + "_collar_box_solid",           // name
				       collarBoxHalfWidth,                   // x half width
				       collarBoxHalfHeight,                  // y half width
				       2*centralHalfLength);                 // z half length
  G4VSolid* collarBoxFaces = new G4CutTubs(name + "_collar_box_faces_solid", // name
					   0,                                // inner radius
					   50*CLHEP::cm,                     // outer radius
					   centralHalfLength - lengthSafety, // length
					   0,                                // starting angle
					   CLHEP::twopi,                     // sweep angle
					   inputface,                        // input face normal
					   outputface);                      // output face normal
  G4VSolid* collarCentralPiece = new G4IntersectionSolid(name + "_collar_central_solid", // name
							 collarBox,                      // solid 1
							 collarBoxFaces);                // solid 2   
  
  G4VSolid* collarTotal = new G4UnionSolid(name + "_collar2_plus_box_solid", // name
					   collars,                          // solid 2
					   collarCentralPiece,               // solid 1
					   0,                                // rotation
					   -dipolePosition);    // translation
  */
  G4VSolid* collarTotal = collars;
  
  G4LogicalVolume *collarsLV =  new G4LogicalVolume(collarTotal,
						    BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
						    name+"_collars_lv");

  // collar annulus visualisation attributes
  collarsLV->SetVisAttributes(collarVisAtt);
  collarsLV->SetVisAttributes(collarVisAtt);

  allLogicalVolumes.push_back(collarsLV); // register locally
  
  new G4PVPlacement(0,                  // rotation
		    G4ThreeVector(-massShift,0,0),     // position
		    collarsLV,          // its logical volume
		    name+"_collars_pv", // its name
		    containerLV,        // its mother  volume
		    false,              // no boolean operation
		    0,                  // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());  
  
  // outer iron yoke
  G4VSolid* yokeCylinder = new G4CutTubs(name+"_yoke_cylinder_solid",     // name
					 0.,                              // inner radius
					 outerDiameter*0.5 - lengthSafety,// outer radius
					 centralHalfLength-2*lengthSafety,// length
					 0,                               // starting angle
					 CLHEP::twopi,                    // sweep angle
					 inputface,                       // input face normal
					 outputface);                     // output face normal

  // need to cut hole out for everything inside - note subtraction solid has to be solid
  G4VSolid* yokeSubtractionCylinder = new G4Tubs(name + "_yoke_subtraction_cyl_solid", // name
						 0,                                    // inner radius
						 collarOuterRadius + lengthSafety,     // outer radius
						 length,                               // z half length - long for unamibiuous subtraction
						 0,                                    // start angle
						 CLHEP::twopi);                        // sweep angle

  G4VSolid* yokeSubtractionSolid = new G4UnionSolid(name + "_yoke_subtraction_solid",  // name
						    yokeSubtractionCylinder,           // solid 1
						    yokeSubtractionCylinder,           // solid 2
						    0,                                 // rotation
						    2*dipolePosition);                 // translation

  G4VSolid* yoke = new G4SubtractionSolid(name+"_yoke_solid",           // name
					  yokeCylinder,                 // from this
					  yokeSubtractionSolid,         // subtract this
					  0,                            // rotation
					  -dipolePosition);              // translation
  
  G4LogicalVolume* yokeLV = new G4LogicalVolume(yoke,
						BDSMaterials::Instance()->GetMaterial("Iron"),
						name+"_yoke_lv");

  // yoke visualisation
  G4VisAttributes* LHCblue = new G4VisAttributes(G4Colour(0.0, 0.5, 1.0));
  LHCblue->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
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
										    2.202*CLHEP::cm,   // aper1
										    1.714*CLHEP::cm,   // aper2
										    2.202*CLHEP::cm,   // aper3
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
  if (BDSExecOptions::Instance()->GetVisDebug())
    {containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());}
  else
    {containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());}

  // USER LIMITS for all components
#ifndef NOUSERLIMITS
  if (!allLogicalVolumes.empty()) {
    G4UserLimits* userLimits = new G4UserLimits("outer_cuts");
    G4double maxStepFactor = 0.5; // fraction of length for maximum step size
    userLimits->SetMaxAllowedStep( length * maxStepFactor );
    userLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
    userLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
    
    for (std::vector<G4LogicalVolume*>::iterator i = allLogicalVolumes.begin(); i != allLogicalVolumes.end(); ++i)
      {
	(*i)->SetUserLimits(userLimits);
      }
  }
#endif
  
  // record extents
  // container radius is the same for all methods as all cylindrical
  G4double containerRadius = outerDiameter;
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

  // copy sensitive volumes if they exist
  outer->RegisterSensitiveVolumes(secondBP->GetAllSensitiveVolumes());
  
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

  // test input parameters - set global options as default if not specified
  TestInputParameters(beamPipe,boxSize,outerMaterial);

  // geometrical constants
  // mass to the right or to the left
  G4ThreeVector dipolePosition;
  G4double      massShift     = 97.26*CLHEP::mm;
  G4double beamPipeAxisSeparation       = 2*massShift;
  G4double collarOuterRadius  = 101.18*CLHEP::mm;
  // these angles were calculated by visually analysing the coil layout graph
  // in lhc desigrn report Vol1, chapter 7, figure 7.1
  G4double poleInnerFullAngle = CLHEP::pi/6.; //30 degrees angle in radians
  G4double poleOuterFullAngle = CLHEP::pi/6.; //30 degrees angle in radians
  G4double coilInnerFullAngle = CLHEP::pi/2. - poleInnerFullAngle;
  G4double coilOuterFullAngle = CLHEP::pi/2. - poleOuterFullAngle;
  G4double coilFullThickness  = 118.6/2.0 - 56.0/2.0; // 41.3mm for two rows of coils, mm by default
  G4double coilOuterRadius = beamPipe->GetContainerRadius() + 2*lengthSafety + 118.6/2.0 * CLHEP::mm - 56.0/2.0*CLHEP::mm ;
  //118.6mm is the outer diameter of the coils, 56mm is the diameter of the coldbore 
  
  if (isLeftOffset)
    {
      dipolePosition = G4ThreeVector(massShift,0.,0.);
      beamPipeAxisSeparation *= -1;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "quadrupole to the left" << G4endl;
#endif
    }
  else
    {
      dipolePosition = G4ThreeVector(-massShift,0.,0.);
      massShift    *= -1;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "quadrupole to the right" << G4endl;
#endif
    }

  // we make a lot of volumes in this class - keep a record for easily attaching properties to all
  std::vector<G4LogicalVolume*> allLogicalVolumes;
  
  if (beamPipe->ContainerIsCircular())
    {
      //circular beampipe so we can simply use its radius
      //container is similar but slightly wider and hollow (to allow placement of beampipe)
      //have to do subtraction as cuttubs aperture is central and the beampipe (active one) is not here
      G4VSolid* containerSolidOuter = new G4Tubs(name + "_contiainer_solid_outer",  // name
						 0,                           // inner radius
						 boxSize*0.5,                 // outer radius
						 length*0.5,                  // half length
						 0,                           // rotation start angle
						 CLHEP::twopi);                // rotation finish angle
						    
      G4VSolid* containerSolidInner = new G4Tubs(name + "_contiainer_solid_inner",  // name
						 0,                                 // inner radius
						 beamPipe->GetContainerRadius() + lengthSafety, // outer radius
						 length,                            // full length for unambiguous subtraction
						 0,                                 // rotation start angle
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
      G4VSolid* containerSolidOuter = new G4Tubs(name + "_contiainer_solid_outer",  // name
						 0,                           // inner radius
						 boxSize*0.5,                 // outer radius
						 length*0.5,                  // half length
						 0,                           // rotation start angle
						 CLHEP::twopi);                // rotation finish angle
						 
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
  // Left and right
  G4double innerRadius = beamPipe->GetContainerRadius() + 2*lengthSafety;
  G4VSolid *coil1Inner = new G4Tubs(name+"_coil1_inner_solid",            // name
				    innerRadius,                          // inner radius
				    innerRadius + coilFullThickness*0.5,  // outer radius
				    length*0.5-2*lengthSafety,            // length
				    -coilInnerFullAngle*0.5,              // start angle
				    coilInnerFullAngle);                   // sweep angle
  G4VSolid *coil1Outer = new G4Tubs(name+"_coil1_outer_solid",            // name
				    innerRadius + coilFullThickness*0.5+lengthSafety, // inner radius
				    innerRadius + coilFullThickness,      // outer radius
				    length*0.5-2*lengthSafety,            // length
				    -coilOuterFullAngle*0.5,              // start angle
				    coilOuterFullAngle);                   // sweep angle
  G4VSolid *coil2Inner = new G4Tubs(name+"_coil2_inner_solid",            // name
				    innerRadius,                          // inner radius
				    innerRadius + coilFullThickness*0.5,  // outer radius
				    length*0.5-2*lengthSafety,            // length
				    CLHEP::pi-coilInnerFullAngle*0.5,     // start angle
				    coilInnerFullAngle);                   // sweep angle
  G4VSolid *coil2Outer = new G4Tubs(name+"_coil2_outer_solid",            // name
				    innerRadius + coilFullThickness*0.5+lengthSafety, // inner radius
				    innerRadius + coilFullThickness,      // outer radius
				    length*0.5-2*lengthSafety,            // length
				    CLHEP::pi-coilOuterFullAngle*0.5,     // start angle
				    coilOuterFullAngle);                   // sweep angle
  G4VSolid *coil3Inner = new G4Tubs(name+"_coil3_inner_solid",            // name
				    innerRadius,                          // inner radius
				    innerRadius + coilFullThickness*0.5,  // outer radius
				    length*0.5-2*lengthSafety,    // length
				    -coilInnerFullAngle*0.5,              // start angle
				    coilInnerFullAngle);                   // sweep angle
  G4VSolid *coil3Outer = new G4Tubs(name+"_coil3_outer_solid",            // name
				    innerRadius + coilFullThickness*0.5+lengthSafety, // inner radius
				    innerRadius + coilFullThickness,      // outer radius
				    length*0.5-2*lengthSafety,    // length
				    -coilOuterFullAngle*0.5,              // start angle
				    coilOuterFullAngle);                   // sweep angle
  G4VSolid *coil4Inner = new G4Tubs(name+"_coil4_inner_solid",            // name
				    innerRadius,                          // inner radius
				    innerRadius + coilFullThickness*0.5,  // outer radius
				    length*0.5-2*lengthSafety,    // length
				    CLHEP::pi-coilInnerFullAngle*0.5,     // start angle
				    coilInnerFullAngle);                   // sweep angle
  G4VSolid *coil4Outer = new G4Tubs(name+"_coil4_outer_solid",            // name
				    innerRadius + coilFullThickness*0.5+lengthSafety, // inner radius
				    innerRadius + coilFullThickness,      // outer radius
				    length*0.5-2*lengthSafety,    // length
				    CLHEP::pi-coilOuterFullAngle*0.5,     // start angle
				    coilOuterFullAngle);                   // sweep angle
  //Up and down
  G4VSolid *coil5Inner = new G4Tubs(name+"_coil5_inner_solid",            // name
				    innerRadius,                          // inner radius
				    innerRadius + coilFullThickness*0.5,  // outer radius
				    length*0.5-2*lengthSafety,            // length
				    CLHEP::pi/2.-coilInnerFullAngle*0.5,              // start angle
				    coilInnerFullAngle);                   // sweep angle
  G4VSolid *coil5Outer = new G4Tubs(name+"_coil5_outer_solid",            // name
				    innerRadius + coilFullThickness*0.5+lengthSafety, // inner radius
				    innerRadius + coilFullThickness,      // outer radius
				    length*0.5-2*lengthSafety,            // length
				    CLHEP::pi/2.-coilOuterFullAngle*0.5,              // start angle
				    coilOuterFullAngle);                   // sweep angle
  G4VSolid *coil6Inner = new G4Tubs(name+"_coil6_inner_solid",            // name
				    innerRadius,                          // inner radius
				    innerRadius + coilFullThickness*0.5,  // outer radius
				    length*0.5-2*lengthSafety,            // length
				    -CLHEP::pi/2.-coilInnerFullAngle*0.5,     // start angle
				    coilInnerFullAngle);                   // sweep angle
  G4VSolid *coil6Outer = new G4Tubs(name+"_coil6_outer_solid",            // name
				    innerRadius + coilFullThickness*0.5+lengthSafety, // inner radius
				    innerRadius + coilFullThickness,      // outer radius
				    length*0.5-2*lengthSafety,            // length
				    -CLHEP::pi/2.-coilOuterFullAngle*0.5,     // start angle
				    coilOuterFullAngle);                   // sweep angle
  G4VSolid *coil7Inner = new G4Tubs(name+"_coil7_inner_solid",            // name
				    innerRadius,                          // inner radius
				    innerRadius + coilFullThickness*0.5,  // outer radius
				    length*0.5-2*lengthSafety,    // length
				    CLHEP::pi/2.-coilInnerFullAngle*0.5,              // start angle
				    coilInnerFullAngle);                   // sweep angle
  G4VSolid *coil7Outer = new G4Tubs(name+"_coil7_outer_solid",            // name
				    innerRadius + coilFullThickness*0.5+lengthSafety, // inner radius
				    innerRadius + coilFullThickness,      // outer radius
				    length*0.5-2*lengthSafety,    // length
				    CLHEP::pi/2.-coilOuterFullAngle*0.5,              // start angle
				    coilOuterFullAngle);                   // sweep angle
  G4VSolid *coil8Inner = new G4Tubs(name+"_coil8_inner_solid",            // name
				    innerRadius,                          // inner radius
				    innerRadius + coilFullThickness*0.5,  // outer radius
				    length*0.5-2*lengthSafety,    // length
				    -CLHEP::pi/2.-coilInnerFullAngle*0.5,     // start angle
				    coilInnerFullAngle);                   // sweep angle
  G4VSolid *coil8Outer = new G4Tubs(name+"_coil8_outer_solid",            // name
				    innerRadius + coilFullThickness*0.5+lengthSafety, // inner radius
				    innerRadius + coilFullThickness,      // outer radius
				    length*0.5-2*lengthSafety,    // length
				    -CLHEP::pi/2.-coilOuterFullAngle*0.5,     // start angle
				    coilOuterFullAngle);                   // sweep angle




  // coil logical volumes
  G4Material* nbti = BDSMaterials::Instance()->GetMaterial("NbTi.1");
  G4LogicalVolume* coil1InnerLV =  new G4LogicalVolume(coil1Inner,
						       nbti,
						       name+"_coil1_Inner_lv");
  G4LogicalVolume* coil1OuterLV =  new G4LogicalVolume(coil1Outer,
						       nbti,
						       name+"_coil1_Inner_lv");
  G4LogicalVolume* coil2InnerLV =  new G4LogicalVolume(coil2Inner,
						       nbti,
						       name+"_coil2_Inner_lv");
  G4LogicalVolume* coil2OuterLV =  new G4LogicalVolume(coil2Outer,
						       nbti,
						       name+"_coil2_Inner_lv");
  G4LogicalVolume* coil3InnerLV =  new G4LogicalVolume(coil3Inner,
						       nbti,
						       name+"_coil3_Inner_lv");
  G4LogicalVolume* coil3OuterLV =  new G4LogicalVolume(coil3Outer,
						       nbti,
						       name+"_coil3_Inner_lv");
  G4LogicalVolume* coil4InnerLV =  new G4LogicalVolume(coil4Inner,
						       nbti,
						       name+"_coil4_Inner_lv");
  G4LogicalVolume* coil4OuterLV =  new G4LogicalVolume(coil4Outer,
						       nbti,
						       name+"_coil4_Inner_lv");
  G4LogicalVolume* coil5InnerLV =  new G4LogicalVolume(coil5Inner,
						       nbti,
						       name+"_coil5_Inner_lv");
  G4LogicalVolume* coil5OuterLV =  new G4LogicalVolume(coil5Outer,
						       nbti,
						       name+"_coil5_Inner_lv");
  G4LogicalVolume* coil6InnerLV =  new G4LogicalVolume(coil6Inner,
						       nbti,
						       name+"_coil6_Inner_lv");
  G4LogicalVolume* coil6OuterLV =  new G4LogicalVolume(coil6Outer,
						       nbti,
						       name+"_coil6_Inner_lv");
  G4LogicalVolume* coil7InnerLV =  new G4LogicalVolume(coil7Inner,
						       nbti,
						       name+"_coil7_Inner_lv");
  G4LogicalVolume* coil7OuterLV =  new G4LogicalVolume(coil7Outer,
						       nbti,
						       name+"_coil7_Inner_lv");
  G4LogicalVolume* coil8InnerLV =  new G4LogicalVolume(coil8Inner,
						       nbti,
						       name+"_coil8_Inner_lv");
  G4LogicalVolume* coil8OuterLV =  new G4LogicalVolume(coil8Outer,
						       nbti,
						       name+"_coil8_Inner_lv");

  // coil visualisation
  G4VisAttributes* coilVisAtt = new G4VisAttributes(G4Colour(0.9, 0.75, 0.)); //gold-ish colour
  coilVisAtt->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  coil1InnerLV->SetVisAttributes(coilVisAtt);
  coil1OuterLV->SetVisAttributes(coilVisAtt);
  coil2InnerLV->SetVisAttributes(coilVisAtt);
  coil2OuterLV->SetVisAttributes(coilVisAtt);
  coil3InnerLV->SetVisAttributes(coilVisAtt);
  coil3OuterLV->SetVisAttributes(coilVisAtt);
  coil4InnerLV->SetVisAttributes(coilVisAtt);
  coil4OuterLV->SetVisAttributes(coilVisAtt);
  coil5InnerLV->SetVisAttributes(coilVisAtt);
  coil5OuterLV->SetVisAttributes(coilVisAtt);
  coil6InnerLV->SetVisAttributes(coilVisAtt);
  coil6OuterLV->SetVisAttributes(coilVisAtt);
  coil7InnerLV->SetVisAttributes(coilVisAtt);
  coil7OuterLV->SetVisAttributes(coilVisAtt);
  coil8InnerLV->SetVisAttributes(coilVisAtt);
  coil8OuterLV->SetVisAttributes(coilVisAtt);

  allLogicalVolumes.push_back(coil1InnerLV); // register locally
  allLogicalVolumes.push_back(coil1OuterLV);
  allLogicalVolumes.push_back(coil2InnerLV);
  allLogicalVolumes.push_back(coil2OuterLV);
  allLogicalVolumes.push_back(coil3InnerLV);
  allLogicalVolumes.push_back(coil3OuterLV);
  allLogicalVolumes.push_back(coil4InnerLV);
  allLogicalVolumes.push_back(coil4OuterLV);
  allLogicalVolumes.push_back(coil5InnerLV);
  allLogicalVolumes.push_back(coil5OuterLV);
  allLogicalVolumes.push_back(coil6InnerLV);
  allLogicalVolumes.push_back(coil6OuterLV);
  allLogicalVolumes.push_back(coil7InnerLV);
  allLogicalVolumes.push_back(coil7OuterLV);
  allLogicalVolumes.push_back(coil8InnerLV);
  allLogicalVolumes.push_back(coil8OuterLV);

  // coil placement
  new G4PVPlacement(0,                      // rotation
		    -dipolePosition,        // position
		    coil1InnerLV,           // its logical volume
		    name+"_coil1_inner_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    -dipolePosition,        // position
		    coil1OuterLV,           // its logical volume
		    name+"_coil1_outer_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    -dipolePosition,        // position
		    coil2InnerLV,           // its logical volume
		    name+"_coil2_inner_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    -dipolePosition,        // position
		    coil2OuterLV,           // its logical volume
		    name+"_coil2_outer_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    dipolePosition,         // position
		    coil3InnerLV,           // its logical volume
		    name+"_coil3_inner_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    dipolePosition,         // position
		    coil3OuterLV,           // its logical volume
		    name+"_coil3_outer_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    dipolePosition,         // position
		    coil4InnerLV,           // its logical volume
		    name+"_coil4_inner_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    dipolePosition,         // position
		    coil4OuterLV,           // its logical volume
		    name+"_coil4_outer_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()); 
  new G4PVPlacement(0,                      // rotation
		    -dipolePosition,        // position
		    coil5InnerLV,           // its logical volume
		    name+"_coil5_inner_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    -dipolePosition,        // position
		    coil5OuterLV,           // its logical volume
		    name+"_coil5_outer_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    -dipolePosition,        // position
		    coil6InnerLV,           // its logical volume
		    name+"_coil6_inner_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    -dipolePosition,        // position
		    coil6OuterLV,           // its logical volume
		    name+"_coil6_outer_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    dipolePosition,         // position
		    coil7InnerLV,           // its logical volume
		    name+"_coil7_inner_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    dipolePosition,         // position
		    coil7OuterLV,           // its logical volume
		    name+"_coil7_outer_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    dipolePosition,         // position
		    coil8InnerLV,           // its logical volume
		    name+"_coil8_inner_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                      // rotation
		    dipolePosition,         // position
		    coil8OuterLV,           // its logical volume
		    name+"_coil8_outer_pv", // its name
		    containerLV,            // its mother  volume
		    false,                  // no boolean operation
		    0, 
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()); 
  
  // non-magnetic collars
  // collar pole solids
  G4VSolid* collar1PoleTopInnerSolid    = new G4Tubs(name+"_collar1_pole_inner_top",      // name
						     innerRadius,                         // inner radius
						     innerRadius + coilFullThickness*0.5, // outer radius
						     length*0.5 - 2*lengthSafety,         // length
						     CLHEP::pi/4.-poleInnerFullAngle*0.5,// start angle
						     poleInnerFullAngle);                  // sweep angle
  G4VSolid* collar1PoleTopOuterSolid    = new G4Tubs(name+"_collar1_pole_outer_top",      // name
						     innerRadius + coilFullThickness*0.5 + lengthSafety, // inner radius
						     innerRadius + coilFullThickness,     // outer radius
						     length*0.5 - 2*lengthSafety,         // length
						     CLHEP::pi/4.-poleOuterFullAngle*0.5,// start angle
						     poleOuterFullAngle);                  // sweep angle
  G4VSolid* collar1PoleBottomInnerSolid = new G4Tubs(name+"_collar1_pole_inner_bottom",   // name
						     innerRadius,                         // inner radius
						     innerRadius + coilFullThickness*0.5, // outer radius
						     length*0.5 - 2*lengthSafety,         // length
						     CLHEP::pi*5./4.-poleInnerFullAngle*0.5,// start angle
						     poleInnerFullAngle);                  // sweep angle						  
  G4VSolid* collar1PoleBottomOuterSolid = new G4Tubs(name+"_collar1_pole_outer_bottom",   // name
						     innerRadius + coilFullThickness*0.5 + lengthSafety, // inner radius
						     innerRadius + coilFullThickness,     // outer radius
						     length*0.5 - 2*lengthSafety,         // length
						     CLHEP::pi*5./4.-poleOuterFullAngle*0.5,// start angle
						     poleOuterFullAngle);                  // sweep angle  

  G4VSolid* collar2PoleTopInnerSolid    = new G4Tubs(name+"_collar2_pole_inner_top",      // name
						     innerRadius,                         // inner radius
						     innerRadius + coilFullThickness*0.5, // outer radius
						     0.5*length-2*lengthSafety,   // length
						     CLHEP::pi*1./4.-poleInnerFullAngle*0.5,// start angle
						     poleInnerFullAngle);                  // sweep angle
  G4VSolid* collar2PoleTopOuterSolid    = new G4Tubs(name+"_collar2_pole_outer_top",      // name
						     innerRadius + coilFullThickness*0.5 + lengthSafety, // inner radius
						     innerRadius + coilFullThickness,     // outer radius
						     0.5*length-2*lengthSafety,   // length
						     CLHEP::pi*1./4.-poleOuterFullAngle*0.5,// start angle
						     poleOuterFullAngle);                  // sweep anglel
  G4VSolid* collar2PoleBottomInnerSolid = new G4Tubs(name+"_collar2_pole_inner_bottom",   // name
						     innerRadius,                         // inner radius
						     innerRadius + coilFullThickness*0.5, // outer radius
						     0.5*length-2*lengthSafety,   // length
						     CLHEP::pi*5./4.-poleInnerFullAngle*0.5,// start angle
						     poleInnerFullAngle);                  // sweep angle
  G4VSolid* collar2PoleBottomOuterSolid = new G4Tubs(name+"_collar2_pole_outer_bottom",   // name
						     innerRadius + coilFullThickness*0.5 + lengthSafety, // inner radius
						     innerRadius + coilFullThickness,     // outer radius
						     0.5*length-2*lengthSafety,   // length
						     CLHEP::pi*5./4.-poleOuterFullAngle*0.5,// start angle
						     poleOuterFullAngle);                  // sweep angle


  G4VSolid* collar3PoleTopInnerSolid    = new G4Tubs(name+"_collar3_pole_inner_top",      // name
						     innerRadius,                         // inner radius
						     innerRadius + coilFullThickness*0.5, // outer radius
						     length*0.5 - 2*lengthSafety,         // length
						     CLHEP::pi*3./4.-poleInnerFullAngle*0.5,// start angle
						     poleInnerFullAngle);                  // sweep angle
  G4VSolid* collar3PoleTopOuterSolid    = new G4Tubs(name+"_collar3_pole_outer_top",      // name
						     innerRadius + coilFullThickness*0.5 + lengthSafety, // inner radius
						     innerRadius + coilFullThickness,     // outer radius
						     length*0.5 - 2*lengthSafety,         // length
						     CLHEP::pi*3./4.-poleOuterFullAngle*0.5,// start angle
						     poleOuterFullAngle);                  // sweep angle
  G4VSolid* collar3PoleBottomInnerSolid = new G4Tubs(name+"_collar3_pole_inner_bottom",   // name
						     innerRadius,                         // inner radius
						     innerRadius + coilFullThickness*0.5, // outer radius
						     length*0.5 - 2*lengthSafety,         // length
						     CLHEP::pi*7./4.-poleInnerFullAngle*0.5,// start angle
						     poleInnerFullAngle);                  // sweep angle						  
  G4VSolid* collar3PoleBottomOuterSolid = new G4Tubs(name+"_collar3_pole_outer_bottom",   // name
						     innerRadius + coilFullThickness*0.5 + lengthSafety, // inner radius
						     innerRadius + coilFullThickness,     // outer radius
						     length*0.5 - 2*lengthSafety,         // length
						     CLHEP::pi*7./4.-poleOuterFullAngle*0.5,// start angle
						     poleOuterFullAngle);                  // sweep angle
  
  G4VSolid* collar4PoleTopInnerSolid    = new G4Tubs(name+"_collar4_pole_inner_top",      // name
						     innerRadius,                         // inner radius
						     innerRadius + coilFullThickness*0.5, // outer radius
						     0.5*length-2*lengthSafety,   // length
						     CLHEP::pi*3./4.-poleInnerFullAngle*0.5,// start angle
						     poleInnerFullAngle);                  // sweep angle
  G4VSolid* collar4PoleTopOuterSolid    = new G4Tubs(name+"_collar4_pole_outer_top",      // name
						     innerRadius + coilFullThickness*0.5 + lengthSafety, // inner radius
						     innerRadius + coilFullThickness,     // outer radius
						     0.5*length-2*lengthSafety,   // length
						     CLHEP::pi*3./4.-poleOuterFullAngle*0.5,// start angle
						     poleOuterFullAngle);                  // sweep anglel
  G4VSolid* collar4PoleBottomInnerSolid = new G4Tubs(name+"_collar4_pole_inner_bottom",   // name
						     innerRadius,                         // inner radius
						     innerRadius + coilFullThickness*0.5, // outer radius
						     0.5*length-2*lengthSafety,   // length
						     CLHEP::pi*7./4.-poleInnerFullAngle*0.5,// start angle
						     poleInnerFullAngle);                  // sweep angle
  G4VSolid* collar4PoleBottomOuterSolid = new G4Tubs(name+"_collar4_pole_outer_bottom",   // name
						     innerRadius + coilFullThickness*0.5 + lengthSafety, // inner radius
						     innerRadius + coilFullThickness,     // outer radius
						     0.5*length-2*lengthSafety,   // length
						     CLHEP::pi*7./4.-poleOuterFullAngle*0.5,// start angle
						     poleOuterFullAngle);                  // sweep angle
  
  // collar pole logical volumes
  G4LogicalVolume* collar1PoleTopInnerLV    = new G4LogicalVolume(collar1PoleTopInnerSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar1_pole_top_inner_lv");
  G4LogicalVolume* collar1PoleTopOuterLV    = new G4LogicalVolume(collar1PoleTopOuterSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar1_pole_top_outer_lv");
  G4LogicalVolume* collar1PoleBottomInnerLV = new G4LogicalVolume(collar1PoleBottomInnerSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar1_pole_bottom_inner_lv");
  G4LogicalVolume* collar1PoleBottomOuterLV = new G4LogicalVolume(collar1PoleBottomOuterSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar1_pole_bottom_outer_lv");
  G4LogicalVolume* collar2PoleTopInnerLV    = new G4LogicalVolume(collar2PoleTopInnerSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar2_pole_top_inner_lv");
  G4LogicalVolume* collar2PoleTopOuterLV    = new G4LogicalVolume(collar2PoleTopOuterSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar2_pole_top_outer_lv");
  G4LogicalVolume* collar2PoleBottomInnerLV = new G4LogicalVolume(collar2PoleBottomInnerSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar2_pole_bottom_inner_lv");
  G4LogicalVolume* collar2PoleBottomOuterLV = new G4LogicalVolume(collar2PoleBottomOuterSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar2_pole_bottom_outer_lv");


  G4LogicalVolume* collar3PoleTopInnerLV    = new G4LogicalVolume(collar3PoleTopInnerSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar3_pole_top_inner_lv");
  G4LogicalVolume* collar3PoleTopOuterLV    = new G4LogicalVolume(collar3PoleTopOuterSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar3_pole_top_outer_lv");
  G4LogicalVolume* collar3PoleBottomInnerLV = new G4LogicalVolume(collar3PoleBottomInnerSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar3_pole_bottom_inner_lv");
  G4LogicalVolume* collar3PoleBottomOuterLV = new G4LogicalVolume(collar3PoleBottomOuterSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar3_pole_bottom_outer_lv");
  G4LogicalVolume* collar4PoleTopInnerLV    = new G4LogicalVolume(collar4PoleTopInnerSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar4_pole_top_inner_lv");
  G4LogicalVolume* collar4PoleTopOuterLV    = new G4LogicalVolume(collar4PoleTopOuterSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar4_pole_top_outer_lv");
  G4LogicalVolume* collar4PoleBottomInnerLV = new G4LogicalVolume(collar4PoleBottomInnerSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar4_pole_bottom_inner_lv");
  G4LogicalVolume* collar4PoleBottomOuterLV = new G4LogicalVolume(collar4PoleBottomOuterSolid,
								  BDSMaterials::Instance()->GetMaterial("stainlesssteel"),
								  name+"_collar4_pole_bottom_outer_lv");

  // collar pole visualisation
  G4VisAttributes* collarVisAtt = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9));
  collarVisAtt->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  collar1PoleTopInnerLV->SetVisAttributes(collarVisAtt);
  collar1PoleTopOuterLV->SetVisAttributes(collarVisAtt);
  collar1PoleBottomInnerLV->SetVisAttributes(collarVisAtt);
  collar1PoleBottomOuterLV->SetVisAttributes(collarVisAtt);
  collar2PoleTopInnerLV->SetVisAttributes(collarVisAtt);
  collar2PoleTopOuterLV->SetVisAttributes(collarVisAtt);
  collar2PoleBottomInnerLV->SetVisAttributes(collarVisAtt);
  collar2PoleBottomOuterLV->SetVisAttributes(collarVisAtt);
  collar3PoleTopInnerLV->SetVisAttributes(collarVisAtt);
  collar3PoleTopOuterLV->SetVisAttributes(collarVisAtt);
  collar3PoleBottomInnerLV->SetVisAttributes(collarVisAtt);
  collar3PoleBottomOuterLV->SetVisAttributes(collarVisAtt);
  collar4PoleTopInnerLV->SetVisAttributes(collarVisAtt);
  collar4PoleTopOuterLV->SetVisAttributes(collarVisAtt);
  collar4PoleBottomInnerLV->SetVisAttributes(collarVisAtt);
  collar4PoleBottomOuterLV->SetVisAttributes(collarVisAtt);
  
  allLogicalVolumes.push_back(collar1PoleTopInnerLV); // register locally
  allLogicalVolumes.push_back(collar1PoleTopOuterLV);
  allLogicalVolumes.push_back(collar1PoleBottomInnerLV);
  allLogicalVolumes.push_back(collar1PoleBottomOuterLV);
  allLogicalVolumes.push_back(collar2PoleTopInnerLV);
  allLogicalVolumes.push_back(collar2PoleTopOuterLV);
  allLogicalVolumes.push_back(collar2PoleBottomInnerLV);
  allLogicalVolumes.push_back(collar2PoleBottomOuterLV);
  
  // collar pole placement
  new G4PVPlacement(0,                          // rotation
		    -dipolePosition,            // position
		    collar1PoleTopInnerLV,      // its logical volume
		    name+"_collar1_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                          // rotation
		    -dipolePosition,            // position
		    collar1PoleTopOuterLV,      // its logical volume
		    name+"_collar1_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                          // rotation
		    -dipolePosition,            // position
		    collar1PoleBottomInnerLV,   // its logical volume
		    name+"_collar1_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                          // rotation
		    -dipolePosition,            // position
		    collar1PoleBottomOuterLV,   // its logical volume
		    name+"_collar1_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                          // rotation
		    dipolePosition,             // position
		    collar2PoleTopInnerLV,      // its logical volume
		    name+"_collar2_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                          // rotation
		    dipolePosition,             // position
		    collar2PoleTopOuterLV,      // its logical volume
		    name+"_collar2_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                          // rotation
		    dipolePosition,             // position
		    collar2PoleBottomInnerLV,   // its logical volume
		    name+"_collar2_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                          // rotation
		    dipolePosition,             // position
		    collar2PoleBottomOuterLV,   // its logical volume
		    name+"_collar2_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());

  new G4PVPlacement(0,                          // rotation
		    -dipolePosition,            // position
		    collar3PoleTopInnerLV,      // its logical volume
		    name+"_collar3_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                          // rotation
		    -dipolePosition,            // position
		    collar3PoleTopOuterLV,      // its logical volume
		    name+"_collar3_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                          // rotation
		    -dipolePosition,            // position
		    collar3PoleBottomInnerLV,   // its logical volume
		    name+"_collar3_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                          // rotation
		    -dipolePosition,            // position
		    collar3PoleBottomOuterLV,   // its logical volume
		    name+"_collar3_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                          // rotation
		    dipolePosition,             // position
		    collar4PoleTopInnerLV,      // its logical volume
		    name+"_collar4_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                          // rotation
		    dipolePosition,             // position
		    collar4PoleTopOuterLV,      // its logical volume
		    name+"_collar4_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                          // rotation
		    dipolePosition,             // position
		    collar4PoleBottomInnerLV,   // its logical volume
		    name+"_collar4_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  new G4PVPlacement(0,                          // rotation
		    dipolePosition,             // position
		    collar4PoleBottomOuterLV,   // its logical volume
		    name+"_collar4_pole_top_inner_pv",// its name
		    containerLV,                // its mother  volume
		    false,                      // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  // outer annulus of collar - two as slightly different lengths
  G4VSolid* collarAnnulus1 = new G4Tubs(name+"_collar1_annulus_solid",      // name
					coilOuterRadius + lengthSafety,     // inner radius
					collarOuterRadius - lengthSafety,   // outer radius
					length*0.5-2*lengthSafety,          // length
					0,                                  // starting angle
					CLHEP::twopi);                       // angle of sweep
  
  G4VSolid* collarAnnulus2 = new G4Tubs(name+"_collar_annulus_solid",       // name
					coilOuterRadius + lengthSafety,     // inner radius
					collarOuterRadius - lengthSafety,   // outer radius
					0.5*length-2*lengthSafety,  // length
					0,                                  // starting angle
					CLHEP::twopi);                       // angle of sweep

  // prepare a solid to cut a hole in the outer yoke volume (can just use one twice)
  // can't use the collarAnnuli as they're not solid - need them to be solid
  G4VSolid* collarSubtractionCylinder = new G4Tubs(name+"_collar_subtraction_solid",    // name
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
  G4VSolid* yokeCylinder = new G4Tubs(name+"_yoke_cylinder_solid",     // name
				      0.,                              // inner radius
				      boxSize*0.5,                     // outer radius
				      0.5*length-2*lengthSafety,// length
				      0,                               // starting angle
				      CLHEP::twopi * CLHEP::rad);       // sweep angle

  G4VSolid* yoke = new G4SubtractionSolid(name+"_yoke_solid",             // name
					  yokeCylinder,                   // from this
					  collarSubtractionCylinders,     // subtract this
					  0,
					  -dipolePosition);               

  
  G4LogicalVolume* yokeLV = new G4LogicalVolume(yoke,
						BDSMaterials::Instance()->GetMaterial("Iron"),
						name+"_yoke_lv");

  // yoke visualisation
  G4VisAttributes* LHCred = new G4VisAttributes(G4Colour(1.0, 0., 0.));
  LHCred->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  yokeLV->SetVisAttributes(LHCred);
  
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
  BDSBeamPipe* secondBP = BDSBeamPipeFactory::Instance()->CreateBeamPipe(BDSBeamPipeType::lhcdetailed,
									 name,
									 length-2*lengthSafety,
									 2.402*CLHEP::cm,   // aper1
									 1.828*CLHEP::cm,   // aper2
									 2.402*CLHEP::cm,   // aper3
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
  if (BDSExecOptions::Instance()->GetVisDebug())
    {containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());}
  else
    {containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());}

  // USER LIMITS and SENSITIVITY for all components
#ifndef NOUSERLIMITS
  if (!allLogicalVolumes.empty()) {
    G4UserLimits* userLimits = new G4UserLimits("outer_cuts");
    G4double maxStepFactor = 0.5; // fraction of length for maximum step size
    userLimits->SetMaxAllowedStep( length * maxStepFactor );
    userLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
    userLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
    
    for (std::vector<G4LogicalVolume*>::iterator i = allLogicalVolumes.begin(); i != allLogicalVolumes.end(); ++i) {
      (*i)->SetSensitiveDetector(BDSSDManager::Instance()->GetEnergyCounterOnAxisSD());
      (*i)->SetUserLimits(userLimits);
    }
  }
#endif
    
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
  outerLV->SetVisAttributes(outerVisAttr);
  // container
#ifdef BDSDEBUG
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());
#else
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
#endif

  // USER LIMITS - set user limits based on bdsim user specified parameters
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
