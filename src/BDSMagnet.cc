/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBeamPipeType.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"
#include "BDSIntegratorType.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSGeometryExternal.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetOuterFactoryLHC.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"
#include "BDSMagnet.hh"
#include "BDSUtilities.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Types.hh"
#include "G4VPhysicalVolume.hh"

#include "CLHEP/Units/SystemOfUnits.h"

class G4Userlimits;

BDSMagnet::BDSMagnet(BDSMagnetType       typeIn,
		     const G4String&     nameIn,
		     G4double            lengthIn,
		     BDSBeamPipeInfo*    beamPipeInfoIn,
		     BDSMagnetOuterInfo* magnetOuterInfoIn,
		     BDSFieldInfo*       vacuumFieldInfoIn,
		     G4double            angleIn,
		     BDSFieldInfo*       outerFieldInfoIn,
		     G4bool              isThinIn):
  BDSAcceleratorComponent(nameIn, lengthIn, angleIn, typeIn.ToString(), beamPipeInfoIn),
  magnetType(typeIn),
  magnetOuterInfo(magnetOuterInfoIn),
  vacuumFieldInfo(vacuumFieldInfoIn),
  outerFieldInfo(outerFieldInfoIn),
  beampipe(nullptr),
  placeBeamPipe(false),
  magnetOuterOffset(G4ThreeVector(0,0,0)),
  outer(nullptr),
  beamPipePlacementTransform(G4Transform3D()),
  isThin(isThinIn)
{
  magnetOuterInfo->name += "_outer";
  horizontalWidth = magnetOuterInfoIn->horizontalWidth;
  containerRadius = 0.5*horizontalWidth;
  
  beampipe = nullptr;
  outer    = nullptr;

  placeBeamPipe = false;

  // It's not possible to build advanced outer geometry for a very thin magnet.
  if (lengthIn < 1e-4*CLHEP::m) // 100um minimum length for geometry
    {magnetOuterInfo->geometryType = BDSMagnetGeometryType::none;}
  // No beam pipe geometry for really short 'magnets'
  if (lengthIn < 1e-6*CLHEP::m)
    {
      GetBeamPipeInfo()->beamPipeType = BDSBeamPipeType::circularvacuum;
      isThin = true;
    }
}

G4String BDSMagnet::DetermineScalingKey(BDSMagnetType typeIn)
{
  G4String result = "none";
  switch (typeIn.underlying())
    {
    case BDSMagnetType::hkicker:
    case BDSMagnetType::vkicker:
    case BDSMagnetType::muonspoiler:
    case BDSMagnetType::sectorbend:
    case BDSMagnetType::rectangularbend:
      {result = "field"; break;}
    case BDSMagnetType::quadrupole:
      {result = "k1"; break;}
    case BDSMagnetType::sextupole:
      {result = "k2"; break;}
    case BDSMagnetType::octupole:
      {result = "k3"; break;}
    case BDSMagnetType::decapole:
      {result = "k4"; break;}
    default:
      {break;} // leave as none without complaint
    };

  return result;
}

void BDSMagnet::SetInputFaceNormal(const G4ThreeVector& input)
{
  if (outer)
    {outer->SetInputFaceNormal(input);}
}

void BDSMagnet::SetOutputFaceNormal(const G4ThreeVector& output)
{
  if (outer)
    {outer->SetOutputFaceNormal(output);}
}

G4String BDSMagnet::Material() const
{
  if (magnetOuterInfo)
    {return magnetOuterInfo->outerMaterial->GetName();}
  else
    {return BDSAcceleratorComponent::Material();}
}

void BDSMagnet::Build()
{
  BuildUserLimits();
  BuildBeampipe();
  BuildVacuumField();
  BuildOuter();
  // Instead of BDSAcceleratorComponent::Build just call BuildContainerLogicalVolume
  // to control user limits in the case where there is no container and we just inherit
  // the beam pipe container
  BuildContainerLogicalVolume();
  BuildOuterField(); // must be done when the containerLV exists
  PlaceComponents(); // place things (if needed) in container
}

