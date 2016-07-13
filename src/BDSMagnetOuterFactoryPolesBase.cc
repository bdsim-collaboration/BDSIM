#include "BDSMagnetOuterFactoryPolesBase.hh"

#include "BDSBeamPipe.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSSimpleComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterFactoryCylindrical.hh" // for default geometry
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"

#include "globals.hh"                      // geant4 globals / types
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4CutTubs.hh"
#include "G4EllipticalTube.hh"
#include "G4ExtrudedSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4IntersectionSolid.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4TwoVector.hh"
#include "G4UnionSolid.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include <algorithm>                       // for std::max
#include <cmath>
#include <string>                          // for std::to_string
#include <utility>                         // for std::pair
#include <vector>

BDSMagnetOuterFactoryPolesBase::BDSMagnetOuterFactoryPolesBase():
  BDSMagnetOuterFactoryPolesBase(1.1)
{;}

BDSMagnetOuterFactoryPolesBase::BDSMagnetOuterFactoryPolesBase(G4double poleStopFactorIn):
  poleFraction(0.7),
  poleAngularFraction(0.7),
  poleTipFraction(0.2),
  poleAnnulusFraction(0.1),
  bendHeightFraction(0.7),
  poleStopFactor(poleStopFactorIn)
{
  // now the base class constructor should be called first which
  // should call clean up (in the derived class) which should initialise
  // the variables I think, but doing here just to be sure.
  CleanUp();
}

