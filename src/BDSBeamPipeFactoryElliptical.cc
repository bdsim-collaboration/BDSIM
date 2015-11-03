#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryElliptical.hh"
#include "BDSBeamPipe.hh"

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"

#include "globals.hh"                 // geant4 globals / types
#include "G4Box.hh"
#include "G4EllipticalTube.hh"
#include "G4CutTubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"

#include <cmath>                           // sin, cos, fabs
#include <utility>                         // for std::pair


BDSBeamPipeFactoryElliptical* BDSBeamPipeFactoryElliptical::_instance = nullptr;

BDSBeamPipeFactoryElliptical* BDSBeamPipeFactoryElliptical::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSBeamPipeFactoryElliptical();}
  return _instance;
}

BDSBeamPipeFactoryElliptical::BDSBeamPipeFactoryElliptical()
{;}

BDSBeamPipeFactoryElliptical::~BDSBeamPipeFactoryElliptical()
{
  _instance = nullptr;
}

BDSBeamPipe* BDSBeamPipeFactoryElliptical::CreateBeamPipe(G4String    nameIn,              // name
							  G4double    lengthIn,            // length [mm]
							  G4double    aper1In,             // aperture parameter 1
							  G4double    aper2In,             // aperture parameter 2
							  G4double    /*aper3In*/,         // aperture parameter 3
							  G4double    /*aper4In*/,         // aperture parameter 4
							  G4Material* vacuumMaterialIn,    // vacuum material
							  G4double    beamPipeThicknessIn, // beampipe thickness [mm]
							  G4Material* beamPipeMaterialIn)  // beampipe material
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // clean up after last usage
  CleanUp();
  
  // build the solids
  vacuumSolid   = new G4EllipticalTube(nameIn + "_vacuum_solid",       // name
				       aper1In,                        // x half width
				       aper2In,                        // y half width
				       (lengthIn*0.5)-2*lengthSafety); // half length

  G4VSolid* beamPipeSolidInner; // construct rectangular beam pipe by subtracting an inner
  G4VSolid* beamPipeSolidOuter; // box from an outer one - only way
  // beamPipeSolidInner will be the inner edge of the metal beampipe
  // therefore it has to be the width of the aperture + lengthSafety
  beamPipeSolidInner = new G4EllipticalTube(nameIn + "_pipe_solid_inner",   // name
					    aper1In + lengthSafety,         // x half width - length safety to avoid overlaps
					    aper2In + lengthSafety,         // y half width
					    lengthIn);                      // length - full length fo unambiguous subtraction
  // beamPipeSolidOuter will be the outer edge of the metal beampipe
  // therefore it has to be the width of the aperture + beampipeThickness
  beamPipeSolidOuter = new G4EllipticalTube(nameIn + "_pipe_solid_outer",   // name
					    aper1In + beamPipeThicknessIn,  // x half width
					    aper2In + beamPipeThicknessIn,  // y half width
					    (lengthIn*0.5)-2*lengthSafety); // half length - lengthSafety to fit in container
  beamPipeSolid = new G4SubtractionSolid(nameIn + "_pipe_solid",
					 beamPipeSolidOuter,
					 beamPipeSolidInner); // outer minus inner
  
  G4double containerXHalfWidth = aper1In + beamPipeThicknessIn + lengthSafety;
  G4double containerYHalfWidth = aper2In + beamPipeThicknessIn + lengthSafety;
  containerSolid = new G4EllipticalTube(nameIn  + "_container_solid",  // name
					containerXHalfWidth,           // x half width
					containerYHalfWidth,           // y half width
					(lengthIn*0.5)-lengthSafety);  // half length
					
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, aper1In, aper2In, beamPipeThicknessIn);
}