void BDSMagnet::BuildBeampipe()
{
  beampipe = BDSBeamPipeFactory::Instance()->CreateBeamPipe(name+"_bp",
							    chordLength - 2*lengthSafety,
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
      if (beamPipePlacementTransform != G4Transform3D::Identity)
        {
          G4Transform3D newFieldTransform = vacuumFieldInfo->Transform() * beamPipePlacementTransform;
          vacuumFieldInfo->SetTransform(newFieldTransform);
        }
      // can use containerLV for field as we don't construct any geometry with thin elements.
      if (isThin)
        {
          BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumFieldInfo,
                                                                    beampipe->GetContainerLogicalVolume(),
                                                                    true);
        }
      else
        {
          BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumFieldInfo,
                                                                    beampipe->GetVolumesForField(),
                                                                    true);
        }
    }
}

void BDSMagnet::BuildOuter()
{
  G4double outerLength = chordLength - 2*lengthSafety;
  // The outerFieldInfo is required only so we don't reuse geometry when we need a unique field on it.
  outer = BDSMagnetOuterFactory::Instance()->CreateMagnetOuter(magnetType,
                                                               magnetOuterInfo,
                                                               outerLength,
                                                               chordLength,
                                                               beampipe);

  if (outer)
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
      InheritExtents(container, contOffset); // update extents

      endPieceBefore = outer->EndPieceBefore();
      endPieceAfter  = outer->EndPieceAfter();

      /// Update record of normal vectors now beam pipe has been constructed.
      SetInputFaceNormal(BDS::RotateToReferenceFrame(outer->InputFaceNormal(), angle));
      SetOutputFaceNormal(BDS::RotateToReferenceFrame(outer->OutputFaceNormal(), -angle));
    }
}

void BDSMagnet::BuildOuterField()
{
  if (outer && outerFieldInfo)
    {
      // determine key for this specific magnet instance
      G4String scalingKey = DetermineScalingKey(magnetType);
      
      BDSMagnetStrength* scalingStrength = vacuumFieldInfo ? vacuumFieldInfo->MagnetStrength() : nullptr;
      G4LogicalVolume* vol = outer->GetContainerLogicalVolume();
      BDSFieldBuilder::Instance()->RegisterFieldForConstruction(outerFieldInfo,
                                                                vol,
                                                                true,
                                                                scalingStrength,
                                                                scalingKey);
      // Attach to the container but don't propagate to daughter volumes. This ensures
      // any gap between the beam pipe and the outer also has a field.
      BDSFieldBuilder::Instance()->RegisterFieldForConstruction(outerFieldInfo,
                                                                containerLogicalVolume,
                                                                false,
                                                                scalingStrength,
                                                                scalingKey);
      
      // in the case of LHC-style geometry, override the second beam pipe (which is a daughter of the outer)
      // field to be the opposite sign of the main vacuum field (same strength)
      auto mgt = magnetOuterInfo->geometryType;
      if (mgt == BDSMagnetGeometryType::lhcleft || mgt == BDSMagnetGeometryType::lhcright)
        {
          std::set<BDSGeometryComponent*> daughtersSet = outer->GetAllDaughters();
          std::vector<BDSGeometryComponent*> daughters(daughtersSet.begin(), daughtersSet.end());
          if (daughters.size() == 1 && vacuumFieldInfo)
            {
              BDSFieldInfo* secondBPField = new BDSFieldInfo(*vacuumFieldInfo);
              G4double sign = mgt == BDSMagnetGeometryType::lhcleft ? 1.0 : -1.0;
              secondBPField->Translate(G4ThreeVector(sign * BDSMagnetOuterFactoryLHC::beamSeparation, 0, 0));
              (*(secondBPField->MagnetStrength()))["field"] *= -1; // flip the sign
              if (BDS::IsFinite((*(secondBPField->MagnetStrength()))["k1"]))
                {(*(secondBPField->MagnetStrength()))["k1"] *= -1;}
              secondBPField->SetIntegratorType(BDSIntegratorType::g4classicalrk4);
              BDSFieldBuilder::Instance()->RegisterFieldForConstruction(secondBPField,
                                                                        daughters[0]->GetContainerLogicalVolume(),
                                                                        true);
            }
        }
    }
}

