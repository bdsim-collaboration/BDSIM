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



#include "CLHEP/Units/SystemOfUnits.h"

#include "parser/element.h"
#include "parser/elementtype.h"

#include "BDSMagnetNoneSplitOuter.hh"

using namespace GMAD;

class G4Userlimits;

BDSMagnetNoneSplitOuter::BDSMagnetNoneSplitOuter(BDSMagnetType typeIn,
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


void BDSMagnetNoneSplitOuter::SBendWithSingleOuter(const G4String&         elementName,
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
    Element* el = new Element(*element);
    el->magnetGeometryType = "none";
    BDSAcceleratorComponent* pipeLine = BDS::BuildSBendLine(elementName,el,st,brho,integratorSet,incomingFaceAngle,outgoingFaceAngle,buildFringeFields,prevElement,nextElement);
    delete el;

    pipeLine->Initialise();

    BDSLine::iterator it = reinterpret_cast<BDSLine*>(pipeLine)->begin();

    if (buildFringeFields)
    {std::advance(it, 1);}

    outer = BDSMagnetOuterFactory::Instance()->CreateMagnetOuter(BDSMagnetType::sectorbend, magnetOuterInfo, arcLength, chordLength, reinterpret_cast<BDSMagnet*>(*it)->BeamPipe());

    G4double offsetAngle = element->angle/2;
    G4double offsetLength = chordLength/2;

    G4ThreeVector     initialGlobalPosition = G4ThreeVector(0,0, -offsetLength) ;
    G4ThreeVector u = G4ThreeVector( std::cos(-offsetAngle), 0, std::sin(-offsetAngle));
    G4ThreeVector v = G4ThreeVector(0, 1,0);
    G4ThreeVector w = G4ThreeVector(-std::sin(-offsetAngle), 0, std::cos(-offsetAngle));
    G4RotationMatrix* initialGlobalRotation  =  new G4RotationMatrix(u, v, w);
    G4double          initialS = 0;

    BDSBeamline* beamline = new BDSBeamline(initialGlobalPosition, initialGlobalRotation,initialS);

    BDSLine::iterator it2 = reinterpret_cast<BDSLine*>(pipeLine)->begin();

    for(it2;it2!=reinterpret_cast<BDSLine*>(pipeLine)->end();it2++)

    {
     beamline->AddComponent(reinterpret_cast<BDSMagnet*>(*it2));
    }

    BDSSimpleComponent* sbend = new BDSSimpleComponent("sbend", beamline->GetTotalArcLength(), beamline->GetTotalAngle(),outer->GetContainerSolid(),outer->GetContainerLogicalVolume(),outer->GetInnerExtent(),G4ThreeVector(0,0,-1),G4ThreeVector(0,0,1),beamPipeInfo);

    G4int i = 0;
    for (auto element : *beamline)
    {

        G4String placementName = element->GetPlacementName() + "_pv";
        G4Transform3D* placementTransform = element->GetPlacementTransform();
        G4int copyNumber = i;
        auto pv = new G4PVPlacement(*placementTransform,                  // placement transform
                                    element->GetContainerLogicalVolume(), // volume to be placed
                                    placementName,                        // placement name
                                    outer->GetContainerLogicalVolume(),                          // volume to place it in
                                    false,                                // no boolean operation
                                    copyNumber,                           // copy number
                                    checkOverlaps);                       // overlap checking

        i++; // for incremental copy numbers

        sbend->RegisterPhysicalVolume(pv);

    }

    BDSLine::iterator it3 = reinterpret_cast<BDSLine*>(pipeLine)->begin();

    for(it3;it3!=reinterpret_cast<BDSLine*>(pipeLine)->end();it3++)
    {
        sbend->RegisterDaughter(*it3);
    }

    RegisterDaughter(sbend);

    containerLogicalVolume = sbend->GetContainerLogicalVolume();
    containerSolid = sbend->GetContainerSolid();

    innerExtent = sbend->GetInnerExtent();
    outerExtent = sbend->GetExtent();

}

void BDSMagnetNoneSplitOuter::Build()
{
   SBendWithSingleOuter(element->name, element, st, brho, integratorSet,
                         incomingFaceAngle, outgoingFaceAngle, buildFringeFields,
                         prevElement, nextElement);

    //BuildUserLimits();
    //BuildVacuumField();
    //BuildOuter();
    // Instead of BDSAcceleratorComponent::Build just call BuildContainerLogicalVolume
    // to control user limits in the case where there is no container and we just inherit
    // the beam pipe container
    //BuildContainerLogicalVolume();
    //BuildOuterField(); // must be done when the containerLV exists
    //PlaceComponents(); // place things (if needed) in container
}
