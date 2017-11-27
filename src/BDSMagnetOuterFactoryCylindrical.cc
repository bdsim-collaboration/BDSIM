/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSMagnetOuterFactoryCylindrical.hh"

#include "BDSBeamPipe.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"                 // for calculateorientation

#include "globals.hh"                      // geant4 globals / types
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
								   G4double     angleIn,
								   G4double     angleOut,
								   G4bool       /*yokeOnLeft*/,
								   G4bool       /*hStyle*/,
								   G4Material*  outerMaterial,
								   G4bool       /*buildEndPiece*/,
								   G4double     /*vhRatio*/,
								   G4double     /*coilWidthFraction*/,
								   G4double     /*coilHeightFraction*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // clear up variables
  CleanUp();
  
  // test input parameters - set global options as default if not specified
  TestInputParameters(beamPipe,outerDiameter,outerMaterial);
    
  // Simple cylinder if no poleface rotation, otherwise angled.
  if (!BDS::IsFinite(angleIn) && !BDS::IsFinite(angleOut))
    {
      CreateCylindricalSolids(name,length, beamPipe, containerLength, outerDiameter);
      G4double magnetContainerRadius = (0.5 * outerDiameter) + lengthSafety;
      BuildMagnetContainerSolidStraight(name, containerLength, magnetContainerRadius);
    }
  else
    {
      std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(angleIn,angleOut);
      inputFaceNormal = faces.first;
      outputFaceNormal = faces.second;

      CreateCylindricalSolidsAngled(name, length, beamPipe, containerLength, outerDiameter);
    
      // build the container for the whole magnet object - this outer diameter should be
      // larger than the magnet outer piece diameter which is just 'outerDiameter' wide.
      G4double magnetContainerRadius = (0.5 * outerDiameter) + lengthSafety;
      BuildMagnetContainerSolidAngled(name, containerLength, magnetContainerRadius);
    }
  return CommonFinalConstructor(name,length,outerDiameter,outerMaterial,
				BDSColours::Instance()->GetColour("sectorbend"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateRectangularBend(G4String     name,
									G4double     length,
									BDSBeamPipe* beamPipe,
									G4double     outerDiameter,
									G4double     containerLength,
									G4double     angleIn,
									G4double     angleOut,
									G4bool       /*yokeOnLeft*/,
									G4bool       /*hStyle*/,
									G4Material*  outerMaterial,
									G4bool       /*buildEndPiece*/,
									G4double     /*vhRatio*/,
									G4double     /*coilWidthFraction*/,
									G4double     /*coilHeightFraction*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif  
  //rectangular bends currently just make a shorter straight volume, so ignore angle for now

  // clear up variables
  CleanUp();
  
  // test input parameters - set global options as default if not specified
  TestInputParameters(beamPipe,outerDiameter,outerMaterial);
    
  // Simple cylinder if no poleface rotation, otherwise angled.
  if ((!BDS::IsFinite(angleIn)) && !BDS::IsFinite(angleOut))
    {
      CreateCylindricalSolids(name,length, beamPipe, containerLength, outerDiameter);
      BuildMagnetContainerSolidStraight(name,containerLength,0.5*outerDiameter);
    }
  else
    {
      std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(angleIn,angleOut);
      inputFaceNormal = faces.first;
      outputFaceNormal = faces.second;

      CreateCylindricalSolidsAngled(name, length, beamPipe, containerLength, outerDiameter);
      BuildMagnetContainerSolidAngled(name, containerLength, 0.5*outerDiameter);
    }

  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("rectangularbend"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateQuadrupole(G4String     name,
								   G4double     length,
								   BDSBeamPipe* beamPipe,
								   G4double     outerDiameter,
								   G4double     containerLength,
								   G4Material*  outerMaterial,
								   G4bool       /*buildEndPiece*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CleanUp();
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("quadrupole"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateSextupole(G4String     name,
								  G4double     length,
								  BDSBeamPipe* beamPipe,
								  G4double     outerDiameter,
								  G4double     containerLength,
								  G4Material*  outerMaterial,
								  G4bool       /*buildEndPiece*/)
{
  CleanUp();
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("sextupole"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateOctupole(G4String     name,
								 G4double     length,
								 BDSBeamPipe* beamPipe,
								 G4double     outerDiameter,
								 G4double     containerLength,
								 G4Material*  outerMaterial,
								 G4bool       /*buildEndPiece*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CleanUp();
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("octupole"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateDecapole(G4String     name,
								 G4double     length,
								 BDSBeamPipe* beamPipe,
								 G4double     outerDiameter,
								 G4double     containerLength,
								 G4Material*  outerMaterial,
								 G4bool       /*buildEndPiece*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CleanUp();
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("decapole"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateSolenoid(G4String     name,
								 G4double     length,
								 BDSBeamPipe* beamPipe,
								 G4double     outerDiameter,
								 G4double     containerLength,
								 G4Material*  outerMaterial,
								 G4bool       /*buildEndPiece*/)
{
  CleanUp();
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("solenoid"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateMultipole(G4String     name,
								  G4double     length,
								  BDSBeamPipe* beamPipe,
								  G4double     outerDiameter,
								  G4double     containerLength,
								  G4Material*  outerMaterial,
								  G4bool       /*buildEndPiece*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CleanUp();
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("multipole"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateRfCavity(G4String     name,
								 G4double     length,
								 BDSBeamPipe* beamPipe,
								 G4double     outerDiameter,
								 G4double     containerLength,
								 G4Material*  outerMaterial,
								 G4bool       /*buildEndPiece*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CleanUp();
  CreateCylindricalSolids(name, length, beamPipe, containerLength, outerDiameter);
  return CommonFinalConstructor(name, length, outerDiameter, outerMaterial,
				BDSColours::Instance()->GetColour("rfcavity"));
}

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CreateMuSpoiler(G4String     name,
								  G4double     length,
								  BDSBeamPipe* beamPipe,
								  G4double     outerDiameter,
								  G4double     containerLength,
								  G4Material*  outerMaterial,
								  G4bool       /*buildEndPiece*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CleanUp();
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
							       G4Material*  outerMaterial,
							       G4bool       /*buildEndPiece*/,
							       G4bool       /*hStyle*/,
							       G4double     /*vhRatio*/,
							       G4double     /*coilWidthFraction*/,
							       G4double     /*coilHeightFraction*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CleanUp();
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
      containerSolid = new G4Tubs(name + "_container_solid",      // name
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

// Function for cylinder with angled faces - for pole face rotation in dipoles.
void BDSMagnetOuterFactoryCylindrical::CreateCylindricalSolidsAngled(G4String     name,
								     G4double     length,
								     BDSBeamPipe* beamPipe,
								     G4double     magnetContainerLength,
								     G4double     outerDiameter)
{ 
  // build the container for the whole magnet object - this outer diameter should be
  // larger than the magnet outer piece diameter which is just 'outerDiameter' wide.
  G4double magnetContainerRadius = (0.5 * outerDiameter) + lengthSafety;
  BuildMagnetContainerSolidStraight(name, magnetContainerLength, magnetContainerRadius);
  
  if (beamPipe->ContainerIsCircular())
    {
      //circular beampipe so we can simply use its radius
      yokeSolid = new G4CutTubs(name + "_yoke_solid",        // name
			     beamPipe->GetContainerRadius() + 2*lengthSafety, // inner radius
			     outerDiameter*0.5,           // outer radius
			     length*0.5-2*lengthSafety,   // half length
			     0,                           // rotation start angle
			     CLHEP::twopi,                // rotation finish angle
			     inputFaceNormal,             // input face normal
			     outputFaceNormal);           // output face normal);

      //container is similar but slightly wider and hollow (to allow placement of beampipe)
      containerSolid = new G4CutTubs(name + "_container_solid",    // name
				  beamPipe->GetContainerRadius() + lengthSafety, // inner radius
				  outerDiameter*0.5 + lengthSafety,// outer radius
				  length*0.5,         // half length
				  0,                  // rotation start angle
				  CLHEP::twopi,       // rotation finish angle
				  inputFaceNormal,    // input face normal
				  outputFaceNormal);  // output face normal);
    }
  else
    {
      G4VSolid* yokeSolidCylinder = new G4CutTubs(name + "_yoke_solid_cylinder",// name
						  0,  // inner radius - for unambiguous subtraction
						  outerDiameter*0.5,        // outer radius
						  length*0.5-2*lengthSafety,// half length
						  0,                        // rotation start angle
						  CLHEP::twopi,             // rotation finish angle
						  inputFaceNormal,          // input face normal
						  outputFaceNormal);              // output face normal);
      allSolids.push_back(yokeSolidCylinder);
      yokeSolid = new G4SubtractionSolid(name + "_yoke_solid",
					 yokeSolidCylinder,
					 beamPipe->GetContainerSubtractionSolid());

      //container is similar but slightly wider
      G4VSolid* containerSolidCylinder = new G4CutTubs(name + "_container_solid_cylinder",// name
						       0,  // inner radius - for unambiguous subtraction
						       outerDiameter*0.5 + lengthSafety,// outer radius
						       length*0.5,       // half length
						       0,                // rotation start angle
						       CLHEP::twopi,     // rotation finish angle
						       inputFaceNormal,  // input face normal
						       outputFaceNormal);// output face normal);
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

BDSMagnetOuter* BDSMagnetOuterFactoryCylindrical::CommonFinalConstructor(G4String    name,
									 G4double    length,
									 G4double    outerDiameter,
									 G4Material* outerMaterial,
									 G4Colour*   colour)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CreateLogicalVolumes(name, colour, outerMaterial);
  CreateMagnetContainerComponent();

  // PLACEMENT
  // place the components inside the container
  // note we don't need the pointer for anything - it's registered upon construction with g4
  yokePV = new G4PVPlacement((G4RotationMatrix*)nullptr,   // no rotation
			     (G4ThreeVector)0,             // position
			     yokeLV,                       // lv to be placed
			     name + "_yoke_pv",            // name
			     containerLV,                  // mother lv to be placed in
			     false,                        // no boolean operation
			     0,                            // copy number
			     checkOverlaps); // whether to check overlaps

  // record extents
  // container radius is the same for all methods as all cylindrical
  G4double containerRadius = outerDiameter + lengthSafety;
  BDSExtent ext = BDSExtent(containerRadius, containerRadius, length*0.5);
  
  // build the BDSMagnetOuter instance and return it
  BDSMagnetOuter* outer = new BDSMagnetOuter(containerSolid,
					     containerLV, ext,
					     magnetContainer);

  SetFaceNormals(outer);
  
  // register all objects that go with the final geometry component (from internal vectors)
  outer->RegisterSolid(allSolids);
  outer->RegisterLogicalVolume(yokeLV);
  outer->RegisterSensitiveVolume(yokeLV);
  outer->RegisterPhysicalVolume(yokePV);
  outer->RegisterVisAttributes(outerVisAttributes);
  // no rotation matrices used in this factory
  return outer;
}


