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
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBeamPipeType.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetStrength.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSMagnetType.hh"
#include "BDSUtilities.hh"
#include "BDSMagnet.hh"
#include "BDSLine.hh"
#include "BDSComponentFactory.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSSimpleComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSBendBuilder.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Types.hh"
#include "G4VPhysicalVolume.hh"
#include "BDSIntegratorSet.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSGeometryFactoryGDML.hh"
#include "BDSGeometryExternal.hh"



#include "CLHEP/Units/SystemOfUnits.h"

#include "parser/element.h"
#include "parser/elementtype.h"

#include "BDSMagnetNonSplitOuter.hh"

using namespace GMAD;

class G4Userlimits;

BDSMagnetNonSplitOuter::BDSMagnetNonSplitOuter(BDSMagnetType typeIn,
                                                 BDSBeamPipeInfo *beamPipeInfoIn,
                                                 BDSMagnetOuterInfo *magnetOuterInfoIn,
                                                 BDSFieldInfo *vacuumFieldInfoIn,
                                                 BDSFieldInfo *outerFieldInfoIn,
                                                 G4bool isThinIn,
                                                 const Element*          elementIn,
                                                 BDSMagnetStrength*      stIn,
                                                 G4double                brhoIn,
                                                 const BDSIntegratorSet* integratorSetIn,
                                                 G4double                incomingFaceAngleIn,
                                                 G4double                outgoingFaceAngleIn,
                                                 G4bool                  buildFringeFieldsIn,
                                                 const GMAD::Element*    prevElementIn,
                                                 const GMAD::Element*    nextElementIn)
                                                 :BDSMagnet(typeIn, elementIn->name, elementIn->l* CLHEP::m, beamPipeInfoIn, magnetOuterInfoIn, vacuumFieldInfoIn, -elementIn->angle, outerFieldInfoIn, isThinIn)
{
    element           = elementIn;
    st                = stIn;
    brho              = brhoIn;
    integratorSet     = integratorSetIn;
    incomingFaceAngle = incomingFaceAngleIn;
    outgoingFaceAngle = outgoingFaceAngleIn;
    buildFringeFields = buildFringeFieldsIn;
    prevElement       = prevElementIn;
    nextElement       = nextElementIn;
    beamPipeInfo      = beamPipeInfoIn;
    magnetOuterInfo   = magnetOuterInfoIn;
    vacuumFieldInfo   = vacuumFieldInfoIn;
    outerFieldInfo    = outerFieldInfoIn;
}


