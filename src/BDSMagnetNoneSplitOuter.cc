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


#include "CLHEP/Units/SystemOfUnits.h"

#include "parser/element.h"
#include "parser/elementtype.h"

#include "BDSMagnetNoneSplitOuter.hh"

using namespace GMAD;

class G4Userlimits;

BDSMagnetNoneSplitOuter::BDSMagnetNoneSplitOuter(BDSMagnetType typeIn, const G4String &nameIn, G4double lengthIn,
                                                 BDSBeamPipeInfo *beamPipeInfoIn, BDSMagnetOuterInfo *magnetOuterInfoIn,
                                                 BDSFieldInfo *vacuumFieldInfoIn, G4double angleIn,
                                                 BDSFieldInfo *outerFieldInfoIn, G4bool isThinIn)
                                                 :BDSMagnet(typeIn, nameIn, lengthIn, beamPipeInfoIn, magnetOuterInfoIn, vacuumFieldInfoIn, angleIn, outerFieldInfoIn, isThinIn)
{;}

BDSMagnetNoneSplitOuter* BDSMagnetNoneSplitOuter::BuildSingleSBend(const GMAD::Element*     element,
                                 const G4String&          name,
                                 G4double                 arcLength,
                                 G4double                 angle,
                                 G4double                 angleIn,
                                 G4double                 angleOut,
                                 const BDSMagnetStrength* strength,
                                 G4double                 brho,
                                 const BDSIntegratorSet*  integratorSet,
                                 G4bool                   yokeOnLeft,
                                 const BDSFieldInfo*      outerFieldIn)
{
    auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, angleIn, angleOut);

    BDSMagnetStrength* strengthCopy = new BDSMagnetStrength(*strength); // the copy is crucial to copy the field strength
    auto  magnetOuterInfo = BDSComponentFactory::PrepareMagnetOuterInfo(name, element, angleIn, angleOut, bpInfo, yokeOnLeft);
    // set the name to the desired one rather than the one from the element
    magnetOuterInfo->name = name;

    G4Transform3D fieldTiltOffset = BDSComponentFactory::CreateFieldTransform(element);

    G4bool finiteK1 = BDS::IsFinite((*strength)["k1"]);
    BDSFieldType dipoleFieldType = finiteK1 ? BDSFieldType::dipolequadrupole : BDSFieldType::dipole;
    BDSIntegratorType intType = BDS::GetDipoleIntegratorType(integratorSet, element);
    BDSFieldInfo* vacuumField = new BDSFieldInfo(dipoleFieldType,
                                                 brho,
                                                 intType,
                                                 strengthCopy,
                                                 true,
                                                 fieldTiltOffset);

    BDSFieldInfo* outerField = new BDSFieldInfo(*outerFieldIn);

    BDSMagnetNoneSplitOuter* magnet = new BDSMagnetNoneSplitOuter(BDSMagnetType::sectorbend,
                                      name,
                                      arcLength,
                                      bpInfo,
                                      magnetOuterInfo,
                                      vacuumField,
                                      -angle,
                                      outerField);

    return magnet;
}

BDSMagnetNoneSplitOuter* BDSMagnetNoneSplitOuter::BuildDipoleFringe(const GMAD::Element*     element,
                                  G4double                 angleIn,
                                  G4double                 angleOut,
                                  const G4String&          name,
                                  const BDSMagnetStrength* st,
                                  G4double                 brho,
                                  const BDSIntegratorSet*  integratorSet,
                                  BDSFieldType             dipoleFieldType)
{
    BDSBeamPipeInfo* beamPipeInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, angleIn, angleOut);
    beamPipeInfo->beamPipeType = BDSBeamPipeType::circularvacuum;
    auto magnetOuterInfo = BDSComponentFactory::PrepareMagnetOuterInfo(name, element,
                                                                       angleIn, angleOut, beamPipeInfo);
    magnetOuterInfo->geometryType   = BDSMagnetGeometryType::none;
    magnetOuterInfo->name           = name;
    magnetOuterInfo->buildEndPieces = false;

    G4Transform3D fieldTiltOffset = BDSComponentFactory::CreateFieldTransform(element);

    BDSIntegratorType intType = integratorSet->dipoleFringe;
    BDSFieldInfo* vacuumField = new BDSFieldInfo(dipoleFieldType,
                                                 brho,
                                                 intType,
                                                 st,
                                                 true,
                                                 fieldTiltOffset);

    return new BDSMagnetNoneSplitOuter(BDSMagnetType::dipolefringe,
                         name,
                         (*st)["length"],
                         beamPipeInfo,
                         magnetOuterInfo,
                         vacuumField,
                         -(*st)["angle"],
                         nullptr,
                         true);
}

