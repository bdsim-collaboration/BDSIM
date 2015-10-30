#include "BDSMagnetOuterFactoryPolesBase.hh"

#include "BDSBeamPipe.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
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
#include "G4LogicalVolume.hh"
#include "G4IntersectionSolid.hh"
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
  bendHeightFraction  = 0.7;

  // now the base class constructor should be called first which
  // should call clean up (in the derived class) which should initialise
  // the variables I think, but doing here just to be sure.
  CleanUp();
}

void BDSMagnetOuterFactoryPolesBase::CleanUp()
{
  BDSMagnetOuterFactoryBase::CleanUp();
  
  poleStartRadius       = 0;
  poleFinishRadius      = 0;
  yokeStartRadius       = 0;
  yokeFinishRadius      = 0;
  magnetContainerRadius = 0;
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::CreateSectorBend(G4String      name,
								 G4double      length,
								 BDSBeamPipe*  beamPipe,
								 G4double      outerDiameter,
								 G4double      containerLength,
								 G4double      angle,
								 G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CleanUp(); // doesn't use CommonConstructor so must do this manually2012

  // test input parameters - set global options as default if not specified
  TestInputParameters(beamPipe,outerDiameter,outerMaterial);

  G4int orientation   = BDS::CalculateOrientation(angle);
  G4double zcomponent = cos(fabs(angle*0.5)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double xcomponent = sin(fabs(angle*0.5)); // note full angle here as it's the exit angle
  G4ThreeVector inputface  = G4ThreeVector(-orientation*xcomponent, 0.0, -1.0*zcomponent); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(-orientation*xcomponent, 0.0, zcomponent);   // no output face angle

  G4double beampipeRadiusX = std::max(beamPipe->GetExtentX().first, beamPipe->GetExtentX().second);
  G4double beampipeRadiusY = std::max(beamPipe->GetExtentY().first, beamPipe->GetExtentY().second);

  yokeFinishRadius = 0.5 * outerDiameter;
  yokeStartRadius  = (poleFraction * yokeFinishRadius) + lengthSafetyLarge;
  G4double yokeFinishRadiusY = yokeFinishRadius * bendHeightFraction;
  G4double yokeThickness     = yokeFinishRadius - yokeStartRadius;
  G4double yokeStartRadiusY  = yokeFinishRadiusY - yokeThickness;
  G4double outerLength       = (0.5 * length) - lengthSafety;
  // make angled face cylinder big enough to encompass it all
  G4double angledFaceRadius  = 1.5 * sqrt(pow(yokeFinishRadius, 2) + pow(yokeFinishRadiusY, 2));

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
					  1.5 * outerLength); // z half width - long for unambiguous intersection
  
  G4VSolid* angledFaceSolid = nullptr;
  if (BDS::IsFinite(angle))
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
				       containerLength); // z half width - long for unambiguous intersection
  G4VSolid* magnetAngledFaceSolid = nullptr;
  if (BDS::IsFinite(angle))
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
			       1.5*outerLength);           // z half width

  G4Box* yokeInner = new G4Box(name + "_yoke_inner_solid", // name
			       yokeStartRadius,            // x half width
			       yokeStartRadiusY,           // y half width
			       2*outerLength);             // z half width
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
					 1.5*outerLength); // z half width - long for unambiguous intersection
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
  yokePV = new G4PVPlacement((G4RotationMatrix*)0,         // no rotation
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
  outer->RegisterLogicalVolume(poleLV);
  outer->RegisterSensitiveVolume(poleLV);
  outer->RegisterLogicalVolume(yokeLV);
  outer->RegisterSensitiveVolume(yokeLV);
  
  return outer;
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesBase::CreateRectangularBend(G4String      name,
								      G4double      length,
								      BDSBeamPipe*  beamPipe,
								      G4double      outerDiameter,
								      G4double      containerDiameter,
								      G4double      containerLength,
								      G4double      angle,
								      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CleanUp(); // doesn't use CommonConstructor so must do this manually
  
  return BDSMagnetOuterFactoryCylindrical::Instance()->CreateRectangularBend(name, length, beamPipe, outerDiameter,
									     containerDiameter, containerLength,
									     angle, outerMaterial);
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
  CreatePoleSolid(name, length, order);
  CreateYokeAndContainerSolid(name, length, order, magnetContainerLength);  
  G4Colour* magnetColour = BDSColours::Instance()->GetMagnetColour(order);
  CreateLogicalVolumes(name, length, magnetColour, outerMaterial);
  CreateMagnetContainerComponent();
  PlaceComponents(name, order); //returns vector of PVs
  
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
  
  return outer;
}

void BDSMagnetOuterFactoryPolesBase::CalculatePoleAndYoke(G4double     outerDiameter,
							  BDSBeamPipe* beamPipe,
							  G4double     /*order*/)
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
}

void BDSMagnetOuterFactoryPolesBase::CreatePoleSolid(G4String     name,
						     G4double     length,
						     G4int        order)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
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
    {outerMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());}

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
  if (visDebug)
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
  BDSMagnetOuter* kicker = new BDSMagnetOuter(containerSolid,
					      containerLV,
					      extX, extY, extZ,
					      NULL, // TO BE COMPLETED!!!
					      containerTranslation);
  
  // register everything
  kicker->RegisterLogicalVolume(allLogicalVolumes);
  kicker->RegisterSensitiveVolume(allLogicalVolumes);
  kicker->RegisterPhysicalVolume(allPhysicalVolumes);
  kicker->RegisterVisAttributes(allVisAttributes);
  kicker->RegisterUserLimits(allUserLimits);
  
  return kicker;
}