void BDSMagnetNonSplitOuter::SBendWithSingleOuter(const G4String&         elementName,
                                                 const Element*          element,
                                                 BDSMagnetStrength*      st,
                                                 G4double                brho,
                                                 const BDSIntegratorSet* integratorSet,
                                                 G4double                incomingFaceAngle,
                                                 G4double                outgoingFaceAngle,
                                                 G4bool                  buildFringeFields,
                                                 const GMAD::Element*    prevElement,
                                                 const GMAD::Element*    nextElement)
{
    Element el = Element(*element);
    el.magnetGeometryType = "none";
    BDSAcceleratorComponent* pipeLine = BDS::BuildSBendLine(elementName,&el,st,brho,integratorSet,incomingFaceAngle,outgoingFaceAngle,buildFringeFields,prevElement,nextElement);
    pipeLine->Initialise();

    BDSBeamPipeInfo* beamPipeInfo1 = new BDSBeamPipeInfo(*beamPipeInfo);
    std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(el.angle/2 + incomingFaceAngle, el.angle/2 + outgoingFaceAngle);
    beamPipeInfo1->inputFaceNormal = faces.first;
    beamPipeInfo1->outputFaceNormal = faces.second;

    BDSBeamPipe* beamPipeTest = BDSBeamPipeFactory::Instance()->CreateBeamPipe(name+"_bp",
                                                   chordLength - 2*lengthSafety,
                                                                           beamPipeInfo1);

    outer = BDSMagnetOuterFactory::Instance()->CreateMagnetOuter(BDSMagnetType::sectorbend, magnetOuterInfo, chordLength-2*lengthSafety, chordLength, beamPipeTest);

    delete beamPipeTest;
    delete beamPipeInfo1;

    BDSGeometryComponent* container = outer->GetMagnetContainer();
    containerLogicalVolume = container->GetContainerLogicalVolume();
    containerSolid    = container->GetContainerSolid()->Clone();
    G4ThreeVector contOffset = container->GetPlacementOffset();
    // set the main offset of the whole magnet which is placed w.r.t. the
    // zero coordinate of the container solid
    SetPlacementOffset(contOffset);
    RegisterDaughter(outer);
    InheritExtents(container, contOffset); // update extents
    // Only clear after extents etc have been used
    outer->ClearMagnetContainer();
    endPieceBefore = outer->EndPieceBefore();
    endPieceAfter  = outer->EndPieceAfter();
    /// Update record of normal vectors now beam pipe has been constructed.
    SetInputFaceNormal(BDS::RotateToReferenceFrame(outer->InputFaceNormal(), angle));
    SetOutputFaceNormal(BDS::RotateToReferenceFrame(outer->OutputFaceNormal(), -angle));

    if (!element->extractOuterContainer)
    {

        G4double offsetAngle = element->angle/2;
        G4double offsetLength = chordLength/2;

        G4ThreeVector     initialGlobalPosition = G4ThreeVector(0,0, -offsetLength) ;
        G4ThreeVector u = G4ThreeVector( std::cos(-offsetAngle), 0, std::sin(-offsetAngle));
        G4ThreeVector v = G4ThreeVector(0, 1,0);
        G4ThreeVector w = G4ThreeVector(-std::sin(-offsetAngle), 0, std::cos(-offsetAngle));
        G4RotationMatrix* initialGlobalRotation  =  new G4RotationMatrix(u, v, w);
        G4double          initialS = 0;

        BDSBeamline* beamline = new BDSBeamline(initialGlobalPosition, initialGlobalRotation,initialS);

        beamline->AddComponent(pipeLine);

        G4int i = 0;
        for (auto element : *beamline)
        {

            G4String placementName = element->GetPlacementName() + "_pv";
            G4Transform3D* placementTransform = element->GetPlacementTransform();
            G4int copyNumber = i;
            auto pv = new G4PVPlacement(*placementTransform,                  // placement transform
                                        element->GetContainerLogicalVolume(), // volume to be placed
                                        placementName,                        // placement name
                                        containerLogicalVolume,                          // volume to place it in
                                        false,                                // no boolean operation
                                        copyNumber,                           // copy number
                                        checkOverlaps);                       // overlap checking

            i++; // for incremental copy numbers

            RegisterPhysicalVolume(pv);

        }

        G4ThreeVector outerOffset = outer->GetPlacementOffset();

        std::cout << "placing the content of the gdml file" << std::endl;
        auto gdml_world = outer->GetContainerLogicalVolume();
        for (G4int j = 0; j < gdml_world->GetNoDaughters(); j++)
        {
            const auto& pv = gdml_world->GetDaughter(j);
            G4String placementName = pv->GetName() + "_pv";
            std::cout << "placing " << placementName << std::endl;
            G4int copyNumber = 1;

            auto vv = new G4PVPlacement(pv->GetRotation(), pv->GetTranslation(),                  // placement transform
                                        pv->GetLogicalVolume(), // volume to be placed
                                        placementName,                        // placement name
                                        containerLogicalVolume,                          // volume to place it in
                                        false,                                // no boolean operation
                                        copyNumber,                           // copy number
                                        checkOverlaps);                       // overlap checking
            RegisterPhysicalVolume(vv);
    }

    }



}

void BDSMagnetNonSplitOuter::Build()
{
   SBendWithSingleOuter(element->name, element, st, brho, integratorSet,
                         incomingFaceAngle, outgoingFaceAngle, buildFringeFields,
                         prevElement, nextElement);

   if (element->fieldOuter != "" and !(element->extractOuterContainer))
   {
       BuildOuterField(); // must be done when the containerLV exists
   }
}