BDSAcceleratorComponent* BDSMagnetNoneSplitOuter::SBendWithSingleOuter(const G4String&         elementName,
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
    BDSLine* pipeLine = BuildSBendLine(elementName,el,st,brho,integratorSet,incomingFaceAngle,outgoingFaceAngle,buildFringeFields,prevElement,nextElement);
    delete el;

    const G4bool yokeOnLeft = BDSComponentFactory::YokeOnLeft(element,st);
    auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, -incomingFaceAngle, -outgoingFaceAngle);
    auto mgInfo = BDSComponentFactory::PrepareMagnetOuterInfo(elementName, element, -incomingFaceAngle, -outgoingFaceAngle, bpInfo, yokeOnLeft);
    const G4double arcLength = pipeLine->GetArcLength()* CLHEP::m;
    const G4double chordLength = pipeLine->GetChordLength()* CLHEP::m;

    BDSLine::iterator it = pipeLine->begin();

    if (buildFringeFields)
    {std::advance(it, 1);}

    BDSMagnetOuter* magnetOuter = BDSMagnetOuterFactory::Instance()->CreateMagnetOuter(BDSMagnetType::sectorbend, mgInfo, arcLength, chordLength, dynamic_cast<BDSMagnetNoneSplitOuter*>(*it)->BeamPipe());

    G4double offsetAngle = element->angle/2;
    G4double offsetLength = chordLength/2;

    G4ThreeVector     initialGlobalPosition = G4ThreeVector(0,0, -offsetLength) ;

    G4ThreeVector u = G4ThreeVector( std::cos(offsetAngle), 0, std::sin(offsetAngle));
    G4ThreeVector v = G4ThreeVector(0, 1,0);
    G4ThreeVector w = G4ThreeVector(-std::sin(offsetAngle), 0, std::cos(offsetAngle));
    G4RotationMatrix* initialGlobalRotation  =  new G4RotationMatrix(u, v, w);

    G4double          initialS = 0;

    BDSBeamline* beamline = new BDSBeamline(initialGlobalPosition,initialGlobalRotation, initialS);

    beamline->AddComponent(pipeLine);

    BDSSimpleComponent* sbend = new BDSSimpleComponent("sbend", beamline->GetTotalArcLength(), beamline->GetTotalAngle(),magnetOuter->GetContainerSolid(),magnetOuter->GetContainerLogicalVolume(),magnetOuter->GetExtent(),G4ThreeVector(0,0,-1),G4ThreeVector(0,0,1),bpInfo);

    G4int i = 0;

    G4int j = 0;

    for (BDSBeamlineElement *el : *beamline) {

        std::set<BDSGeometryComponent*> daughtersSet = (*el).GetAcceleratorComponent()->GetAllDaughters();
        std::set<BDSGeometryComponent*>::iterator it;

        if(j!=100) {

            for (it = daughtersSet.begin(); it != daughtersSet.end(); ++it) {

                G4String placementName = (*it)->GetName() + "_pv";
                //G4Transform3D placementTransform = el->GetAcceleratorComponent()->GetPlacementTransform();
                G4Transform3D placementTransform = (*it)->GetPlacementTransform();
                G4int copyNumber = i;
                auto pv = new G4PVPlacement(placementTransform,                  // placement transform
                                            (*it)->GetContainerLogicalVolume(), // volume to be placed
                                            placementName,                        // placement name
                                            magnetOuter->GetContainerLogicalVolume(),// volume to place it in
                                            false,                                // no boolean operation
                                            copyNumber,                           // copy number
                                            true);                       // overlap checking
                sbend->RegisterDaughter((*it));

                i++; // for incremental copy numbers

            }
        }
        j++;

    }



    return sbend;
}