BDSBeamPipe* BDSBeamPipeFactoryElliptical::CreateBeamPipeAngledInOut(G4String    nameIn,              // name
								     G4double    lengthIn,            // length [mm]
								     G4double    angleInIn,           // the normal angle of the input face
								     G4double    angleOutIn,          // the normal angle of the input face
								     G4double    aper1In,             // aperture parameter 1
								     G4double    aper2In,             // aperture parameter 2
								     G4double    /*aper3In*/,         // aperture parameter 3
								     G4double    /*aper4In */,        // aperture parameter 4
								     G4Material* vacuumMaterialIn,    // vacuum material
								     G4double    beamPipeThicknessIn, // beampipe thickness [mm]
								      G4Material* beamPipeMaterialIn   // beampipe material
								     )
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // clean up after last usage
  CleanUp();
  
  std::pair<G4ThreeVector,G4ThreeVector> faces = CalculateFaces(angleInIn, angleOutIn);
  G4ThreeVector inputface  = faces.first;
  G4ThreeVector outputface = faces.second;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, aper2In, beamPipeThicknessIn, inputface, outputface);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, aper1In, aper2In, beamPipeThicknessIn);
}

BDSBeamPipe* BDSBeamPipeFactoryElliptical::CommonFinalConstruction(G4String    nameIn,
								   G4Material* vacuumMaterialIn,
								   G4Material* beamPipeMaterialIn,
								   G4double    lengthIn,
								   G4double    aper1In,
								   G4double    aper2In,
								   G4double    beamPipeThicknessIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // prepare a longer container subtraction solid
  G4double containerXHalfWidth = aper1In + beamPipeThicknessIn + lengthSafety;
  G4double containerYHalfWidth = aper2In + beamPipeThicknessIn + lengthSafety;
  // doesn't have to be angled as it's only used for transverse subtraction
  containerSubtractionSolid = new G4EllipticalTube(nameIn  + "_container_solid", // name
						   containerXHalfWidth,          // x half width
						   containerYHalfWidth,          // y half width
						   lengthIn);                    // full length for unambiguous subtraction


  BDSBeamPipeFactoryBase::CommonConstruction(nameIn,
					     vacuumMaterialIn,
					     beamPipeMaterialIn,
					     lengthIn);

  // record extents
  std::pair<double,double> extX = std::make_pair(-containerXHalfWidth,containerXHalfWidth);
  std::pair<double,double> extY = std::make_pair(-containerYHalfWidth,containerYHalfWidth);
  std::pair<double,double> extZ = std::make_pair(-lengthIn*0.5,lengthIn*0.5);
  // calculate radius if a tube were to be place around it
  G4double containerRadius = std::max(containerXHalfWidth, containerYHalfWidth);
  
  BDSBeamPipe* aPipe = BuildBeamPipeAndRegisterVolumes(extX,extY,extZ,containerRadius);
  
  return aPipe;
}