void BDSMagnet::BuildContainerLogicalVolume()
{
  // note beam pipe is not optional!
  if (outer)
    {//build around that
      // container solid will have been updated in BuildOuter if the outer exists
      containerLogicalVolume = outer->GetMagnetContainer()->GetContainerLogicalVolume();
      outer->ClearMagnetContainer();
      // user limits - provided by BDSAcceleratorComponent
      containerLogicalVolume->SetUserLimits(userLimits);
      containerLogicalVolume->SetVisAttributes(containerVisAttr);
      
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
}

void BDSMagnet::PlaceComponents()
{
  if (placeBeamPipe && !(magnetOuterInfo->extractOuterContainer) && (beamPipeInfo->beamPipeType != BDSBeamPipeType::none))
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

  if (outer && !(magnetOuterInfo->extractOuterContainer))
    {
      // place the outer logical volume into the container logical volume
      if (magnetOuterInfo->geometryType != BDSMagnetGeometryType::external or magnetOuterInfo->includeGdmlWorldVolume)
        {
          G4ThreeVector outerOffset = outer->GetPlacementOffset();
          // place outer volume
          G4PVPlacement* magnetOuterPV = new G4PVPlacement(nullptr,                            // rotation
                                                           outerOffset,                        // at normally (0,0,0)
                                                           outer->GetContainerLogicalVolume(), // its logical volume
                                                           name+"_outer_pv",                   // its name
                                                           containerLogicalVolume,             // its mother  volume
                                                           false,                              // no boolean operation
                                                           0,                                  // copy number
                                                           false);
          
          if (magnetOuterPV->CheckOverlaps() && checkOverlaps)
            {throw BDSException(__METHOD_NAME__, "Overlapping detected for the outer elements");}
          RegisterPhysicalVolume(magnetOuterPV);
        }
      
      // place the elements inside the external outer logical volume (world volume) inside the container logical volume
      if (magnetOuterInfo->geometryType == BDSMagnetGeometryType::external)
        {
          auto gdmlContainer = outer->GetContainerLogicalVolume();
          for (G4int j = 0; j < (G4int)gdmlContainer->GetNoDaughters(); j++)
            {
              const auto& pv = gdmlContainer->GetDaughter(j);
              G4String placementName = pv->GetName() + "_pv";
              G4int copyNumberLocal = 1;
              
              if (!magnetOuterInfo->includeGdmlWorldVolume)
                {
                  auto vv = new G4PVPlacement(pv->GetRotation(),
                                              pv->GetTranslation(),
                                              pv->GetLogicalVolume(),                   // volume to be placed
                                              placementName,                            // placement name
                                              containerLogicalVolume,                   // volume to place it in
                                              false,                                    // no boolean operation
                                              copyNumberLocal,                          // copy number
                                              false);                                   // overlap checking
                  
                  if (vv->CheckOverlaps() && checkOverlaps)
                    {throw BDSException(__METHOD_NAME__, "Overlapping detected for the outer elements");}
                  
                  RegisterPhysicalVolume(vv);
                }
            }
          
          // place the outer field inside the GDML logical volumes and the vacuum field inside the GDML logical volumes defined by namedVacuumVolumes
          BDSGeometryExternal* outerGDML = outer->ExternalGeometry();
          std::set<G4LogicalVolume*> vacuumVols;
          std::set<G4LogicalVolume*> Vols;
          
          if (outerGDML) // the dynamic cast will only work if it's loaded as GDML//
            {
              Vols = outerGDML->GetAllLogicalVolumes();
              
              outerFieldInfo->SetScalingRadius(beamPipeInfo->aper1 + 2.5);
              outerFieldInfo->SetBeamPipeRadius(beamPipeInfo->aper1 + 2.5);
              
              BDSFieldBuilder::Instance()->RegisterFieldForConstruction(outerFieldInfo,
                                                                        Vols,
                                                                        true);
              
              vacuumVols = outerGDML->VacuumVolumes();
              BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumFieldInfo,
                                                                        vacuumVols,
                                                                        true);
            }
        }
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

void BDSMagnet::SetFieldUsePlacementWorldTransform()
{
  BDSAcceleratorComponent::SetFieldUsePlacementWorldTransform();
  if (vacuumFieldInfo)
    {vacuumFieldInfo->SetUsePlacementWorldTransform(true);}
  if (outerFieldInfo)
    {outerFieldInfo->SetUsePlacementWorldTransform(true);}
}