void BDSMagnetOuterFactoryPolesBase::CleanUp()
{
  BDSMagnetOuterFactoryBase::CleanUp();

  yokeStartRadius       = 0;
  yokeFinishRadius      = 0;
  magnetContainerRadius = 0;
  buildPole             = true;
  poleStartRadius       = 0;
  poleFinishRadius      = 0;
  poleSquareWidth       = 0;
  poleSquareStartRadius = 0;
  segmentAngle          = 0;
  poleAngle             = 0;
  poleTranslation       = G4ThreeVector(0,0,0);
  coilHeight            = 0;
  coilCentreRadius      = 0;
  endPieceLength        = 0;
  endPieceInnerR        = 0;
  endPieceOuterR        = 0;
  poleIntersectionSolid = nullptr;
  coilLeftSolid         = nullptr;
  coilRightSolid        = nullptr;
  coilLeftLV            = nullptr;
  coilRightLV           = nullptr;
  endPieceCoilLV        = nullptr;
  endPiece              = nullptr;
  leftPoints.clear();
  rightPoints.clear();
  endPiecePoints.clear();
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::CreateSectorBend(G4String      name,
								 G4double      length,
								 BDSBeamPipe*  beamPipe,
								 G4double      outerDiameter,
								 G4double      containerLength,
								 G4double      angleIn,
								 G4double      angleOut,
								 G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  auto colour = BDSColours::Instance()->GetColour("sectorbend");
  return CreateDipole(name, length, beamPipe, outerDiameter, containerLength, angleIn,
		      angleOut, outerMaterial, true, colour);
  
  CleanUp(); // doesn't use CommonConstructor so must do this manually

  // test input parameters - set global options as default if not specified
  TestInputParameters(beamPipe,outerDiameter,outerMaterial);

  std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(angleIn,angleOut);
  G4ThreeVector inputface = faces.first;
  G4ThreeVector outputface = faces.second;

  G4double beampipeRadiusX = std::max(beamPipe->GetExtentX().first, beamPipe->GetExtentX().second);
  G4double beampipeRadiusY = std::max(beamPipe->GetExtentY().first, beamPipe->GetExtentY().second);

  yokeFinishRadius = 0.5 * outerDiameter;
  yokeStartRadius  = (poleFraction * yokeFinishRadius) + lengthSafetyLarge;
  G4double yokeFinishRadiusY = yokeFinishRadius * bendHeightFraction;
  G4double yokeThickness     = yokeFinishRadius - yokeStartRadius;
  G4double yokeStartRadiusY  = yokeFinishRadiusY - yokeThickness;
  G4double outerLength       = (0.5 * length) - lengthSafety;
  // make angled face cylinder big enough to encompass it all
  G4double angledFaceRadius  = sqrt(pow(yokeFinishRadius, 2) + pow(yokeFinishRadiusY, 2)) + lengthSafetyLarge;

  // protection
  G4bool buildPoles = true;
  if (yokeStartRadius < beampipeRadiusX)
    {yokeStartRadius = beampipeRadiusX + lengthSafetyLarge;}
  if (yokeFinishRadius < yokeStartRadius)
    {yokeFinishRadius = yokeStartRadius + 1*CLHEP::mm;}
  if (yokeStartRadiusY < beampipeRadiusY)
    {
      yokeStartRadiusY = beampipeRadiusY + lengthSafetyLarge;
      buildPoles = false;
    }
  if (yokeFinishRadiusY < yokeStartRadiusY)
    {
      yokeFinishRadiusY = yokeStartRadiusY + 1*CLHEP::mm;
      buildPoles = false;
    }

  if (yokeStartRadiusY - beampipeRadiusY < 1*CLHEP::mm)
    {buildPoles = false;}
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "beam pipe x max radius: " << beampipeRadiusX   << G4endl;
  G4cout << __METHOD_NAME__ << "beam pipe y max radius: " << beampipeRadiusY   << G4endl;
  G4cout << __METHOD_NAME__ << "outer diameter:         " << outerDiameter     << G4endl;
  G4cout << __METHOD_NAME__ << "yoke start radius x:    " << yokeStartRadius   << G4endl;
  G4cout << __METHOD_NAME__ << "yoke finish radius x:   " << yokeFinishRadius  << G4endl;
  G4cout << __METHOD_NAME__ << "yoke thickness:         " << yokeThickness     << G4endl;
  G4cout << __METHOD_NAME__ << "yoke start radius y:    " << yokeStartRadiusY  << G4endl;
  G4cout << __METHOD_NAME__ << "yoke finish radius y:   " << yokeFinishRadiusY << G4endl;
  G4cout << __METHOD_NAME__ << "outer z half length:    " << outerLength       << G4endl;
  G4cout << __METHOD_NAME__ << "angled face radius:     " << angledFaceRadius  << G4endl;
  G4cout << __METHOD_NAME__ << "build poles:            " << buildPoles        << G4endl;
#endif

  G4Box* containerSquareSolid = new G4Box(name + "_container_square_solid", // name
					  yokeFinishRadius,                 // x half width
					  yokeFinishRadiusY,                // y half width
					  15.0*outerLength - lengthSafety);  // z half width - long for unambiguous intersection
  
  G4VSolid* angledFaceSolid = nullptr;
  if (BDS::IsFinite(angleIn) || BDS::IsFinite(angleOut))
    {
      angledFaceSolid = new G4CutTubs(name + "_angled_face_solid", // name
				      0,                           // inner radius
				      angledFaceRadius,            // outer radius
				      outerLength,                 // half length - must fit within container
				      0,                           // rotation start angle
				      CLHEP::twopi,                // rotation sweep angle
				      inputface,                   // input face normal
				      outputface);                 // output face normal
    }
  else
    {
      angledFaceSolid = new G4Tubs(name + "_angled_face_solid", // name
				   0,                           // inner radius
				   angledFaceRadius,            // outer radius
				   outerLength,                 // half length - must fit within container
				   0,                           // rotation start angle
				   CLHEP::twopi);               // rotation sweep angle
    }
  allSolids.push_back(containerSquareSolid);
  allSolids.push_back(angledFaceSolid);

  G4VSolid* containerSolidSolid = new G4IntersectionSolid(name + "_container_solid_solid", // name
							  containerSquareSolid,
							  angledFaceSolid);

  // always just cut a box out with the bounds of the beam pipe
  G4VSolid* containerInner = new G4Box(name + "_container_inner",      // name
				       beampipeRadiusX + lengthSafety, // x half width
				       beampipeRadiusY + lengthSafety, // y half width
				       length*3); // z long for unambiguous subtraction

  allSolids.push_back(containerSolidSolid);
  allSolids.push_back(containerInner);

  // always need to do a subtraction so save memory by not creating another subtraciton solid
  containerSolid = new G4SubtractionSolid(name + "_container_solid",      // name
					  containerSolidSolid,            // this
					  containerInner); // minus this

  G4double contRX = yokeFinishRadius  + lengthSafetyLarge;
  G4double contRY = yokeFinishRadiusY + lengthSafetyLarge;
  G4Box* magnetContSqSolid = new G4Box(name + "_mag_cont_square_solid", // name
				       contRX,                          // x half width
				       contRY,                          // y half width
				       15.0*outerLength - 2.0*lengthSafety); // z half width - long for unambiguous intersection
  G4VSolid* magnetAngledFaceSolid = nullptr;
  if (BDS::IsFinite(angleIn) || BDS::IsFinite(angleOut))
    {
      magnetAngledFaceSolid = new G4CutTubs(name + "_angled_face_solid", // name
					    0,                           // inner radius
					    angledFaceRadius,            // outer radius
					    containerLength*0.5,         // half length - must fit within container
					    0,                           // rotation start angle
					    CLHEP::twopi,                // rotation sweep angle
					    inputface,                   // input face normal
					    outputface);                 // output face normal
    }
  else
    {
      magnetAngledFaceSolid = new G4Tubs(name + "_angled_face_solid", // name
					 0,                           // inner radius
					 angledFaceRadius,            // outer radius
					 containerLength*0.5,         // half length - must fit within container
					 0,                           // rotation start angle
					 CLHEP::twopi);               // rotation sweep angle
    }
  allSolids.push_back(magnetContSqSolid);
  allSolids.push_back(magnetAngledFaceSolid);
  magnetContainerSolid = new G4IntersectionSolid(name + "_magnet_container_solid", // name
						 magnetContSqSolid,
						 magnetAngledFaceSolid);

  G4double contRZ = 0.5*length;
  BDSMagnetOuter* magnetContainer = new BDSMagnetOuter(magnetContainerSolid,
						       nullptr,
						       std::make_pair(-contRX,contRX),
						       std::make_pair(-contRY,contRY),
						       std::make_pair(-contRZ,contRZ),
						       nullptr);
  
  G4Box* yokeOuter = new G4Box(name + "_yoke_outer_solid", // name
			       yokeFinishRadius - lengthSafety,           // x half width
			       yokeFinishRadiusY - lengthSafety,          // y half width
			       15.0*outerLength - 3.0*lengthSafety);           // z half width

  G4Box* yokeInner = new G4Box(name + "_yoke_inner_solid", // name
			       yokeStartRadius,            // x half width
			       yokeStartRadiusY,           // y half width
			       15.0*outerLength - 4.0*lengthSafety);             // z half width
  // z long for unambiguous first subtraction and second intersection

  G4SubtractionSolid* yokeSquareSolid = new G4SubtractionSolid(name + "_yoke_square_solid", // name
							       yokeOuter,                   // this
							       yokeInner);                  // minus this
  
  yokeSolid = new G4IntersectionSolid(name + "_yoke_solid", // name
				      yokeSquareSolid,
				      angledFaceSolid);

  allSolids.push_back(yokeOuter);
  allSolids.push_back(yokeInner);

  G4double poleWidth  = 1.7 * beamPipe->GetExtentX().second; // +ve x extent
  if (poleWidth > yokeStartRadius)
    {poleWidth = 0.7 * yokeStartRadius;}
  G4double beamPipeY  = beamPipe->GetExtentY().second;
  G4double poleHeight = yokeStartRadiusY - lengthSafety - beamPipe->GetExtentY().second;
  G4double poleDisp   = (0.5 * poleHeight) + beamPipeY;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "beam pipe half height:  " << beamPipeY  << G4endl;
  G4cout << __METHOD_NAME__ << "pole half width:        " << poleWidth  << G4endl;
  G4cout << __METHOD_NAME__ << "pole half height:       " << poleHeight << G4endl;
  G4cout << __METHOD_NAME__ << "pole y displacement:    " << poleDisp   << G4endl;
#endif

  poleSolid = nullptr;
  if (buildPoles)
    {
      G4Box* poleSquareSolid = new G4Box(name + "_pole_square_solid", // name
					 poleWidth,                   // x half width
					 poleHeight*0.5,                  // y half width
					 15.0*outerLength - 5.0*lengthSafety); // z half width - long for unambiguous intersection
      allSolids.push_back(poleSquareSolid);
      
      poleSolid = new G4IntersectionSolid(name + "_pole_solid", // name
					  poleSquareSolid,
					  angledFaceSolid);
    }

  G4Colour* magnetColour = BDSColours::Instance()->GetMagnetColour(1); /*order = 1*/
  
  BDSMagnetOuterFactoryBase::CreateLogicalVolumes(name, length, magnetColour, outerMaterial);

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

  allPhysicalVolumes.push_back(yokePV);

  if (buildPoles)
    {
      G4PVPlacement* topPolePV = new G4PVPlacement(nullptr,                           // rotation
						   G4ThreeVector(0,poleDisp,0), // position
						   poleLV,                      // logical volume
						   name + "_top_pole_pv",       // name
						   containerLV,                 // mother lv
						   false,                       // no boolean operation
						   0,                           // copy number
						   checkOverlaps);              // check overlaps
      
      G4PVPlacement* botPolePV = new G4PVPlacement(nullptr,                            // rotation
						   G4ThreeVector(0,-poleDisp,0), // position
						   poleLV,                       // logical volume
						   name + "_bottom_pole_pv",     // name
						   containerLV,                  // mother lv
						   false,                        // no boolean operation 
						   0,                            // copy number
						   checkOverlaps);               // check overlaps
      allPhysicalVolumes.push_back(topPolePV);
      allPhysicalVolumes.push_back(botPolePV);
    }
  
  // record extents
  std::pair<double,double> extX = std::make_pair(-yokeFinishRadius,  yokeFinishRadius);
  std::pair<double,double> extY = std::make_pair(-yokeFinishRadiusY, yokeFinishRadiusY);
  std::pair<double,double> extZ = std::make_pair(-length*0.5,length*0.5);
  
  // build the BDSMagnetOuter instance and return it
  BDSMagnetOuter* outer = new BDSMagnetOuter(containerSolid,
					     containerLV,
					     extX, extY, extZ,
					     magnetContainer);
  
  outer->RegisterSolid(allSolids);
  outer->RegisterRotationMatrix(allRotationMatrices);
  outer->RegisterPhysicalVolume(allPhysicalVolumes);
  outer->RegisterVisAttributes(allVisAttributes);
  outer->RegisterUserLimits(allUserLimits);
  
  // Register logical volumes and set sensitivity
  if (buildPoles)
    {
      outer->RegisterLogicalVolume(poleLV);
      outer->RegisterSensitiveVolume(poleLV);
    }
  outer->RegisterLogicalVolume(yokeLV);
  outer->RegisterSensitiveVolume(yokeLV);

  outer->SetEndPieceBefore(endPiece);
  outer->SetEndPieceAfter(endPiece);
  
  return outer;
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::CreateRectangularBend(G4String      name,
								      G4double      length,
								      BDSBeamPipe*  beamPipe,
								      G4double      outerDiameter,
								      G4double      containerDiameter,
								      G4double      containerLength,
								      G4double      angleIn,
								      G4double      angleOut,
								      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CleanUp(); // doesn't use CommonConstructor so must do this manually
  
  return BDSMagnetOuterFactoryCylindrical::Instance()->CreateRectangularBend(name, length, beamPipe, outerDiameter,
									     containerDiameter, containerLength,
									     angleIn, angleOut, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::CreateQuadrupole(G4String      name,
								 G4double      length,
								 BDSBeamPipe*  beamPipe,
								 G4double      outerDiameter,
								 G4double      containerLength,
								 G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return CommonConstructor(name, length, beamPipe, 2, outerDiameter, outerMaterial, containerLength);
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::CreateSextupole(G4String      name,
								G4double      length,
								BDSBeamPipe*  beamPipe,
								G4double      outerDiameter,
								G4double      containerLength,
								G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return CommonConstructor(name, length, beamPipe, 3, outerDiameter, outerMaterial, containerLength);
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::CreateOctupole(G4String      name,
							       G4double      length,
							       BDSBeamPipe*  beamPipe,
							       G4double      outerDiameter,
							       G4double      containerLength,
							       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return CommonConstructor(name, length, beamPipe, 4, outerDiameter, outerMaterial, containerLength);
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::CreateDecapole(G4String      name,
							       G4double      length,
							       BDSBeamPipe*  beamPipe,
							       G4double      outerDiameter,
							       G4double      containerLength,
							       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return CommonConstructor(name, length, beamPipe, 5, outerDiameter, outerMaterial, containerLength);
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::CreateSolenoid(G4String      name,
							       G4double      length,
							       BDSBeamPipe*  beamPipe,
							       G4double      outerDiameter,
							       G4double      containerLength,
							       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return BDSMagnetOuterFactoryCylindrical::Instance()->CreateSolenoid(name,length,beamPipe,outerDiameter,
								      containerLength,outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::CreateMultipole(G4String      name,
								G4double      length,
								BDSBeamPipe*  beamPipe,
								G4double      outerDiameter,
								G4double      containerLength,
								G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return BDSMagnetOuterFactoryCylindrical::Instance()->CreateMultipole(name,length,beamPipe,outerDiameter,
								       containerLength,outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::CreateRfCavity(G4String      name,
							       G4double      length,
							       BDSBeamPipe*  beamPipe,
							       G4double      outerDiameter,
							       G4double      containerLength,
							       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return BDSMagnetOuterFactoryCylindrical::Instance()->CreateRfCavity(name,length,beamPipe,outerDiameter,
								      containerLength,outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::CreateMuSpoiler(G4String      name,
								G4double      length,
								BDSBeamPipe*  beamPipe,
								G4double      outerDiameter,
								G4double      containerLength,
								G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return BDSMagnetOuterFactoryCylindrical::Instance()->CreateMuSpoiler(name,length,beamPipe,outerDiameter,
								       containerLength,outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::CreateKicker(G4String      name,
							     G4double      length,
							     BDSBeamPipe*  beamPipe,
							     G4double      outerDiameter,
							     G4double      containerLength,
							     G4bool        vertical,
							     G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return BDSMagnetOuterFactoryCylindrical::Instance()->CreateKicker(name,length,beamPipe,outerDiameter,
								    containerLength,vertical,outerMaterial);
}

/// functions below here are private to this particular factory
BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::CommonConstructor(G4String     name,
								  G4double     length,
								  BDSBeamPipe* beamPipe,
								  G4int        order,
								  G4double     outerDiameter,
								  G4Material*  outerMaterial,
								  G4double     magnetContainerLength)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // reset all pointers and variables to protect against bugs using previous use of factory
  CleanUp();
  
  CalculatePoleAndYoke(outerDiameter, beamPipe, order);
  if (buildPole)
    {
      CreatePoleSolid(name, length, order);
      CreateCoilSolids(name, length);
    }
  CreateYokeAndContainerSolid(name, length, order, magnetContainerLength);
  if (buildPole)
    {IntersectPoleWithYoke(name, length, order);}
  G4Colour* magnetColour = BDSColours::Instance()->GetMagnetColour(order);
  CreateLogicalVolumes(name, length, magnetColour, outerMaterial);
  CreateMagnetContainerComponent();
  if (buildPole)
    {CreateEndPiece(name);}
  PlaceComponents(name, order); //returns vector of PVs
  if (buildPole)
    {PlaceComponentsCoils(name, order);}
  
  // record extents
  // container radius is just outerDiamter as yoke is circular
  G4double containerRadius = outerDiameter + lengthSafety;
  std::pair<double,double> extX = std::make_pair(-containerRadius,containerRadius);
  std::pair<double,double> extY = std::make_pair(-containerRadius,containerRadius);
  std::pair<double,double> extZ = std::make_pair(-length*0.5,length*0.5);
  
  // build the BDSMagnetOuter instance and return it
  BDSMagnetOuter* outer = new BDSMagnetOuter(containerSolid,
					     containerLV,
					     extX, extY, extZ,
					     magnetContainer);

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

  outer->SetEndPieceBefore(endPiece);
  outer->SetEndPieceAfter(endPiece);
  
  return outer;
}

void BDSMagnetOuterFactoryPolesBase::CalculatePoleAndYoke(G4double     outerDiameter,
							  BDSBeamPipe* beamPipe,
							  G4double     order)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
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
  yokeFinishRadius      = outerDiameter*0.5 - lengthSafety;
  G4double totalLength  = yokeFinishRadius - poleStartRadius;
  poleFinishRadius      = poleStartRadius + poleFraction*totalLength;
  yokeStartRadius       = poleFinishRadius + lengthSafety;
  magnetContainerRadius = yokeFinishRadius + lengthSafetyLarge;
  poleSquareWidth       = (yokeFinishRadius - yokeStartRadius)*1.3;

  G4int nPoles = 2*order;
  // full circle is divided into segments for each pole
  segmentAngle = CLHEP::twopi / (G4double)nPoles;
  poleAngle = segmentAngle * poleAngularFraction;

  G4double minimumPoleFraction = 0.05;
  // If the space occupied by the yoke / pole is < 5% of outerDiameter, don't bother with
  // pole and coil - it's clearly unphysical.
  if ( (yokeFinishRadius - yokeStartRadius) < (minimumPoleFraction * outerDiameter) )
    {
        buildPole = false;
        yokeStartRadius = poleStartRadius; // make the magnet all yoke
    }
}

void BDSMagnetOuterFactoryPolesBase::CreatePoleSolid(G4String     name,
						     G4double     length,
						     G4int        order)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // calculate geometrical parameters first.
  // pole is ellipse at tip, then (possibly) tapered section going outwards and then
  // a section that is straight - ie constant width going outwards. This is later
  // intersected with the yoke solid to give the matching shape. Therefore, the pole
  // is made slightly too long (ie > poleFinishRadius).
    
  // make some % of pole length the curved ellipsoidal surface at the pole tip
  G4double poleLength      = poleFinishRadius - poleStartRadius - 2*lengthSafety;
  G4double ellipsoidHeight = poleTipFraction*poleLength; // full height of an ellipse (2*a)

  // calculate the ellipsoid centre radius from the centre of the magnet.
  G4double ellipsoidCentreY = poleStartRadius + ellipsoidHeight*0.5;
  
  // calculate the width (2*b) of the ellipse given its height - at the centre of the
  // ellipsoid, go out by the pole angle and that's the half width, x2.
  G4double ellipsoidWidth  = 2 * ellipsoidCentreY * tan(poleAngle*0.5);
  // don't allow the pole to be wider than it is long - silly proportions - stop the scaling
  ellipsoidWidth = std::min(ellipsoidWidth, poleLength); 

  // calculate the distance from the centre of the magnet where the straight sides start
  G4double fraction = std::min(0.5, (G4double)order/10.0); // scale slightly with order
  poleSquareStartRadius = ellipsoidCentreY + (poleFinishRadius - ellipsoidCentreY) * fraction;

  // poleSquareWidth is initially calculated in CalculatePoleAndYoke - here we check it
  // ensure pole doesn't get narrower than ellipsoid tip part
  poleSquareWidth = std::max(poleSquareWidth, ellipsoidWidth);
  // but ensure the square section can't protrude outside the angular segment for a single pole
  G4double maxPoleSquareWidth = 2 * poleSquareStartRadius * tan(0.5*poleAngle);
  poleSquareWidth = std::min(maxPoleSquareWidth, poleSquareWidth);

  // points that define extruded polygon
  std::vector<G4TwoVector> points;

  // generate points on a ellipse in the positive quadrant for the pole tip
  // note, the QT visualiser can't visualise the union of an ellipsoid and extruded
  // polygon so can't use that method and this method produces a better transition
  // between the ellipse and the pole. We just generate an extruded solid for the whole pole.
  std::vector<G4double> xEllipse;
  std::vector<G4double> yEllipse;
  const G4int numAngPoints = 6;
  for (G4double angle = 0; angle < CLHEP::halfpi; angle += CLHEP::halfpi/(G4double)numAngPoints)
    {
      xEllipse.push_back(0.5*ellipsoidWidth*sin(angle));
      yEllipse.push_back(0.5*ellipsoidHeight*cos(angle));
    }

  // add bottom left quadrant - miss out first point so don't reach apex.
  for (G4int i = 0; i < (G4int)xEllipse.size()-1; i++)
    {points.push_back(G4TwoVector(xEllipse[i], ellipsoidCentreY-yEllipse[i]));}
  points.push_back(G4TwoVector(poleSquareWidth*0.5, poleSquareStartRadius));
  // top points are x poleStopFactor for unambiguous intersection later on
  points.push_back(G4TwoVector(poleSquareWidth*0.5, poleFinishRadius*poleStopFactor));
  points.push_back(G4TwoVector(-poleSquareWidth*0.5, poleFinishRadius*poleStopFactor));
  points.push_back(G4TwoVector(-poleSquareWidth*0.5, poleSquareStartRadius));
  // add bottom right quadrant - miss out first point so don't reach apex.
  // required start index is of course size()-1 also.
  for (G4int i = (G4int)xEllipse.size()-2; i > 0; i--)
    {points.push_back(G4TwoVector(-xEllipse[i], ellipsoidCentreY-yEllipse[i]));}
  
  G4TwoVector zOffsets(0,0); // the transverse offset of each plane from 0,0
  G4double zScale = 1;       // the scale at each end of the points = 1
  poleSolid = new G4ExtrudedSolid(name + "_pole_raw_solid",  // name
				  points,                    // transverse 2d coordinates
				  length*0.5 - lengthSafety, // z half length
				  zOffsets, zScale,          // dx,dy offset for each face, scaling
				  zOffsets, zScale);         // dx,dy offset for each face, scaling
  
  allSolids.push_back(poleSolid);
}

void BDSMagnetOuterFactoryPolesBase::CreateCoilSolids(G4String name,
						      G4double length)
{
  // create an extruded polygon even though a square to avoid the need for
  // individual rotated translations. This also allows the same rotation
  // to be used for the coils as the poles.
  CreateCoilPoints();

  G4TwoVector zOffsets(0,0); // the transverse offset of each plane from 0,0
  G4double zScale = 1;       // the scale at each end of the points = 1
  
  coilLeftSolid = new G4ExtrudedSolid(name + "_coil_left_solid", // name
				      leftPoints,                // transverse 2d coordinates
				      length*0.5 - lengthSafety, // z half length
				      zOffsets, zScale, // dx,dy offset for each face, scaling
				      zOffsets, zScale);// dx,dy offset for each face, scaling

  coilRightSolid = new G4ExtrudedSolid(name + "_coil_right_solid", // name
				       rightPoints,                // transverse 2d coordinates
				       length*0.5 - lengthSafety, // z half length
				       zOffsets, zScale, // dx,dy offset for each face, scaling
				       zOffsets, zScale);// dx,dy offset for each face, scaling
  
  allSolids.push_back(coilLeftSolid);
  allSolids.push_back(coilRightSolid);
}

void BDSMagnetOuterFactoryPolesBase::CreateCoilPoints()
{
  G4double innerX = 0.5*poleSquareWidth + lengthSafetyLarge;

  // Start the coil just after the pole becomes square in cross-section
  // start it just beyond this point - say + 20% square section length
  G4double lowerY = poleSquareStartRadius + 0.2*(poleFinishRadius - poleSquareStartRadius);

  // At lower Y, project out in X as far as possible within the pole segment
  G4double outerX = 0.9 * lowerY * tan (segmentAngle*0.5);

  // At outer X, we find out the maximum y before hitting the inside of the yoke and
  // back off a wee bit
  G4double upperY = 0.95 * std::abs(std::sqrt(pow(yokeStartRadius,2) - pow(outerX,2)));
  
  coilHeight  = upperY - lowerY;
  G4double dx = outerX - innerX;
  // Check proportions and make roughly square.
  if (coilHeight < dx)
    {
      G4double newXHeight = std::max(dx*0.5, dx - (dx-coilHeight));
      outerX = innerX + newXHeight;
      // recalculate upperY given the new outerX
      upperY = 0.95 * std::abs(std::sqrt(pow(yokeStartRadius,2) - pow(outerX,2)));
    }

  coilCentreRadius = lowerY + 0.5*coilHeight;
  endPieceInnerR   = lowerY - lengthSafetyLarge;
  endPieceOuterR   = yokeStartRadius;
  
  leftPoints.push_back(G4TwoVector(innerX, lowerY));
  leftPoints.push_back(G4TwoVector(outerX, lowerY));
  leftPoints.push_back(G4TwoVector(outerX, upperY));
  leftPoints.push_back(G4TwoVector(innerX, upperY));

  // must be in clockwise order
  rightPoints.push_back(G4TwoVector(-innerX, lowerY));
  rightPoints.push_back(G4TwoVector(-innerX, upperY));
  rightPoints.push_back(G4TwoVector(-outerX, upperY));
  rightPoints.push_back(G4TwoVector(-outerX, lowerY));

  // this will be the eventual length along z but for now its the amplitude in y.
  // make slightly smaller version as endPieceLength used for container dimensions
  endPieceLength = (outerX - innerX) - lengthSafetyLarge;
  for (G4double angle = 0; angle <= CLHEP::halfpi; angle+= CLHEP::halfpi / 8.0)
    {
      G4double x = outerX + endPieceLength * (cos(angle) - 1.0);
      G4double y = endPieceLength * sin(angle);
      endPiecePoints.push_back(G4TwoVector(x,y));
    }
  for (G4double angle = 0; angle <= CLHEP::halfpi; angle+= CLHEP::halfpi / 8.0)
    {
      G4double x = -outerX - endPieceLength * (sin(angle) - 1.0);
      G4double y = endPieceLength * cos(angle);
      endPiecePoints.push_back(G4TwoVector(x,y));
    }
}

void BDSMagnetOuterFactoryPolesBase::CreateYokeAndContainerSolid(G4String name,
								 G4double length,
								 G4int    /*order*/,
								 G4double magnetContainerLength)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif  
  
  // circular yoke so pretty easy
  yokeSolid = new G4Tubs(name + "_yoke_solid",      // name
			 yokeStartRadius,           // start radius
			 yokeFinishRadius,          // finish radius
			 length*0.5 - lengthSafety, // z half length
			 0,                         // start angle
			 CLHEP::twopi);             // sweep angle

  poleIntersectionSolid = new G4Tubs(name + "_yoke_intersection_solid", // name
				     0,                                 // start radius
				     yokeStartRadius - lengthSafetyLarge,
				     length,  // long half length for unamibiguous intersection
				     0,                                 // start angle
				     CLHEP::twopi);                     // sweep angle
  allSolids.push_back(poleIntersectionSolid);
  

  // note container must have hole in it for the beampipe to fit in!
  // poled geometry doesn't fit tightly to beampipe so can alays use a circular aperture
  containerSolid = new G4Tubs(name + "_outer_container_solid", // name
			      poleStartRadius,                 // start radius
			      yokeFinishRadius + lengthSafety, // finish radius
			      length*0.5,                      // z half length
			      0,                               // start angle
			      CLHEP::twopi);                   // sweep angle
  allSolids.push_back(yokeSolid);

  // magnet container radius calculated when poles are calculated and assigned to
  // BDSMagnetOuterFactoryBase::magnetContainerRadius
  BuildMagnetContainerSolidStraight(name, magnetContainerLength, magnetContainerRadius); 
}

void BDSMagnetOuterFactoryPolesBase::IntersectPoleWithYoke(G4String name,
							   G4double /*length*/,
							   G4int  /*order*/)
{ 
  // cut pole here with knowledge of yoke shape.
  poleSolid = new G4IntersectionSolid(name + "_pole_solid",   // name
				      poleSolid,              // solid a
				      poleIntersectionSolid); // solid b
}

void BDSMagnetOuterFactoryPolesBase::CreateLogicalVolumes(G4String    name,
							  G4double    length,
							  G4Colour*   colour,
							  G4Material* outerMaterial)
{
  BDSMagnetOuterFactoryBase::CreateLogicalVolumes(name, length, colour, outerMaterial);
  CreateLogicalVolumesCoil(name);
}

void BDSMagnetOuterFactoryPolesBase::CreateLogicalVolumesCoil(G4String name)
{
  if (coilLeftSolid)
    { // if one exists, all coil solids exist
      G4Material* coilMaterial = BDSMaterials::Instance()->GetMaterial("copper");
      coilLeftLV = new G4LogicalVolume(coilLeftSolid,
				       coilMaterial,
				       name + "_coil_left_lv");

      coilRightLV = new G4LogicalVolume(coilRightSolid,
					coilMaterial,
					name + "_coil_right_lv");
      
      G4Colour* coil = BDSColours::Instance()->GetColour("coil");
      G4VisAttributes* coilVisAttr = new G4VisAttributes(*coil);
      coilVisAttr->SetVisibility(true);

      coilLeftLV->SetVisAttributes(coilVisAttr);
      coilRightLV->SetVisAttributes(coilVisAttr);
      allVisAttributes.push_back(coilVisAttr);
    }
}

void BDSMagnetOuterFactoryPolesBase::TestInputParameters(BDSBeamPipe* beamPipe,
							 G4double&    outerDiameter,
							 G4Material*& outerMaterial)// reference to a pointer
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  //function arguments by reference to they can be modified in place
  //check outer material is something
  if (!outerMaterial)
    {outerMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());}

  // ensure box size is bigger than the beampipe
  if (beamPipe->ContainerIsCircular())
    {
      // if it's circular, just check radius
      if (outerDiameter < 2*(beamPipe->GetContainerRadius()) )
	{outerDiameter = 2*(beamPipe->GetContainerRadius()) + 1*CLHEP::mm;}
    }
  else
    {
      // it's not circular - have a look at extents
      // +ve - -ve
      G4double extentX = beamPipe->GetExtentX().second - beamPipe->GetExtentX().first;
      G4double extentY = beamPipe->GetExtentY().second - beamPipe->GetExtentY().first;
      if ( (outerDiameter < extentX) || (outerDiameter < extentY) )
	{
	  // outerDiameter isn't sufficient for range in x or y
	  outerDiameter = std::max(extentX,extentY) + 1*CLHEP::mm;
	}
    }
}

void BDSMagnetOuterFactoryPolesBase::PlaceComponents(G4String name,
						     G4int    order)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // place the components inside the container
  yokePV = new G4PVPlacement((G4RotationMatrix *) nullptr, // no rotation
			     (G4ThreeVector) 0,            // position
			     yokeLV,                       // lv to be placed
			     name + "_yoke_pv",            // name
			     containerLV,                  // mother lv to be placed in
			     false,                        // no boolean operation
			     0,                            // copy number
			     checkOverlaps);               // whether to check overlaps
  allPhysicalVolumes.push_back(yokePV);
  
  // place poles
  if (!buildPole)
    {return;}
  // else continue and place poles and coils
  G4PVPlacement* aPolePV     = nullptr;  
  for (G4int n = 0; n < 2*order; ++n)
    {
      G4RotationMatrix* rm  = new G4RotationMatrix();
      allRotationMatrices.push_back(rm);
      rm->rotateZ((n+0.5)*segmentAngle + CLHEP::pi*0.5);
      // poleTranslation is by default (0,0,0)
      aPolePV = new G4PVPlacement(rm,                 // rotation
				  poleTranslation,    // position
				  poleLV,             // logical volume
				  name + "_pv",       // name
				  containerLV,        // mother lv to be placed in
				  false,              // no boolean operation
				  n,                  // copy number
				  checkOverlaps);     // check overlaps
      allPhysicalVolumes.push_back(aPolePV);
    }
}

void BDSMagnetOuterFactoryPolesBase::PlaceComponentsCoils(G4String name,
							  G4int    order)
{
  if (!buildPole)
    {return;}
  G4PVPlacement* coilLeftPV   = nullptr;
  G4PVPlacement* coilRightPV  = nullptr;
  G4PVPlacement* endCoilPV    = nullptr;
  G4RotationMatrix* endCoilRM = new G4RotationMatrix();
  endCoilRM->rotateX(CLHEP::halfpi);

  G4ThreeVector endCoilTranslation(0,coilCentreRadius,0.5*endPieceLength);
  
  for (G4int n = 0; n < 2*order; ++n)
    {
      // prepare a new rotation matrix - must be new and can't reuse the same one
      // as the placement doesn't own it - changing the existing one will affect all
      // previously placed objects
      G4RotationMatrix* rm  = new G4RotationMatrix();
      G4RotationMatrix* ecrm = new G4RotationMatrix(*endCoilRM);
      allRotationMatrices.push_back(rm);
      G4double rotationAngle = (n+0.5)*segmentAngle + CLHEP::pi*0.5;
      rm->rotateZ((n+0.5)*segmentAngle + CLHEP::pi*0.5);

      // nominally this should be around z axis, but I guess z is now y give
      // rotation already... could do with rotate about axis in future
      ecrm->rotateY(rotationAngle);

      coilLeftPV = new G4PVPlacement(rm,                 // rotation
				     (G4ThreeVector)0,   // position
				     coilLeftLV,         // logical volume
				     name + "_coil_left_pv", // name      
				     containerLV,        // mother lv to be placed in
				     false,              // no boolean operation
				     n,                  // copy number
				     checkOverlaps);     // check overlaps
      
      coilRightPV = new G4PVPlacement(rm,                 // rotation
				      (G4ThreeVector)0,   // position
				      coilRightLV,        // logical volume
				      name + "_coil_right_pv", // name      
				      containerLV,        // mother lv to be placed in
				      false,              // no boolean operation
				      n,                  // copy number
				      checkOverlaps);     // check overlaps

      G4ThreeVector placementOffset = G4ThreeVector(endCoilTranslation);
      placementOffset.rotateZ(rotationAngle);
      endCoilPV = new G4PVPlacement(ecrm,                    // rotation
				    placementOffset,         // position
				    endPieceCoilLV,          // logical volume
				    name + "_end_piece_coil_pv", // name      
				    endPieceContainerLV,     // mother lv to be placed in
				    false,                   // no boolean operation
				    n,                       // copy number
				    checkOverlaps);          // check overlaps
      endPiece->RegisterPhysicalVolume(endCoilPV);
      endPiece->RegisterRotationMatrix(ecrm);
      
      allPhysicalVolumes.push_back(coilLeftPV);
      allPhysicalVolumes.push_back(coilRightPV);
    }
}

void BDSMagnetOuterFactoryPolesBase::CreateEndPiece(G4String name)
{
  // container solid
  // TBC - this will be too narrow on outside - some trig needed - x2 for now
  G4VSolid* endPieceContainerSolid = new G4Tubs(name + "_end_container_solid", // name
						endPieceInnerR,                // inner radius
						endPieceOuterR,                // outer radius
						endPieceLength*0.5,            // z half length
						0,                             // start angle
						CLHEP::twopi);                 // sweep angle
  
  // container lv
  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());
  endPieceContainerLV = new G4LogicalVolume(endPieceContainerSolid,     // solid
					    emptyMaterial,              // material
					    name + "_end_container_lv");// name
  
  // coil end piece solid
  G4TwoVector zOffsets(0,0); // the transverse offset of each plane from 0,0
  G4double zScale = 1;       // the scale at each end of the points = 1
  G4VSolid* endPieceCoilSolid = new G4ExtrudedSolid(name + "_end_coil_solid", // name
						    endPiecePoints, // transverse 2d coordinates
						    coilHeight*0.5, // z half length
						    zOffsets, zScale,
						    zOffsets, zScale);
  
  // coil end piece lv
  G4Material* copper = BDSMaterials::Instance()->GetMaterial("copper");
  endPieceCoilLV = new G4LogicalVolume(endPieceCoilSolid,      // solid
				       copper,                 // material
				       name + "_end_coil_lv"); // name
				       
  // vis attributes - copy the coil ones that are already built
  G4VisAttributes* endPieceCoilVis = new G4VisAttributes(*(coilLeftLV->GetVisAttributes()));
  endPieceCoilLV->SetVisAttributes(endPieceCoilVis);
  // container vis attributes are held in global constants - only need copy pointer
  endPieceContainerLV->SetVisAttributes(containerLV->GetVisAttributes());
  
  // user limits - copy again - length will be a wee bit too long, but should be ok
  // TBC - geant4 has removed copy constructor for limits - wonderful
  //G4UserLimits* endPieceLimits = new G4UserLimits(*(coilLeftLV->GetUserLimits()));
  //endPieceCoilLV->SetUserLimits(endPieceLimits);

  // geometry component
  auto endPieceGC = new BDSGeometryComponent(endPieceContainerSolid,
					     endPieceContainerLV);
  endPieceGC->RegisterSolid(endPieceCoilSolid);
  endPieceGC->RegisterLogicalVolume(endPieceCoilLV);
  endPieceGC->RegisterVisAttributes(endPieceCoilVis);
  //endPieceGC->RegisterUserLimits(endPieceLimits);
  endPieceGC->SetExtentX(-endPieceOuterR, endPieceOuterR);
  endPieceGC->SetExtentY(-endPieceOuterR, endPieceOuterR);
  endPieceGC->SetExtentZ(-endPieceLength*0.5, endPieceLength*0.5);
  endPieceGC->SetInnerExtentX(-endPieceInnerR, endPieceInnerR);
  endPieceGC->SetInnerExtentY(-endPieceInnerR, endPieceInnerR);
  endPieceGC->SetInnerExtentZ(endPieceGC->GetExtentZ());

  endPiece = new BDSSimpleComponent(name + "_end_piece",
				    endPieceGC,
				    endPieceLength);
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::KickerConstructor(G4String     name,
								  G4double     length,
								  G4double     angle,
								  BDSBeamPipe* beamPipe,
								  G4double     outerDiameter,
								  G4Material*  outerMaterial,
								  G4bool       isVertical)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
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
  G4Material* empty  = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());
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

  allLogicalVolumes.push_back(yokeLV); //note container as done separately in BDSMagnetOuter constructor
  allLogicalVolumes.push_back(poleLV);

  // VISUAL ATTRIBUTES
  // set visual attributes
  std::string magnetType;
  if (isVertical)
    {magnetType = "vkicker";}
  else
    {magnetType = "hkicker";}

  // yoke and pole
  G4Colour* magnetColour = BDSColours::Instance()->GetColour(magnetType);
  G4VisAttributes* outerVisAttr = new G4VisAttributes(*magnetColour);
  outerVisAttr->SetVisibility(true);
  allVisAttributes.push_back(outerVisAttr);
  // no curved surfaces so don't need to set nsegments
  yokeLV->SetVisAttributes(outerVisAttr);
  poleLV->SetVisAttributes(outerVisAttr);

  // coil
  G4Colour* coilColour  = BDSColours::Instance()->GetColour("coil");
  G4VisAttributes* coilVisAttr = new G4VisAttributes(*coilColour);
  coilVisAttr->SetVisibility(true);
  allVisAttributes.push_back(coilVisAttr);
  coilLV->SetVisAttributes(coilVisAttr);

  // container
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());
  
  // user limits
#ifndef NOUSERLIMITS
  G4UserLimits* outerUserLimits = new G4UserLimits("outer_cuts");
  outerUserLimits->SetMaxAllowedStep( length * maxStepFactor );
  outerUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->MaxTime());
  allUserLimits.push_back(outerUserLimits);
  // attach cuts to volumes
  yokeLV->SetUserLimits(outerUserLimits);
  poleLV->SetUserLimits(outerUserLimits);
  coilLV->SetUserLimits(outerUserLimits);
  containerLV->SetUserLimits(outerUserLimits);
#endif

  // place components
  yokePV = new G4PVPlacement((G4RotationMatrix*)nullptr,   // no rotation
			     yokeTranslation,              // position
			     yokeLV,                       // lv to be placed
			     name + "_yoke_pv",            // name
			     containerLV,                  // mother lv to be place in
			     false,                        // no boolean operation
			     0,                            // copy number
			     checkOverlaps);
  
  G4PVPlacement* poleLowerPV = new G4PVPlacement((G4RotationMatrix*)nullptr,   // no rotation
						 lowerPoleTranslation,         // position
						 poleLV,                       // lv to be placed
						 name + "_lower_pole_pv",      // name
						 containerLV,                  // mother lv to be place in
						 false,                        // no boolean operation
						 0,                            // copy number
						 checkOverlaps);

  G4PVPlacement* poleUpperPV = new G4PVPlacement((G4RotationMatrix*)nullptr,   // no rotation
						 upperPoleTranslation,         // position
						 poleLV,                       // lv to be placed
						 name + "_upper_pole_pv",      // name
						 containerLV,                  // mother lv to be place in
						 false,                        // no boolean operation
						 0,                            // copy number
						 checkOverlaps);

  G4PVPlacement* coilOuterPV = new G4PVPlacement((G4RotationMatrix*)nullptr,   // no rotation
						 outerCoilTranslation,         // position
						 coilLV,                       // lv to be placed
						 name + "_outer_coil_pv",      // name
						 containerLV,                  // mother lv to be place in
						 false,                        // no boolean operation
						 0,                            // copy number
						 checkOverlaps);

  G4PVPlacement* coilInnerPV = new G4PVPlacement((G4RotationMatrix*)nullptr,   // no rotation
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
  BDSMagnetOuter* kicker = new BDSMagnetOuter(containerSolid,
					      containerLV,
					      extX, extY, extZ,
					      nullptr, // TO BE COMPLETED!!!
					      containerTranslation);
  
  // register everything
  kicker->RegisterLogicalVolume(allLogicalVolumes);
  kicker->RegisterSensitiveVolume(allLogicalVolumes);
  kicker->RegisterPhysicalVolume(allPhysicalVolumes);
  kicker->RegisterVisAttributes(allVisAttributes);
  kicker->RegisterUserLimits(allUserLimits);
  
  return kicker;
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::CreateDipole(G4String     name,
							     G4double     length,
							     BDSBeamPipe* beamPipe,
							     G4double     outerDiameter,
							     G4double     containerLength,
							     G4double     angleIn,
							     G4double     angleOut,
							     G4Material*  material,
							     G4bool       bendLeft,
							     G4Colour*    colour)
{
  CleanUp();

  // 1 calculations
  // 2 c shaped solid
  // 3 angled solids for intersection
  // 4 intersection solids
  // 5 logical volumes
  // 6 placement
  // general order - yoke, container, magnet container, coils

  // calculate any geometrical parameters
  const auto extXbp = beamPipe->GetExtentX();
  const auto extYbp = beamPipe->GetExtentY();
  const G4double bpHalfWidth    = std::max(std::abs(extXbp.first), std::abs(extXbp.second));
  const G4double bpHalfHeight   = std::max(std::abs(extYbp.first), std::abs(extYbp.second));
  G4double poleHalfWidth  = bpHalfWidth  + lengthSafety;
  poleHalfWidth = std::max(poleHalfWidth, outerDiameter*0.15);
  G4double poleHalfHeight = bpHalfHeight + lengthSafety;
  G4double outerHalf      = outerDiameter * 0.5;

  // outerDiameter must be > max ( bp height , bp width )
  G4double maxOfBP = std::max(poleHalfWidth, poleHalfHeight);
  if (outerHalf <= maxOfBP)
    {outerHalf = maxOfBP + lengthSafetyLarge;}

  // propose yoke thickness
  G4double yokeThickness = 0.23 * outerDiameter;
  // check it's a suitable size and will fit in
  if (yokeThickness > outerHalf - poleHalfHeight)
    {
      buildPole     = false;
      yokeThickness = outerHalf - poleHalfHeight;
    }

  G4double yokeInsideX = poleHalfWidth - outerDiameter + yokeThickness;

  // coil calculations
  // these are full height and width maximum so that they'd fit
  G4double coilHeight = outerHalf - poleHalfHeight - yokeThickness;
  G4double coilWidth  = outerDiameter - yokeThickness - 2*poleHalfWidth;
      
  // reduce the coils from the absolute maximum;
  coilHeight *= 0.8;
  coilWidth  *= 0.65;

  // T = top, B = bottom, L = left, R = right
  G4double coilDX = poleHalfWidth  + 0.5*coilWidth  + lengthSafetyLarge;
  G4double coilDY = poleHalfHeight + 0.5*coilHeight + lengthSafetyLarge;
  G4ThreeVector coilTLDisp = G4ThreeVector( coilDX, coilDY, 0);
  G4ThreeVector coilTRDisp = G4ThreeVector(-coilDX, coilDY, 0);
  G4ThreeVector coilBLDisp = G4ThreeVector( coilDX,-coilDY, 0);
  G4ThreeVector coilBRDisp = G4ThreeVector(-coilDX,-coilDY, 0);
  std::vector<G4ThreeVector> coilDisps;
  coilDisps.push_back(coilTLDisp);
  coilDisps.push_back(coilTRDisp);
  coilDisps.push_back(coilBLDisp);
  coilDisps.push_back(coilBRDisp);

  // create vectors of points for extruded solids
  // create yoke + pole (as one solid about 0,0,0)
  std::vector<G4TwoVector> yokePoints;
  std::vector<G4TwoVector> cPoints;  // container points (for magnet outer only)
  std::vector<G4TwoVector> mCPoints; // magnet container points

  // Typically we have a positive bend angle that (by convention) causes a
  // bend to the -ve x direction in right hadned coordinates. Also, typically,
  // a C shaped magnet has the yoke to the inside so there is an aperture for
  // any radiation to escape to the outside. Therefore, we build the yoke like this
  // and flip it if required. Points are done in clock wise order from the bottom left
  // corner of the top pole.
  const G4double lsl = lengthSafetyLarge; // shortcut
  yokePoints.push_back(G4TwoVector(poleHalfWidth - lsl,  poleHalfHeight - lsl));
  yokePoints.push_back(G4TwoVector(poleHalfWidth - lsl,  outerHalf - lsl));
  yokePoints.push_back(G4TwoVector(poleHalfWidth + lsl - outerDiameter,  outerHalf - lsl));
  yokePoints.push_back(G4TwoVector(poleHalfWidth + lsl - outerDiameter, -outerHalf + lsl));
  yokePoints.push_back(G4TwoVector(poleHalfWidth - lsl, -outerHalf + lsl));
  yokePoints.push_back(G4TwoVector(poleHalfWidth - lsl, -poleHalfHeight - lsl));
  if (buildPole)
    {
      yokePoints.push_back(G4TwoVector(-poleHalfWidth + lsl, -poleHalfHeight));
      yokePoints.push_back(G4TwoVector(-poleHalfWidth + lsl, -outerHalf+yokeThickness));
      yokePoints.push_back(G4TwoVector(yokeInsideX - lsl, -outerHalf+yokeThickness));
      yokePoints.push_back(G4TwoVector(yokeInsideX - lsl,  outerHalf-yokeThickness));
      yokePoints.push_back(G4TwoVector(-poleHalfWidth + lsl,  outerHalf-yokeThickness));
      yokePoints.push_back(G4TwoVector(-poleHalfWidth + lsl,  poleHalfHeight));
    }
  else
    {
      yokePoints.push_back(G4TwoVector(yokeInsideX - lsl, -poleHalfHeight - lsl));
      yokePoints.push_back(G4TwoVector(yokeInsideX - lsl,  poleHalfHeight + lsl));
    }
  
  // points for container for magnet outer only
  cPoints.push_back(G4TwoVector(poleHalfWidth + lsl, poleHalfHeight));
  if (buildPole)
    {
      cPoints.push_back(G4TwoVector(poleHalfWidth + coilWidth + 2*lsl, poleHalfHeight));
      cPoints.push_back(G4TwoVector(poleHalfWidth + coilWidth + 2*lsl, poleHalfHeight + coilHeight + 4*lsl));
      cPoints.push_back(G4TwoVector(poleHalfWidth + lsl, poleHalfHeight + coilHeight + 4*lsl));
    }
  cPoints.push_back(G4TwoVector(poleHalfWidth + lsl, outerHalf + lsl));
  cPoints.push_back(G4TwoVector(poleHalfWidth - outerDiameter - 2*lsl, outerHalf + lsl));
  cPoints.push_back(G4TwoVector(poleHalfWidth - outerDiameter - 2*lsl, -outerHalf - lsl));
  cPoints.push_back(G4TwoVector(poleHalfWidth + lsl, -outerHalf - lsl));
  if (buildPole)
    {
      cPoints.push_back(G4TwoVector(poleHalfWidth + lsl, -poleHalfHeight - coilHeight - 4*lsl));
      cPoints.push_back(G4TwoVector(poleHalfWidth + coilWidth + 2*lsl, -poleHalfHeight - coilHeight - 4*lsl));
      cPoints.push_back(G4TwoVector(poleHalfWidth + coilWidth + 2*lsl, -poleHalfHeight));
    }
  cPoints.push_back(G4TwoVector(poleHalfWidth + lsl, -poleHalfHeight));
  cPoints.push_back(G4TwoVector(yokeInsideX, -poleHalfHeight));
  cPoints.push_back(G4TwoVector(yokeInsideX,  poleHalfHeight));

  // points for container for full magnet object including beam pipe
  mCPoints.push_back(G4TwoVector(poleHalfWidth + 2*lsl, poleHalfHeight));
  if (buildPole)
    {
      mCPoints.push_back(G4TwoVector(poleHalfWidth + coilWidth + 4*lsl, poleHalfHeight));
      mCPoints.push_back(G4TwoVector(poleHalfWidth + coilWidth + 4*lsl, poleHalfHeight + coilHeight + 2*lsl));
      mCPoints.push_back(G4TwoVector(poleHalfWidth + 2*lsl, poleHalfHeight + coilHeight + 2*lsl));
    }
  mCPoints.push_back(G4TwoVector(poleHalfWidth + 2*lsl,  outerHalf + 2*lsl));
  mCPoints.push_back(G4TwoVector(poleHalfWidth - outerDiameter - 2*lsl,  outerHalf + 2*lsl));
  mCPoints.push_back(G4TwoVector(poleHalfWidth - outerDiameter - 2*lsl, -outerHalf - 2*lsl));
  mCPoints.push_back(G4TwoVector(poleHalfWidth + 2*lsl, -outerHalf - 2*lsl));
  if (buildPole)
    {
      mCPoints.push_back(G4TwoVector(poleHalfWidth + 2*lsl, -poleHalfHeight - coilHeight - 2*lsl));
      mCPoints.push_back(G4TwoVector(poleHalfWidth + coilWidth + 4*lsl, -poleHalfHeight - coilHeight - 2*lsl));
      mCPoints.push_back(G4TwoVector(poleHalfWidth + coilWidth + 4*lsl, -poleHalfHeight));
    }
  mCPoints.push_back(G4TwoVector(poleHalfWidth + 2*lsl, -poleHalfHeight));
  
  // record extents - do here as all the extents are bascially calcualted here
  G4double mx = poleHalfWidth - outerDiameter - 2*lsl;
  G4double px = poleHalfWidth + coilWidth + 4*lsl;
  G4double my = -outerHalf - 2*lsl;
  G4double py = outerHalf + 2*lsl;
  std::pair<double,double> extX = std::make_pair(mx,px);
  std::pair<double,double> extY = std::make_pair(my,py);
  std::pair<double,double> extZ = std::make_pair(-length*0.5,length*0.5);
  
  if (bendLeft)
    {
      // flip x component so geometry is reflected horizontally
      for (auto& vec : yokePoints)
	{vec.setX(vec.x() * -1);}
      // reverse order so it's clockwise as geant4 requires
      std::reverse(yokePoints.begin(), yokePoints.end());

      // same for container
      for (auto& vec : cPoints)
	{vec.setX(vec.x() * -1);}
      std::reverse(cPoints.begin(), cPoints.end());

      // same for magnet container
      for (auto& vec : mCPoints)
	{vec.setX(vec.x() * -1);}
      std::reverse(mCPoints.begin(), mCPoints.end());
    }

  G4double sLength = length; // default is normal length
  // if we have angled faces, make square faced solids longer for intersection.
  if (BDS::IsFinite(angleIn) || BDS::IsFinite(angleOut))
    {
      // create yoke, coil, container and magnet container solids
      // create a safe length here so solids are long enough for intersection
      G4double dzIn  = tan(std::abs(angleIn))  * outerDiameter; // over estimation, but ok
      G4double dzOut = tan(std::abs(angleOut)) * outerDiameter;
      // safe length for intersection
      sLength = std::max(2*length, 1.5*length + dzIn + dzOut);
    }
    
  G4TwoVector zOffsets(0,0); // the transverse offset of each plane from 0,0
  G4double zScale = 1;       // the scale at each end of the points = 1
  yokeSolid = new G4ExtrudedSolid(name + "_yoke_sq_solid",     // name
				  yokePoints,                  // transverse 2d coordinates
				  sLength*0.5 - lengthSafety,  // z half length
				  zOffsets, zScale,  // dx,dy offset for each face, scaling
				  zOffsets, zScale); // dx,dy offset for each face, scaling
  
  containerSolid = new G4ExtrudedSolid(name + "_outer_sq_container_solid", // name
				       cPoints,                    // transverse 2d coordinates
				       sLength*0.5 - lengthSafety, // z half length
				       zOffsets, zScale, // dx,dy offset for each face, scaling
				       zOffsets, zScale);// dx,dy offset for each face, scaling

  magnetContainerSolid = new G4ExtrudedSolid(name + "_sq_container_solid", // name
					     mCPoints,                     // transverse 2d coordinates
					     sLength*0.5 - lengthSafety,   // z half length
					     zOffsets, zScale, // dx,dy offset for each face, scaling
					     zOffsets, zScale);// dx,dy offset for each face, scaling


  // create coil - one solid that will be placed 4 times... or
  // if we have angled faces, they're all unique unfortunately so use a vector of solids
  // and place individually
  G4VSolid* coilSolid = nullptr;
  std::vector<G4VSolid*>        coilsSolids;
  std::vector<G4LogicalVolume*> coilLVs;
  G4bool individualCoilsSolids = false;
  if (buildPole)
    {
      coilSolid = new G4Box(name + "_coil_solid",        // name
			    coilWidth*0.5,               // x half width
			    coilHeight*0.5,              // y half height
			    sLength*0.5 - lengthSafety); // z half length
    }

  // Intersect and replace solids. Do it via replacmeent of the base class member G4VSolid*
  // as the intersection is only done if one of the angles is finite.

  // If we have angled faces, we need to also intersect the outer magnet container and
  // we therefore have an extra solid that should belong to that geometry component
  // and not the magnet outer - create a vector for holding any extra solids. These can
  // only be registered once the container object is created and that can only happen
  // once the solid and logical volumes are fully complete.
  std::vector<G4VSolid*> magnetContainerExtraSolids;

  // Declare and intialise angle variables here as they're used in multiple places later on
  // They will only be used if either angle is finite, but need in this scope.
  std::pair<G4ThreeVector,G4ThreeVector> faces = std::make_pair(G4ThreeVector(), G4ThreeVector());
  G4ThreeVector inputface = G4ThreeVector();
  G4ThreeVector outputface = G4ThreeVector();
  if (BDS::IsFinite(angleIn) || BDS::IsFinite(angleOut))
    { 
      faces = BDS::CalculateFaces(angleIn,angleOut);
      inputface  = faces.first;
      outputface = faces.second;
      
      // angled solid for magnet outer and coils
      G4VSolid* angledFaces = new G4CutTubs(name + "_angled_face_solid", // name
					    0,                           // inner radius
					    outerDiameter,               // outer radius
					    length*0.5 - lengthSafety,   // z half length
					    0,                           // start angle
					    CLHEP::twopi,                // sweep angle
					    inputface,                   // input face normal
					    outputface);                 // output face normal
      
      // angled solid for magnet outer container volume
      G4VSolid* angledFacesCont = new G4CutTubs(name + "_angled_face_cont_solid", // name
						0,                           // inner radius
						outerDiameter,               // outer radius
						length*0.5,                  // z half length
						0,                           // start angle
						CLHEP::twopi,                // sweep angle
						inputface,                   // input face normal
						outputface);                 // output face normal
      
      // angled solid for full magnet container
      G4VSolid* angledFacesMagCont = new G4CutTubs(name + "_angled_face_mag_cont_solid", // name
						   0,                    // inner radius
						   outerDiameter,        // outer radius
						   containerLength*0.5,  // z half length
						   0,                    // start angle
						   CLHEP::twopi,         // sweep angle
						   inputface,            // input face normal
						   outputface);          // output face normal

      // register angled solids
      allSolids.push_back(angledFaces);
      allSolids.push_back(angledFacesCont);
      allSolids.push_back(angledFacesMagCont);

      // register existing square solids as we're going to overwrite them with intersected ones
      allSolids.push_back(yokeSolid);
      allSolids.push_back(containerSolid);
      allSolids.push_back(coilSolid);
      allSolids.push_back(magnetContainerSolid);
      
      // now do intersections overwriting existing pointers
      yokeSolid = new G4IntersectionSolid(name + "_yoke_solid", // name
					  yokeSolid,            // solid a - existing yoke
					  angledFaces);         // solid b
      
      containerSolid = new G4IntersectionSolid(name + "_outer_container_solid", // name
					       containerSolid,                  // solid a
					       angledFacesCont);                // solid b

      magnetContainerSolid = new G4IntersectionSolid(name + "_container_solid", // name
						     magnetContainerSolid,      // solid a
						     angledFacesMagCont);       // solid b

      individualCoilsSolids = true; // flag that we have individual coil solids
      for (G4int i = 0; i < 4; i++)
	{
	  G4VSolid* coilS = new G4IntersectionSolid(name + "_pole_solid_" + std::to_string(i), // name
						    angledFaces,                // solid a
						    coilSolid,                  // solid b
						    (G4RotationMatrix*)nullptr, // 0 rotation
						    coilDisps[i]);              // translation
	  coilsSolids.push_back(coilS);
	  allSolids.push_back(coilS);
	}
    }
  
  // logical volumes
  BDSMagnetOuterFactoryBase::CreateLogicalVolumes(name, length, colour, material);
  // we only use one coil solid here so do that here
  G4LogicalVolume* coilLV;
  if (buildPole)
    {
      G4Colour* coil = BDSColours::Instance()->GetColour("coil");
      G4VisAttributes* coilVis = new G4VisAttributes(*coil);
      coilVis->SetVisibility(true);
      allVisAttributes.push_back(coilVis);
      G4Material* coilMaterial = BDSMaterials::Instance()->GetMaterial("copper");
      if (individualCoilsSolids)
	{
	  for (G4int i = 0; i < 4; i++)
	    {
	      G4String theName =  name + "_coil_solid_" + std::to_string(i);
	      G4LogicalVolume* aCoilLV = new G4LogicalVolume(coilsSolids[i],
							     coilMaterial,
							     theName);
	      aCoilLV->SetVisAttributes(coilVis);
	      coilLVs.push_back(aCoilLV);
	      allLogicalVolumes.push_back(aCoilLV);
	    }
	}
      else
	{
	  coilLV = new G4LogicalVolume(coilSolid,
				       coilMaterial,
				       name + "_coil_lv");
	  coilLV->SetVisAttributes(coilVis);
	  allLogicalVolumes.push_back(coilLV);
	}
    }
  
  // placement
  // place yoke+pole (one solid) together
  G4bool buildPoleOriginal = buildPole;
  buildPole = false; // this will avoid PlaceComponents trying to place poles that don't exist
  PlaceComponents(name, 1); // places yoke
  buildPole = buildPoleOriginal;
  // place coils
  if (buildPole)
    {
      G4PVPlacement* aCoilPV = nullptr;
      // coils intersection was done such that the coordinates of the resultant solid
      // are the same as the plain coil G4Box. This allows us to programmatically place
      // the coils here irrespective if there's one or 4 unqiue coils.
      for (G4int i = 0; i < 4; i++)
	{
	  G4LogicalVolume* volToPlace = coilLV;
	  G4ThreeVector displacement = G4ThreeVector();
	  if (individualCoilsSolids)
	    {volToPlace = coilLVs[i];}
	  else
	    {displacement = coilDisps[i];} // with no intersection we have to displace it
	  G4String theName = name + "_coil_" + std::to_string(i) + "_pv";
	  aCoilPV = new G4PVPlacement((G4RotationMatrix*)nullptr, // no rotation
				      displacement,               // position
				      volToPlace,                 // lv to be placed
				      theName,                    // name
				      containerLV,                // mother lv to be place in
				      false,                      // no boolean operation
				      0,                          // copy number
				      checkOverlaps);
	  allPhysicalVolumes.push_back(aCoilPV);
	}
    }
  
  // end pieces
  // note with bends both are likely to be different so build independently here
  //  G4TwoVector insideAxis;

  // end piece coil solids
  // build in x-y plane and project in z as that's the only way with an extruded solid
  // then rotate so it's along z
  // curved sections are elliptical with major axis being the projected with of the coil
  // at an angle and the minor being the normal coil width.
  // x = x0 + acos(t)cos(phi) - bsin(t)sin(phi)
  // y = y0 + acos(t)sin(phi) + bsin(t)cos(phi)
  std::vector<G4TwoVector> inEPPoints;  // input face end piece points
  std::vector<G4TwoVector> outEPPoints; // output face end piece points

  G4double inXO = poleHalfWidth + lengthSafetyLarge;
  G4double a    = coilWidth / cos(angleIn); // projected coil width - major axis of ellipse
  G4double b    = coilWidth;

  // create an ellipse with no angle, then shear it to match the angle
  // left side
  for (G4double t = -CLHEP::pi; t <= -CLHEP::halfpi; t += CLHEP::halfpi/8.0)
    {
      G4double x = -inXO + a*cos(t);
      G4double y = b*sin(t);
      inEPPoints.push_back(G4TwoVector(x,y));
    }
  // right side
  for (G4double t = -CLHEP::halfpi; t <= 0; t += CLHEP::halfpi/8.0)
    {
      G4double x = inXO + a*cos(t);
      G4double y = b*sin(t);
      inEPPoints.push_back(G4TwoVector(x,y));
    }

  // shear it
  // (x')   (1        0) (x)
  // (y') = (tan(phi) 1) (y)
  // x' = x; y' = tan(phi)*x + y
  // copy the points, flip in y and shear for output angle
  for (const auto point : inEPPoints) // copying this time
    {
      G4double outy = -1*(point.x()*tan(angleOut) + point.y());
      outEPPoints.push_back(G4TwoVector(point.x(),outy));
    }
  // simply modify in place for shearing original points
  for (auto& point : inEPPoints)
    {point.setY(point.x()*tan(-angleIn) + point.y());}

  // these are projected by coilHeight in z as they'll be rotated later
  G4VSolid* endPieceSolidIn  = new G4ExtrudedSolid(name + "_end_coil_in_solid", // name
						   inEPPoints, // transverse 2d coordinates
						   coilHeight*0.5, // z half length
						   zOffsets, zScale,
						   zOffsets, zScale);
  
  // end piece container solids
  // simple extruded solid intersectd with cut tubs for angled faces
  // build about magnet zero so we get the coordinates right for general placement
  std::vector<G4TwoVector> contEPPoints;
  G4double xmax = poleHalfWidth + coilWidth + 1*CLHEP::mm;
  G4double ymax = poleHalfHeight + coilHeight + 1*CLHEP::mm;
  contEPPoints.push_back(G4TwoVector(xmax + 1*CLHEP::mm,  ymax));
  contEPPoints.push_back(G4TwoVector(-xmax,  ymax));
  contEPPoints.push_back(G4TwoVector(-xmax,  poleHalfHeight));
  contEPPoints.push_back(G4TwoVector( xmax,  poleHalfHeight));
  contEPPoints.push_back(G4TwoVector( xmax, -poleHalfHeight));
  contEPPoints.push_back(G4TwoVector(-xmax, -poleHalfHeight));
  contEPPoints.push_back(G4TwoVector(-xmax, -ymax));
  contEPPoints.push_back(G4TwoVector(xmax + 1*CLHEP::mm, -ymax));
  
  // calculate length for each end piece container volume - could be different due to different angles
  G4double ePInLength  = coilWidth + lengthSafetyLarge; // adjustable for intersection
  G4double ePInLengthZ = ePInLength; // copy that will be final length of object
  if (BDS::IsFinite(angleIn))
    {
      G4double dzIn = tan(std::abs(angleIn)) * outerDiameter;
      ePInLength    = std::max(2*ePInLength, 2*dzIn); // 2x as long for intersection
    }

  G4VSolid* ePContSolidIn  = new G4ExtrudedSolid(name + "_end_coil_solid", // name
						 contEPPoints,   // transverse 2d coordinates
						 ePInLength*0.5, // z half length
						 zOffsets, zScale,
						 zOffsets, zScale);

  // need to keep memory separate for each end piece so duplicate container square
  //G4VSolid* ePContSolidSqOut = ePContSolidSqIn->Clone();

  // If there is a finite input face angle, the extruded solid above will be longer and
  // we now make a cut tubs for the angled faces to intersect it with.
  if (BDS::IsFinite(angleIn))
    {
      
      G4ThreeVector inputfaceReversed = inputface * -1;
      G4VSolid* ePContSolidInAng = new G4CutTubs(name + "_angled_face_solid", // name
						 0,                           // inner radius
						 outerDiameter,               // outer radius
						 ePInLengthZ*0.5,             // z half length
						 0,                           // start angle
						 CLHEP::twopi,                // sweep angle
						 inputface,                   // input face normal
						 inputfaceReversed);          // output face normal

      // potential memory leak here with overwriting
      ePContSolidIn = new G4IntersectionSolid(name + "_end_coil_cont_solid", // name
					      ePContSolidIn,
					      ePContSolidInAng);
    }

  // end piece logical volumes
  // coil end piece lv
  G4Material* copper = BDSMaterials::Instance()->GetMaterial("copper");
  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());
  
  G4LogicalVolume* ePInLV = new G4LogicalVolume(endPieceSolidIn,           // solid
						copper,                    // material
						name + "_end_coil_in_lv"); // name

  G4LogicalVolume* ePContInLV = new G4LogicalVolume(ePContSolidIn,             // solid
						    emptyMaterial,             // material
						    name + "_end_cont_in_lv"); // name

  G4Colour* coilColour = BDSColours::Instance()->GetColour("coil");
  G4VisAttributes* coilVis = new G4VisAttributes(*coilColour);
  coilVis->SetVisibility(true);

  ePInLV->SetVisAttributes(coilVis);
  ePContInLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());
  
  ePInLV->SetUserLimits(BDSGlobalConstants::Instance()->GetDefaultUserLimits());
  ePContInLV->SetUserLimits(BDSGlobalConstants::Instance()->GetDefaultUserLimits());
  
  // placements
  G4RotationMatrix* endCoilInRM = new G4RotationMatrix();
  endCoilInRM->rotateX(-CLHEP::halfpi);

  G4ThreeVector endCoilTranslationTop(0, coilDY, 0.5*coilWidth);
  G4ThreeVector endCoilTranslationLow(0,-coilDY, 0.5*coilWidth);
  G4PVPlacement* ePInTopPv = new G4PVPlacement(endCoilInRM,        // rotation
					       endCoilTranslationTop, // position
					       ePInLV,             // logical volume
					       name + "_end_piece_in_top_pv", // name
					       ePContInLV,         // mother lv to be placed in
					       false,              // no boolean operation
					       0,                  // copy number
					       checkOverlaps);     // check overlaps
  G4PVPlacement* ePInLowPv = new G4PVPlacement(endCoilInRM,        // rotation
					       endCoilTranslationLow, // position
					       ePInLV,             // logical volume
					       name + "_end_piece_in_low_pv", // name
					       ePContInLV,         // mother lv to be placed in
					       false,              // no boolean operation
					       0,                  // copy number
					       checkOverlaps);     // check overlaps

  
  // packaging - geometry component
  auto endPieceInGC = new BDSGeometryComponent(ePContSolidIn,
					       ePContInLV);
  endPieceInGC->RegisterPhysicalVolume(ePInTopPv);
  endPieceInGC->RegisterPhysicalVolume(ePInLowPv);
  endPieceInGC->RegisterRotationMatrix(endCoilInRM);
  endPieceInGC->RegisterVisAttributes(coilVis);
  endPieceInGC->RegisterLogicalVolume(ePInLV);
  endPieceInGC->RegisterSolid(endPieceSolidIn);

  endPieceInGC->RegisterSensitiveVolume(ePInLV);

  BDSSimpleComponent* endPieceInSC = new BDSSimpleComponent(name + "_end_piece_in",
							    endPieceInGC,
							    ePInLengthZ);
  // back to general assembly here
  
  // magnet outer instance
  CreateMagnetContainerComponent();
  magnetContainer->RegisterSolid(magnetContainerExtraSolids);

  // build the BDSMagnetOuter instance and return it
  BDSMagnetOuter* outer = new BDSMagnetOuter(containerSolid,
					     containerLV,
					     extX, extY, extZ,
					     magnetContainer);
  
  outer->RegisterSolid(allSolids);
  outer->RegisterLogicalVolume(allLogicalVolumes);
  outer->RegisterRotationMatrix(allRotationMatrices);
  outer->RegisterPhysicalVolume(allPhysicalVolumes);
  outer->RegisterVisAttributes(allVisAttributes);
  outer->RegisterUserLimits(allUserLimits);
  
  outer->RegisterSolid(yokeSolid);
  outer->RegisterLogicalVolume(yokeLV);

  outer->RegisterSensitiveVolume(yokeLV);
  if (individualCoilsSolids)
    {outer->RegisterSensitiveVolume(coilLVs);}
  else
    {outer->RegisterSensitiveVolume(coilLV);}

  outer->SetEndPieceBefore(endPieceInSC);
  //outer->SetEndPieceAfter(endPiece);
  
  return outer;
}
