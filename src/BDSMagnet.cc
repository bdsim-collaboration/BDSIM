#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"
#include "BDSMagnet.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4VPhysicalVolume.hh"

#include <cstdlib>
#include <cmath>
#include <string>


BDSMagnet::BDSMagnet(BDSMagnetType       type,
		     G4String            name,
		     G4double            length,
		     BDSBeamPipeInfo*    beamPipeInfoIn,
		     BDSMagnetOuterInfo* magnetOuterInfoIn,
		     BDSFieldInfo*       vacuumFieldInfoIn,
		     G4double            angle,
		     BDSFieldInfo*       outerFieldInfoIn):
  BDSAcceleratorComponent(name, length, angle, type.ToString()),
  magnetType(type),
  beamPipeInfo(beamPipeInfoIn),
  magnetOuterInfo(magnetOuterInfoIn),
  vacuumFieldInfo(vacuumFieldInfoIn),
  outerFieldInfo(outerFieldInfoIn),
  beampipe(nullptr),
  placeBeamPipe(false),
  magnetOuterOffset(G4ThreeVector(0,0,0)),
  outer(nullptr),
  vacuumField(nullptr),
  outerField(nullptr)
{
  outerDiameter   = magnetOuterInfo->outerDiameter;
  containerRadius = 0.5*outerDiameter;
  inputface       = G4ThreeVector(0,0,-1);
  outputface      = G4ThreeVector(0,0, 1);
  
  beampipe = nullptr;
  outer    = nullptr;

  placeBeamPipe = false;
}

void BDSMagnet::Build()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif  
  BuildBeampipe();
  BuildVacuumField();
  BuildOuter();
  BuildOuterField();
  BDSAcceleratorComponent::Build(); // build container
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
                                                    BDSGlobalConstants::Instance()->CheckOverlaps());
      
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
                                                       BDSGlobalConstants::Instance()->CheckOverlaps());

      RegisterPhysicalVolume(magnetOuterPV);
    }
}

BDSMagnet::~BDSMagnet()
{
  delete beamPipeInfo;
  delete magnetOuterInfo;  
  delete vacuumField;
  delete outerField;
  delete vacuumFieldInfo;
  delete outerFieldInfo;
}
