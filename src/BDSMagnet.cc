#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"
#include "BDSMagnet.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"

#include <cstdlib>
#include <cmath>
#include <string>


BDSMagnet::BDSMagnet(BDSMagnetType       type,
		     G4String            name,
		     G4double            length,
		     BDSBeamPipeInfo*    beamPipeInfo,
		     BDSMagnetOuterInfo* magnetOuterInfoIn,
		     BDSFieldInfo*       vacuumFieldInfoIn,
		     G4double            angle,
		     BDSFieldInfo*       outerFieldInfoIn):
  BDSAcceleratorComponent(name, length, angle, type.ToString(), beamPipeInfo),
  magnetType(type),
  magnetOuterInfo(magnetOuterInfoIn),
  vacuumFieldInfo(vacuumFieldInfoIn),
  outerFieldInfo(outerFieldInfoIn),
  beampipe(nullptr),
  placeBeamPipe(false),
  magnetOuterOffset(G4ThreeVector(0,0,0)),
  outer(nullptr),
  beamPipePlacementTransform(G4Transform3D())
{
  outerDiameter   = magnetOuterInfo->outerDiameter;
  containerRadius = 0.5*outerDiameter;
  inputface       = G4ThreeVector(0,0,-1);
  outputface      = G4ThreeVector(0,0, 1);
  
  beampipe = nullptr;
  outer    = nullptr;

  placeBeamPipe = false;

  // It's not possible to build advanced outer geometry for a very thin magnet.
  if (length < 1e-4*CLHEP::m) // 100um minimum length for geometry
    {magnetOuterInfo->geometryType = BDSMagnetGeometryType::none;}
}

void BDSMagnet::Build()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif  
  BuildBeampipe();
  BuildVacuumField();
  BuildOuter();
  BDSAcceleratorComponent::Build(); // build container
  BuildOuterField(); // must be done when the containerLV exists
  PlaceComponents(); // place things (if needed) in container
}

void BDSMagnet::BuildBeampipe()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  beampipe = BDSBeamPipeFactory::Instance()->CreateBeamPipe(name,
							    chordLength - lengthSafety,
							    beamPipeInfo);

  beamPipePlacementTransform = beampipe->GetPlacementTransform().inverse();
  
  RegisterDaughter(beampipe);
  InheritExtents(beampipe);

  SetAcceleratorVacuumLogicalVolume(beampipe->GetVacuumLogicalVolume());

  /// Update record of normal vectors now beam pipe has been constructed.
  SetInputFaceNormal(BDS::RotateToReferenceFrame(beampipe->InputFaceNormal(), angle));
  SetOutputFaceNormal(BDS::RotateToReferenceFrame(beampipe->OutputFaceNormal(), -angle));
}

void BDSMagnet::BuildVacuumField()
{
  if (vacuumFieldInfo)
    {
      G4Transform3D newFieldTransform = vacuumFieldInfo->Transform() * beamPipePlacementTransform;
      vacuumFieldInfo->SetTransform(newFieldTransform);
      BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumFieldInfo,
								beampipe->GetVacuumLogicalVolume(),
								true);
    }
}

void BDSMagnet::BuildOuter()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<  *magnetOuterInfo << G4endl;
#endif
  G4double outerLength = chordLength - 2*lengthSafety;
  outer = BDSMagnetOuterFactory::Instance()->CreateMagnetOuter(magnetType,
							       magnetOuterInfo,
							       outerLength,
							       chordLength,
							       beampipe);

  if(outer)
    {
      // copy necessary bits out of BDSGeometryComponent that holds
      // container information for whole magnet object provided by
      // magnet outer factory.
      BDSGeometryComponent* container = outer->GetMagnetContainer();
      containerSolid    = container->GetContainerSolid()->Clone();
      G4ThreeVector contOffset = container->GetPlacementOffset();
      // set the main offset of the whole magnet which is placed w.r.t. the
      // zero coordinate of the container solid
      SetPlacementOffset(contOffset);

      RegisterDaughter(outer);
      InheritExtents(container); // update extents

      // Only clear after extents etc have been used
      outer->ClearMagnetContainer();
      
      endPieceBefore = outer->EndPieceBefore();
      endPieceAfter  = outer->EndPieceAfter();

      /// Update record of normal vectors now beam pipe has been constructed.
      SetInputFaceNormal(BDS::RotateToReferenceFrame(outer->InputFaceNormal(), angle));
      SetOutputFaceNormal(BDS::RotateToReferenceFrame(outer->OutputFaceNormal(), -angle));
    }
}

