#include "BDSMagnetOuterFactoryCylindrical.hh"

#include "BDSBeamPipe.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"                 // for calculateorientation

#include "globals.hh"                      // geant4 globals / types
#include "G4Colour.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"

#include <cmath>
#include <utility>                         // for std::pair
#include <algorithm>                       // for std::max

BDSMagnetOuterFactoryCylindrical* BDSMagnetOuterFactoryCylindrical::_instance = nullptr;

BDSMagnetOuterFactoryCylindrical* BDSMagnetOuterFactoryCylindrical::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSMagnetOuterFactoryCylindrical();}
  return _instance;
}

BDSMagnetOuterFactoryCylindrical::BDSMagnetOuterFactoryCylindrical()
{;}

BDSMagnetOuterFactoryCylindrical::~BDSMagnetOuterFactoryCylindrical()
{
  _instance = nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateSectorBend(G4String     name,
								   G4double     length,
								   BDSBeamPipe* beamPipe,
								   G4double     outerDiameter,
								   G4double     containerLength,
								   G4double     angle,
								   G4Material*  outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // clear up variables
  CleanUp();
  
  // test input parameters - set global options as default if not specified
  TestInputParameters(beamPipe,outerDiameter,outerMaterial);

  if (!BDS::IsFinite(angle))
    {
      CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
      return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				    BDSColours::Instance()->GetColour("sectorbend"));
    }
  
  G4int orientation   = BDS::CalculateOrientation(angle);
  G4double zcomponent = cos(fabs(angle*0.5)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double xcomponent = sin(fabs(angle*0.5)); // note full angle here as it's the exit angle
  G4ThreeVector inputface  = G4ThreeVector(-orientation*xcomponent, 0.0, -1.0*zcomponent); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(-orientation*xcomponent, 0.0, zcomponent);   // no output face angle
  
  // build the container for the whole magnet object - this outer diameter should be
  // larger than the magnet outer piece diameter which is just 'outerDiameter' wide.
  G4double magnetContainerRadius = (0.5 * outerDiameter) + lengthSafety;
  BuildMagnetContainerSolidAngled(name, containerLength, magnetContainerRadius,
				  inputface, outputface);
    
  if (beamPipe->ContainerIsCircular())
    {
      //circular beampipe so we can simply use its radius
      yokeSolid = new G4CutTubs(name + "_yoke_solid",        // name
				beamPipe->GetContainerRadius() + 2*lengthSafety,  // inner radius
				outerDiameter*0.5,           // outer radius
				length*0.5-2*lengthSafety,   // half length
				0,                           // rotation start angle
				CLHEP::twopi,                // rotation finish angle
				inputface,                   // input face normal
				outputface);                 // output face normal

      //container is similar but slightly wider and hollow (to allow placement of beampipe)
      containerSolid = new G4CutTubs(name + "_contiainer_solid",       // name
				     beamPipe->GetContainerRadius() + lengthSafety, // inner radius
				     outerDiameter*0.5 + lengthSafety, // outer radius
				     length*0.5,                       // half length
				     0,                                // rotation start angle
				     CLHEP::twopi,                     // rotation finish angle
				     inputface,                        // input face normal
				     outputface);                      // output face normal
    }
  else
    {
      G4VSolid* yokeSolidCylinder = new G4CutTubs(name + "_yoke_solid_cylinder",// name
						  0,  // inner radius - for unambiguous subtraction
						  outerDiameter*0.5,            // outer radius
						  length*0.5-2*lengthSafety,    // half length
						  0,                            // rotation start angle
						  CLHEP::twopi,                 // rotation finish angle
						  inputface,                    // input face normal
						  outputface);                  // output face normal
      allSolids.push_back(yokeSolidCylinder);
      yokeSolid = new G4SubtractionSolid(name + "_yoke_solid",
					  yokeSolidCylinder,
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
      allSolids.push_back(containerSolidCylinder);
      containerSolid = new G4SubtractionSolid(name + "_container_solid",
					      containerSolidCylinder,
					      beamPipe->GetContainerSubtractionSolid());
    }
  return CommonFinalConstructor(name,length,outerDiameter,outerMaterial,
				BDSColours::Instance()->GetColour("sectorbend"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateRectangularBend(G4String     name,
									G4double     length,
									BDSBeamPipe* beamPipe,
									G4double     outerDiameter,
									G4double     containerDiameter,
									G4double     containerLength,
									G4double     angle,
									G4Material*  outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif  
  //rectangular bends currently just make a shorter straight volume, so ignore angle for now

  // clear up variables
  CleanUp();
  
  // test input parameters - set global options as default if not specified
  TestInputParameters(beamPipe,outerDiameter,outerMaterial);

  G4int orientation   = BDS::CalculateOrientation(angle);
  G4double zcomponent = cos(fabs(angle*0.5)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double xcomponent = sin(fabs(angle*0.5)); // note full angle here as it's the exit angle
  G4ThreeVector inputface  = G4ThreeVector(-orientation*xcomponent, 0.0, -1.0*zcomponent); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(-orientation*xcomponent, 0.0, zcomponent);   // no output face angle

  G4double magnetContainerRadius = 0.5*containerDiameter;
  
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);

  // delete the magnet container solid created by default in CreateCylindricalSolids
  // (common to all apart from this one)
  delete magnetContainerSolid;
  // make a new magnet container solid
  BuildMagnetContainerSolidAngled(name, containerLength, magnetContainerRadius,
				  inputface, outputface);
  
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("rectangularbend"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateQuadrupole(G4String     name,
								   G4double     length,
								   BDSBeamPipe* beamPipe,
								   G4double     outerDiameter,
								   G4double     containerLength,
								   G4Material*  outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("quadrupole"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateSextupole(G4String     name,
								  G4double     length,
								  BDSBeamPipe* beamPipe,
								  G4double     outerDiameter,
								  G4double     containerLength,
								  G4Material*  outerMaterial)
{
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("sextupole"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateOctupole(G4String     name,
								 G4double     length,
								 BDSBeamPipe* beamPipe,
								 G4double     outerDiameter,
								 G4double     containerLength,
								 G4Material*  outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("octupole"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateDecapole(G4String     name,
								 G4double     length,
								 BDSBeamPipe* beamPipe,
								 G4double     outerDiameter,
								 G4double     containerLength,
								 G4Material*  outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("decapole"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateSolenoid(G4String     name,
								 G4double     length,
								 BDSBeamPipe* beamPipe,
								 G4double     outerDiameter,
								 G4double     containerLength,
								 G4Material*  outerMaterial)
{
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("solenoid"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateMultipole(G4String     name,
								  G4double     length,
								  BDSBeamPipe* beamPipe,
								  G4double     outerDiameter,
								  G4double     containerLength,
								  G4Material*  outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("multipole"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateRfCavity(G4String     name,
								 G4double     length,
								 BDSBeamPipe* beamPipe,
								 G4double     outerDiameter,
								 G4double     containerLength,
								 G4Material*  outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("rfcavity"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateMuSpoiler(G4String     name,
								  G4double     length,
								  BDSBeamPipe* beamPipe,
								  G4double     outerDiameter,
								  G4double     containerLength,
								  G4Material*  outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("muspoiler"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateKicker(G4String     name,
							       G4double     length,
							       BDSBeamPipe* beamPipe,
							       G4double     outerDiameter,
							       G4double     containerLength,
							       G4bool       /*vertical*/,
							       G4Material*  outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // in this factory, h and v kickers will look the same so ignore bool vertical
  // have to retain it though for virtual base class compatability
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("hkicker"));
}

/// functions below here are private to this particular factory

void BDSMagnetOuterFactoryCylindrical::CreateCylindricalSolids(G4String     name,
							       G4double     length,
							       BDSBeamPipe* beamPipe,
							       G4double     magnetContainerLength,
							       G4double     outerDiameter)
{
  // clear up variables
  CleanUp();

  // build the container for the whole magnet object - this outer diameter should be
  // larger than the magnet outer piece diameter which is just 'outerDiameter' wide.
  G4double magnetContainerRadius = (0.5 * outerDiameter) + lengthSafety;
  BuildMagnetContainerSolidStraight(name, magnetContainerLength, magnetContainerRadius);
  
  if (beamPipe->ContainerIsCircular())
    {
      //circular beampipe so we can simply use its radius
      yokeSolid = new G4Tubs(name + "_yoke_solid",        // name
			     beamPipe->GetContainerRadius() + 2*lengthSafety, // inner radius
			     outerDiameter*0.5,           // outer radius
			     length*0.5-2*lengthSafety,   // half length
			     0,                           // rotation start angle
			     CLHEP::twopi);               // rotation finish angle

      //container is similar but slightly wider and hollow (to allow placement of beampipe)
      containerSolid = new G4Tubs(name + "_contiainer_solid",      // name
				  beamPipe->GetContainerRadius() + lengthSafety, // inner radius
				  outerDiameter*0.5 + lengthSafety,// outer radius
				  length*0.5,                      // half length
				  0,                               // rotation start angle
				  CLHEP::twopi);                   // rotation finish angle
    }
  else
    {
      G4VSolid* yokeSolidCylinder = new G4Tubs(name + "_yoke_solid_cylinder",// name
					       0,  // inner radius - for unambiguous subtraction
					       outerDiameter*0.5,            // outer radius
					       length*0.5-2*lengthSafety,    // half length
					       0,                            // rotation start angle
					       CLHEP::twopi);                // rotation finish angle
      allSolids.push_back(yokeSolidCylinder);
      yokeSolid = new G4SubtractionSolid(name + "_yoke_solid",
					 yokeSolidCylinder,
					 beamPipe->GetContainerSubtractionSolid());

      //container is similar but slightly wider
      G4VSolid* containerSolidCylinder = new G4Tubs(name + "_container_solid_cylinder",// name
						    0,  // inner radius - for unambiguous subtraction
						    outerDiameter*0.5 + lengthSafety,  // outer radius
						    length*0.5,                        // half length
						    0,                                 // rotation start angle
						    CLHEP::twopi);                     // rotation finish angle
      allSolids.push_back(containerSolidCylinder);
      containerSolid = new G4SubtractionSolid(name + "_container_solid",
					      containerSolidCylinder,
					      beamPipe->GetContainerSubtractionSolid());
    }
  allSolids.push_back(yokeSolid);
}

void BDSMagnetOuterFactoryCylindrical::TestInputParameters(BDSBeamPipe* beamPipe,
							   G4double&    outerDiameter,
							   G4Material*& outerMaterial)// reference to a pointer
{
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

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CommonFinalConstructor(G4String    name,
									 G4double    length,
									 G4double    outerDiameter,
									 G4Material* outerMaterial,
									 G4Colour*   colour)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CreateLogicalVolumes(name, length, colour, outerMaterial);
  CreateMagnetContainerComponent();

  // PLACEMENT
  // place the components inside the container
  // note we don't need the pointer for anything - it's registered upon construction with g4
  yokePV = new G4PVPlacement((G4RotationMatrix*)nullptr,   // no rotation
			     (G4ThreeVector)0,             // position
			     yokeLV,                       // lv to be placed
			     name + "_yoke_pv",            // name
			     containerLV,                  // mother lv to be place in
			     false,                        // no boolean operation
			     0,                            // copy number
			     checkOverlaps); // whether to check overlaps

  // record extents
  // container radius is the same for all methods as all cylindrical
  G4double containerRadius = outerDiameter + lengthSafety;
  std::pair<double,double> extX = std::make_pair(-containerRadius,containerRadius);
  std::pair<double,double> extY = std::make_pair(-containerRadius,containerRadius);
  std::pair<double,double> extZ = std::make_pair(-length*0.5,length*0.5);
  
  // build the BDSMagnetOuter instance and return it
  BDSMagnetOuter* outer = new BDSMagnetOuter(containerSolid,
					     containerLV,
					     extX, extY, extZ,
					     magnetContainer);
  
  // register all objects that go with the final geometry component (from internal vectors)
  outer->RegisterSolid(allSolids);
  outer->RegisterLogicalVolume(yokeLV);
  outer->RegisterSensitiveVolume(yokeLV);
  outer->RegisterPhysicalVolume(yokePV);
  outer->RegisterVisAttributes(outerVisAttributes);
  // no rotation matrices used in this factory
#ifdef NOUSERLIMITS
  RegisterUserLimit(outerUserLimits);
#endif
  return outer;
}