/// the angled ones have degeneracy in the geant4 solids they used so we can avoid code duplication
/// by grouping common construction tasks
void BDSBeamPipeFactoryElliptical::CreateGeneralAngledSolids(G4String      nameIn,
							     G4double      lengthIn,
							     G4double      aper1In,
							     G4double      aper2In,
							     G4double      beamPipeThicknessIn,
							     G4ThreeVector inputfaceIn,
							     G4ThreeVector outputfaceIn)
{
  // this function will make a longer normal rectangular beampipe and chop it off
  // to make angled faces as required
  // achieve this using the intersection of the normal beampipe (but a little longer)
  // with a large G4CutTubs to get the angled faces.
  // note even if one face is flat, we don't save a boolean operation as the intersection
  // can be on both sides using a G4CutTubs.  Also, keeping one side flat would require
  // shifting the volume from 0 which causes headaches later with SDs.

  // build the solids - vacuum, beampipe and container solids
  // extra solids required for booleans
  G4VSolid* vacuumSolidLong;
  G4VSolid* beamPipeSolidLong;
  G4VSolid* angledFaceSolid;
  G4VSolid* containerSolidLong;
  G4VSolid* angledFaceSolidContainer;

  // build the solid with angled faces for intersection
  G4double angledFaceRadius = (std::max(aper1In,aper2In) + beamPipeThicknessIn)*2.0; //huge for unambiguous intersection
  angledFaceSolid = new G4CutTubs(nameIn + "_angled_face",       // name
				  0,                             // inner radius
				  angledFaceRadius,              // outer radius
				  (lengthIn*0.5)-2*lengthSafety, // half length - must fit within container
				  0,                             // rotation start angle
				  CLHEP::twopi,                  // rotation finish angle
				  inputfaceIn,                   // input face normal
				  outputfaceIn);                 // output face normal
  
  vacuumSolidLong = new G4EllipticalTube(nameIn + "_vacuum_solid_long", // name
					 aper1In,                       // x half width
					 aper2In,                       // y half width
					 lengthIn);                     // full length for unambiguous boolean
  vacuumSolid     = new G4IntersectionSolid(nameIn + "_vacuum_solid",
					    vacuumSolidLong,
					    angledFaceSolid);

  allSolids.push_back(angledFaceSolid);
  allSolids.push_back(vacuumSolidLong);
  
  G4VSolid* beamPipeSolidInner; // construct rectangular beam pipe by subtracting an inner
  G4VSolid* beamPipeSolidOuter; // box from an outer one - only way
  // beamPipeSolidInner will be the inner edge of the metal beampipe
  // therefore it has to be the width of the aperture + lengthSafety
  beamPipeSolidInner = new G4EllipticalTube(nameIn + "_pipe_solid_inner",   // name
					    aper1In + lengthSafety,         // x half width - length safety to avoid overlaps
					    aper2In + lengthSafety,         // y half width
					    2*lengthIn);                    // 2*length - full length fo unambiguous subtraction
  // beamPipeSolidOuter will be the outer edge of the metal beampipe
  // therefore it has to be the width of the aperture + beampipeThickness
  beamPipeSolidOuter = new G4EllipticalTube(nameIn + "_pipe_solid_outer",   // name
					    aper1In + beamPipeThicknessIn,  // x half width
					    aper2In + beamPipeThicknessIn,  // y half width
					    lengthIn);                      // full length for unambiguous intersection
  beamPipeSolidLong = new G4SubtractionSolid(nameIn + "_pipe_solid_long",
					 beamPipeSolidOuter,
					 beamPipeSolidInner); // outer minus inner
  allSolids.push_back(beamPipeSolidInner);
  allSolids.push_back(beamPipeSolidOuter);
  allSolids.push_back(beamPipeSolidLong);
  
  beamPipeSolid = new G4IntersectionSolid(nameIn + "_pipe_solid",
					  beamPipeSolidLong,
					  angledFaceSolid);
  
  G4double containerXHalfWidth = aper1In + beamPipeThicknessIn + lengthSafety;
  G4double containerYHalfWidth = aper2In + beamPipeThicknessIn + lengthSafety;
  containerSolidLong = new G4EllipticalTube(nameIn  + "_container_solid_long",// name
					    containerXHalfWidth,              // x half width
					    containerYHalfWidth,              // y half width
					    lengthIn);                        // full length for unambiguous intersection
  angledFaceSolidContainer = new G4CutTubs(nameIn + "_angled_face_container",// name
					   0,                                // inner radius
					   angledFaceRadius,                 // outer radius
					   (lengthIn*0.5)-lengthSafety,      // half length - must fit within magnet
					   0,                                // rotation start angle
					   CLHEP::twopi,                     // rotation finish angle
					   inputfaceIn,                      // input face normal
					   outputfaceIn);                    // output face normal
  allSolids.push_back(containerSolidLong);
  allSolids.push_back(angledFaceSolidContainer);
  
  containerSolid = new G4IntersectionSolid(nameIn + "_container_solid",
					   containerSolidLong,
					   angledFaceSolidContainer);
}