void BDSMagnet::BuildOuterField()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif  
  if (outer && outerFieldInfo)
    {
      G4LogicalVolume* vol = outer->GetContainerLogicalVolume();
      BDSFieldBuilder::Instance()->RegisterFieldForConstruction(outerFieldInfo,
								vol,
								true);
      // Attach to the container but don't propagate to daughter volumes. This ensures
      // any gap between the beam pipe and the outer also has a field.
      BDSFieldBuilder::Instance()->RegisterFieldForConstruction(outerFieldInfo,
								containerLogicalVolume,
								false);
    }
}

void BDSMagnet::BuildContainerLogicalVolume()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // note beam pipe is not optional!
  if (outer)
    {//build around that
      // container solid will have been updated in BuildOuter if the outer exists
      containerLogicalVolume = new G4LogicalVolume(containerSolid,
						   emptyMaterial,
						   name + "_container_lv");
      placeBeamPipe = true;
    }
  else
    {
      // use beam pipe container volume as ours and no need to place beam pipe
      containerSolid         = beampipe->GetContainerSolid();
      containerLogicalVolume = beampipe->GetContainerLogicalVolume();
      InheritExtents(beampipe);
      placeBeamPipe = false;
    }

  // now protect the fields inside the container volume by giving the
  // it a null magnetic field (otherwise G4VPlacement can
  // over-ride the already-created fields, by calling 
  // G4LogicalVolume::AddDaughter, which calls 
  // pDaughterLogical->SetFieldManager(fFieldManager, true) - the
  // latter 'true' over-writes all the other fields
  // This shouldn't override the field attached to daughters (vacuum for example) which will
  // retain their field manager if one is already specified.
  containerLogicalVolume->SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false); 
}

void BDSMagnet::PlaceComponents()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  if (placeBeamPipe)
    {
      G4ThreeVector beamPipeOffset = -1*GetPlacementOffset();
      // place beampipe
      G4PVPlacement* beamPipePV = new G4PVPlacement(nullptr,                 // rotation
						    beamPipeOffset,          // position in container
						    beampipe->GetContainerLogicalVolume(),  // its logical volume
						    name + "_beampipe_pv",   // its name
						    containerLogicalVolume,  // its mother  volume
						    false,                   // no boolean operation
						    0,                       // copy number
                                                    checkOverlaps);
      
      RegisterPhysicalVolume(beamPipePV);
    }

  if (outer)
    {
      //G4ThreeVector placementOffset = magnetOuterOffset + outer->GetPlacementOffset();
      G4ThreeVector outerOffset = outer->GetPlacementOffset();
      
      // place outer volume
      G4PVPlacement* magnetOuterPV = new G4PVPlacement(nullptr,                // rotation
						       outerOffset,            // at normally (0,0,0)
						       outer->GetContainerLogicalVolume(), // its logical volume
						       name+"_outer_pv",       // its name
						       containerLogicalVolume, // its mother  volume
						       false,                  // no boolean operation
						       0,                      // copy number
                                                       checkOverlaps);

      RegisterPhysicalVolume(magnetOuterPV);
    }
}

void BDSMagnet::SetOuterField(BDSFieldInfo* outerFieldInfoIn)
{
  delete outerFieldInfo;
  outerFieldInfo = outerFieldInfoIn;
}

void BDSMagnet::SetVacuumField(BDSFieldInfo* vacuumFieldInfoIn)
{
  delete vacuumFieldInfo;
  vacuumFieldInfo = vacuumFieldInfoIn;
}

BDSMagnet::~BDSMagnet()
{
  delete magnetOuterInfo;
  delete vacuumFieldInfo;
  delete outerFieldInfo;
}