BDSLine* BDSMagnetNoneSplitOuter::BuildSBendLine(const G4String&         elementName,
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
    const G4String             baseName = elementName;
    const G4double thinElementArcLength = BDSGlobalConstants::Instance()->ThinElementLength();
    const G4bool             yokeOnLeft = BDSComponentFactory::YokeOnLeft(element,st);
    const G4double            arcLength = element->l  * CLHEP::m;
    const G4double                angle = (*st)["angle"];
    G4double                       fint = element->fint;
    G4double                      fintx = element->fintx;
    G4double                       hgap = element->hgap * CLHEP::m;

    // Note for tilted dipoles, the geometry is tilted but the curvilinear world isn't,
    // therefore we tilt the field to match the geometry.
    G4Transform3D fieldTiltOffset = BDSComponentFactory::CreateFieldTransform(element);

    G4bool buildFringeIncoming = buildFringeFields;
    G4bool buildFringeOutgoing = buildFringeFields;

    G4bool finiteK1 = BDS::IsFinite((*st)["k1"]);
    BDSFieldType dipoleFieldType = finiteK1 ? BDSFieldType::dipolequadrupole : BDSFieldType::dipole;

    if (buildFringeFields)
    {
        // perform series of checks on fringe field parameters
        // nominally, don't build the fringe element if there's no face angle
        if (!BDS::IsFinite(element->e1))
        {buildFringeIncoming = false;} // by default false
        if (!BDS::IsFinite(element->e2))
        {buildFringeOutgoing = false;}

        // fintx = -1 is how madx writes out that fintx should default to fint
        // it's also our default. If by default fint=0 and fintx=0, no fringes
        // would be built. If finite fint and -1 for fintx both will be built.
        // if fint=0 and fintx != -1, only the outgoin will be built
        if (fintx == -1)
        {fintx = fint;}

        // however if finite hgap and fint or fintx specified, there is an effect
        if (BDS::IsFinite(fint) && BDS::IsFinite(hgap))
        {buildFringeIncoming = true;}
        if (BDS::IsFinite(fintx) && BDS::IsFinite(hgap))
        {buildFringeOutgoing = true;}

        // overriding checks - don't build fringe field if we're butt against another
        // sbend.
        if (prevElement)
        {
            if (prevElement->type == ElementType::_SBEND)
            {
                buildFringeIncoming = false;
                if (BDS::IsFinite(prevElement->e2 - element->e1))
                {throw BDSException( __METHOD_NAME__, prevElement->name + " e2 clashes with " + elementName + " e1");}
            }
        }
        if (nextElement)
        {
            if (nextElement->type == ElementType::_SBEND)
            {buildFringeOutgoing = false;}
            // check above on clashing sbends with e1 e2 will be used for next bend.
        }

        if (!BDS::IsFinite(angle))
        {// no fringe fields if magnet of zero angle, ie strength
            buildFringeIncoming = false;
            buildFringeOutgoing = false;
        }
    }// end of checks

    // Calculate number of sbends to split parent into
    G4int nSBends = BDS::CalculateNSBendSegments(arcLength, angle, incomingFaceAngle,
                                                 outgoingFaceAngle);

    // We're definitely building a line now.
    BDSLine* sbendline  = new BDSLine(baseName); // line for resultant sbend

    // length of bends minus the fringe segments
    G4double centralArcLength = arcLength;
    G4double centralAngle     = angle;
    G4double oneFringeAngle   = 0;
    if (BDS::IsFinite(angle))
    {oneFringeAngle = (thinElementArcLength / arcLength) * angle;}

    if (buildFringeIncoming)
    {
        centralArcLength -= thinElementArcLength;
        centralAngle     -= oneFringeAngle;
    }
    if (buildFringeOutgoing)
    {
        centralArcLength -= thinElementArcLength;
        centralAngle     -= oneFringeAngle;
    }

    // calculate their angles and length
    G4double semiAngle     = centralAngle / (G4double) nSBends;
    G4double semiArcLength = centralArcLength / (G4double) nSBends;

    BDSMagnetStrength* semiStrength = new BDSMagnetStrength(*st); // the copy is crucial to copy the field strength
    (*semiStrength)["angle"]  = semiAngle;
    (*semiStrength)["length"] = semiArcLength;

    G4double zExtentIn  = 0;
    G4double zExtentOut = 0;
    G4bool fadeIn = true;
    G4bool fadeOut = true;

    // unlike an rbend, the sbend will mostly likely be split up into segments.
    // we must check that the faces of each segment (varying from e1 to e2) will
    // not overlap given the outer diameter.
    // calculate extent along z due poleface rotation at half the horizontal width.
    G4double horizontalWidth = BDSComponentFactory::PrepareHorizontalWidth(element);
    if (incomingFaceAngle > 0)
    {zExtentIn = 0.5*horizontalWidth*std::tan(incomingFaceAngle - 0.5*std::abs(semiAngle));}
    else if (incomingFaceAngle < 0)
    {zExtentIn = 0.5*horizontalWidth*std::tan(0.5*std::abs(semiAngle) + incomingFaceAngle);}
    if (outgoingFaceAngle > 0)
    {zExtentOut = 0.5*horizontalWidth*std::tan(outgoingFaceAngle - 0.5*std::abs(semiAngle));}
    else if (outgoingFaceAngle < 0)
    {zExtentOut = 0.5*horizontalWidth*std::tan(0.5*std::abs(semiAngle) + outgoingFaceAngle);}

    //decide if segment angles fade or not depending on the extents
    if (std::abs(zExtentIn) < semiArcLength/4.0)
    {fadeIn = false;}
    if (std::abs(zExtentOut) < semiArcLength/4.0)
    {fadeOut = false;}

    // field recipe for one segment of the sbend
    G4String centralName = baseName + "_even_ang";
    BDSIntegratorType intType = BDS::GetDipoleIntegratorType(integratorSet, element);
    BDSFieldInfo* semiVacuumField = new BDSFieldInfo(dipoleFieldType,
                                                     brho,
                                                     intType,
                                                     semiStrength,
                                                     true,
                                                     fieldTiltOffset);

    auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, 0.5*semiAngle, 0.5*semiAngle);
    auto mgInfo = BDSComponentFactory::PrepareMagnetOuterInfo(centralName, element,
                                                              0.5*semiAngle, 0.5*semiAngle, bpInfo,
                                                              yokeOnLeft);
    BDSFieldInfo* semiOuterField = BDSComponentFactory::PrepareMagnetOuterFieldInfo(semiStrength,
                                                                                    BDSFieldType::dipole,
                                                                                    bpInfo,
                                                                                    mgInfo,
                                                                                    fieldTiltOffset,
                                                                                    integratorSet,
                                                                                    brho);
    mgInfo->name = centralName;
    BDSMagnetNoneSplitOuter* centralWedge = new BDSMagnetNoneSplitOuter(BDSMagnetType::sectorbend,
                                            centralName,
                                            semiArcLength,
                                            bpInfo,
                                            mgInfo,
                                            semiVacuumField,
                                            -semiAngle,
                                            semiOuterField); // minus for 3d cartesian conversion

    // check magnet outer info
    auto magnetOuterInfoCheck = BDSComponentFactory::PrepareMagnetOuterInfo("checking", element,
                                                                            -incomingFaceAngle,
                                                                            -outgoingFaceAngle,
                                                                            bpInfo, yokeOnLeft);
    // minus for conversion to 3d cartesian
    G4double minimalRadius = 2*magnetOuterInfoCheck->MinimumIntersectionRadiusRequired();
    // extra pedantic check for dipoles with certain geometry types
    if (!magnetOuterInfoCheck->hStyle)
    {// in the case of C-shaped poled dipoles, the full 'horizontalWidth' is shifted to the yoke side
        switch (magnetOuterInfoCheck->geometryType.underlying())
        {
            case BDSMagnetGeometryType::polescircular:
            case BDSMagnetGeometryType::polesfacet:
            case BDSMagnetGeometryType::polesfacetcrop:
            case BDSMagnetGeometryType::polessquare:
            {
                minimalRadius *= element->yokeOnInside ? 2.0 : 0.5;
                break;
            }
            default:
            {break;}
        }
    }
    BDSComponentFactory::CheckBendLengthAngleWidthCombo(semiArcLength, -semiAngle, minimalRadius, element->name);
    delete magnetOuterInfoCheck; // clean up

    // build incoming fringe field if required
    if (buildFringeIncoming)
    {
        BDSMagnetStrength* fringeStIn = BDS::GetFringeMagnetStrength(element, st, oneFringeAngle,
                                                                     element->e1, element->e2, fintx, 1);
        G4String segmentName           = baseName + "_e1_fringe";
        G4double fringeAngleIn         = 0.5*oneFringeAngle - incomingFaceAngle;
        G4double fringeAngleOut        = 0.5*oneFringeAngle + incomingFaceAngle;
        BDSMagnetNoneSplitOuter* startfringe = BuildDipoleFringe(element, fringeAngleIn, fringeAngleOut,
                                                        segmentName, fringeStIn, brho,
                                                        integratorSet, dipoleFieldType);
        sbendline->AddComponent(startfringe);
    }

    // logic for elements in the sbend line:
    // reuse central segment for all segment of in/out half if no poleface angle(s)
    // if small poleface, new first/last segment, reuse central wedge for remainder of in/out half
    // otherwise fade in/out faces for all segments in app. halves.
    // 'central' one is definitely used for the central part, but also it's just a segment
    // with even incoming and outgoing face angles w.r.t. the chord.
    G4double segmentAngleIn  = 0;
    G4double segmentAngleOut = 0;
    G4int    numberOfUniqueComponents = 1; // used for naming purposes
    BDSMagnetNoneSplitOuter* oneBend = nullptr;
    const G4int numSegmentsEitherSide = (nSBends - 1) / 2;
    for (G4int i = 0; i < nSBends; ++i)
    {
        G4String name = baseName;
        if (i < numSegmentsEitherSide)
        {// first half of magnet
            if (!BDS::IsFinite(incomingFaceAngle)) // no pole face rotation so just repeat central segment
            {oneBend = centralWedge;}
            else if (fadeIn) // build incremented angled segment
            {
                name += "_"+std::to_string(numberOfUniqueComponents);
                numberOfUniqueComponents++;
                BDS::UpdateSegmentAngles(i,nSBends,semiAngle,incomingFaceAngle,outgoingFaceAngle,segmentAngleIn,segmentAngleOut);
                oneBend = BuildSingleSBend(element, name, semiArcLength, semiAngle,
                                                segmentAngleIn, segmentAngleOut, semiStrength,
                                                brho, integratorSet, yokeOnLeft, semiOuterField);
            }
            else
            {// finite pole face, but not strong so build one angled, then repeat the rest to save memory
                if (i == 0) // the first one is unique
                {
                    name += "_"+std::to_string(numberOfUniqueComponents);
                    numberOfUniqueComponents++;
                    segmentAngleIn  = 0.5*semiAngle - incomingFaceAngle; // whole pole face angle
                    segmentAngleOut = 0.5*semiAngle;      // even matching angle
                    oneBend = BuildSingleSBend(element, name, semiArcLength, semiAngle,
                                                    segmentAngleIn, segmentAngleOut, semiStrength,
                                                    brho, integratorSet, yokeOnLeft, semiOuterField);
                }
                else // others afterwards are a repeat of the even angled one
                {oneBend = centralWedge;}
            }
        }
        else if (i > numSegmentsEitherSide) // i is zero counting
        {// second half of magnet
            if (!BDS::IsFinite(outgoingFaceAngle)) // no pole face rotation so just repeat central segment
            {oneBend = centralWedge;}
            else if (fadeOut) // build incremented angled segment
            {
                name += "_"+std::to_string(numberOfUniqueComponents);
                numberOfUniqueComponents++;
                BDS::UpdateSegmentAngles(i,nSBends,semiAngle,incomingFaceAngle,outgoingFaceAngle,segmentAngleIn,segmentAngleOut);
                oneBend = BuildSingleSBend(element, name, semiArcLength, semiAngle,
                                                segmentAngleIn, segmentAngleOut, semiStrength,
                                                brho, integratorSet, yokeOnLeft, semiOuterField);
            }
            else
            {// finite pole face, but not strong so build only one unique angled on output face
                if (i == (nSBends-1)) // last one
                {
                    name += "_"+std::to_string(numberOfUniqueComponents);
                    numberOfUniqueComponents++;
                    segmentAngleIn  = 0.5*semiAngle;
                    segmentAngleOut = 0.5*semiAngle - outgoingFaceAngle;
                    oneBend = BuildSingleSBend(element, name, semiArcLength, semiAngle,
                                                    segmentAngleIn, segmentAngleOut, semiStrength,
                                                    brho, integratorSet, yokeOnLeft, semiOuterField);
                }
                else // after central, but before unique end piece - even angled.
                {oneBend = centralWedge;}
            }
        }
        else // the middle piece
        {oneBend = centralWedge;}

        // append to the line

        oneBend->BuildBeampipe();
        sbendline->AddComponent(oneBend);
    }

    //Last element should be fringe if poleface specified
    if (buildFringeOutgoing)
    {
        BDSMagnetStrength* fringeStOut = BDS::GetFringeMagnetStrength(element, st, oneFringeAngle,
                                                                      element->e1, element->e2, fintx, 0);
        G4double fringeAngleIn          = 0.5*oneFringeAngle + outgoingFaceAngle;
        G4double fringeAngleOut         = 0.5*oneFringeAngle - outgoingFaceAngle;
        G4String segmentName            = baseName + "_e2_fringe";

        BDSMagnetNoneSplitOuter* endfringe = BuildDipoleFringe(element, fringeAngleIn, fringeAngleOut,
                                                      segmentName, fringeStOut, brho,
                                                      integratorSet, dipoleFieldType);
        sbendline->AddComponent(endfringe);
    }
    return sbendline;
}

void BDSMagnetNoneSplitOuter::Build()
{
    BuildUserLimits();
    BuildVacuumField();
    BuildOuter();
    // Instead of BDSAcceleratorComponent::Build just call BuildContainerLogicalVolume
    // to control user limits in the case where there is no container and we just inherit
    // the beam pipe container
    BuildContainerLogicalVolume();
    BuildOuterField(); // must be done when the containerLV exists
    PlaceComponents();
}