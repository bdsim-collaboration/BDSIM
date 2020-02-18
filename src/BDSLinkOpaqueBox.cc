/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "BDSAcceleratorComponent.hh"
#include "BDSApertureInfo.hh"
#include "BDSApertureType.hh"
#include "BDSBeamline.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSLinkOpaqueBox.hh"
#include "BDSMaterials.hh"
#include "BDSSamplerCustom.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4TwoVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <limits>

BDSLinkOpaqueBox::BDSLinkOpaqueBox(BDSAcceleratorComponent* acceleratorComponentIn,
				   G4double tiltIn,
				   G4double outputSamplerRadiusIn):
  BDSGeometryComponent(nullptr, nullptr),
  component(acceleratorComponentIn),
  tilt(tiltIn),
  outputSamplerRadius(outputSamplerRadiusIn),
  sampler(nullptr)
{
  if (BDS::IsFinite(tilt) && BDS::IsFinite(component->GetAngle()))
    {throw BDSException(__METHOD_NAME__, "finite tilt with angled component unsupported.");}
  
  BDSExtent extent = component->GetExtent();
  extent = extent.Tilted(tiltIn); // apply tilt
  const G4double gap                = 10 * CLHEP::cm;
  const G4double opaqueBoxThickness = 10 * CLHEP::mm;
  G4String name = component->GetName();

  G4Box* terminatorBoxOuter = new G4Box(name + "_terminator_box_outer_solid",
					0.5 * extent.DX() + gap + opaqueBoxThickness,
					0.5 * extent.DY() + gap + opaqueBoxThickness,
					0.5 * extent.DZ() + gap + opaqueBoxThickness);
  RegisterSolid(terminatorBoxOuter);
  G4Box* terminatorBoxInner = new G4Box(name + "_terminator_box_inner_solid",
					0.5 * extent.DX() + gap,
					0.5 * extent.DY() + gap,
					0.5 * extent.DZ() + gap);
  RegisterSolid(terminatorBoxInner);
  G4SubtractionSolid* opaqueBox = new G4SubtractionSolid(name + "_opaque_box_solid",
							 terminatorBoxOuter,
							 terminatorBoxInner);
  RegisterSolid(opaqueBox);
  G4LogicalVolume* opaqueBoxLV = new G4LogicalVolume(opaqueBox,
						     BDSMaterials::Instance()->GetMaterial("G4_Galactic"),
						     name + "_opaque_box_lv");
  RegisterLogicalVolume(opaqueBoxLV);

  G4UserLimits* termUL = new G4UserLimits();
  termUL->SetUserMinEkine(std::numeric_limits<double>::max());
  RegisterUserLimits(termUL);
  opaqueBoxLV->SetUserLimits(termUL);
  
  G4VisAttributes* obVis = new G4VisAttributes(*BDSColours::Instance()->GetColour("opaquebox"));
  obVis->SetVisibility(true);
  opaqueBoxLV->SetVisAttributes(obVis);
  RegisterVisAttributes(obVis);
  
  G4double ls = BDSGlobalConstants::Instance()->LengthSafety();
  G4double margin = gap + opaqueBoxThickness + ls;
  containerSolid = new G4Box(name + "_opaque_box_vacuum_solid",
			     0.5 * extent.DX() + margin,
			     0.5 * extent.DY() + margin,
			     0.5 * extent.DZ() + margin);
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       BDSMaterials::Instance()->GetMaterial("G4_Galactic"),
					       name + "_container_lv");
  containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());

  // auto boxPlacement = 
  new G4PVPlacement(nullptr,
		    G4ThreeVector(),
		    opaqueBoxLV,
		    name + "_opaque_box_pv",
		    containerLogicalVolume,
		    false,
		    1,
		    true);

  G4RotationMatrix* rm = nullptr;
  if (BDS::IsFinite(tilt))
    {
      rm = new G4RotationMatrix();
      rm->rotateZ(tilt);
      RegisterRotationMatrix(rm);
    }
  // auto componentPlacement =
  new G4PVPlacement(rm,
		    G4ThreeVector(),
		    component->GetContainerLogicalVolume(),
		    component->GetName() + "_pv",
		    containerLogicalVolume,
		    false,
		    1,
		    true);
  
  outerExtent = BDSExtent(0.5 * extent.DX() + gap + opaqueBoxThickness + ls,
			  0.5 * extent.DY() + gap + opaqueBoxThickness + ls,
			  0.5 * extent.DZ() + gap + opaqueBoxThickness + ls);

  G4RotationMatrix* rm2 = new G4RotationMatrix();
  G4TwoVector xy = G4TwoVector(component->Sagitta(),0);
  if (BDS::IsFinite(tilt))
    {xy.rotate(tilt);}
  else if (BDS::IsFinite(component->GetAngle()))
    {rm2->rotate(-0.5 * component->GetAngle(), G4ThreeVector(0,1,0));}
  offsetToStart = G4ThreeVector(xy.x(), xy.y(), -0.5*component->GetChordLength());
  transformToStart = G4Transform3D(rm->inverse(), offsetToStart);
}

BDSLinkOpaqueBox::~BDSLinkOpaqueBox()
{
  delete sampler;
}

void BDSLinkOpaqueBox::PlaceOutputSampler(G4int ID)
{  
  G4String samplerName = component->GetName() + "_sampler";
  BDSApertureType apt = BDSApertureType::circular;
  BDSApertureInfo ap = BDSApertureInfo(apt, outputSamplerRadius, 0, 0, 0);
  sampler = new BDSSamplerCustom(samplerName, ap);

  G4double pl = BDSBeamline::PaddingLength();
  G4ThreeVector position = G4ThreeVector(0,0,0.5*component->GetChordLength() + pl);
  G4RotationMatrix* rm = nullptr;
  if (BDS::IsFinite(component->GetAngle()))
    {
      rm = new G4RotationMatrix();
      rm->rotateY(0.5 * component->GetAngle()); // rotate to output face
      RegisterRotationMatrix(rm);
      position = G4ThreeVector(component->Sagitta(), 0, 0.5*component->GetChordLength());
      G4ThreeVector gap = G4ThreeVector(0,0,pl);
      position += gap.transform(*rm);
    }
  // if there's finite angle, we ensure (in constructor) there's no tilt
  
  new G4PVPlacement(rm,
      position,
      sampler->GetContainerLogicalVolume(),
      samplerName + "_pv",
      containerLogicalVolume,
      false,
      ID,
      true);
}
