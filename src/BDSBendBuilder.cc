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
#include "globals.hh" // geant4 globals / types

#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBendBuilder.hh"
#include "BDSComponentFactory.hh"
#include "BDSDebug.hh"
#include "BDSFieldType.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorSet.hh"
#include "BDSIntegratorType.hh"
#include "BDSMagnetStrength.hh"
#include "BDSLine.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSUtilities.hh"

#include "parser/element.h"
#include "parser/elementtype.h"

#include "G4Transform3D.hh"

using namespace GMAD;

BDSAcceleratorComponent* BDS::BuildSBendLine(const Element*          element,
					     BDSMagnetStrength*      st,
					     const G4double          brho,
					     const BDSIntegratorSet* integratorSet)
{
  const G4String             baseName = element->name;
  const G4bool          includeFringe = BDSGlobalConstants::Instance()->IncludeFringeFields();
  const G4double thinElementArcLength = BDSGlobalConstants::Instance()->ThinElementLength();
  const G4bool             yokeOnLeft = BDSComponentFactory::YokeOnLeft(element,st);
  G4bool          buildFringeIncoming = includeFringe;
  G4bool          buildFringeOutgoing = includeFringe;  
  const G4double            arcLength = element->l  * CLHEP::m;
  const G4double                angle = (*st)["angle"];
  G4double              bendingRadius = DBL_MAX; // default for zero angle
  // Avoid bad calculations for zero angle. Here we need bending radius to be in
  // correct global cartesian convention, hence -ve.
  if (BDS::IsFinite(angle))
    {bendingRadius = - brho / (*st)["field"];}
  
  G4Transform3D fieldTiltOffset = BDSComponentFactory::CreateFieldTransform(element);
  
  // face rotations
  // convention: +ve e1 / e2 reduces outside of bend
  G4double   factor = angle < 0 ? -1 : 1; 
  const G4double e1 = factor * element->e1 * CLHEP::rad;
  const G4double e2 = factor * element->e2 * CLHEP::rad;
  
  // don't build the fringe element if there's no face angle - no physical effect
  if (!BDS::IsFinite(e1))
    {buildFringeIncoming = false;}
  if (!BDS::IsFinite(e2))
    {buildFringeOutgoing = false;}
  
  // Calculate number of sbends to split parent into
  G4int nSBends = BDS::CalculateNSBendSegments(arcLength, angle, e1, e2);

  const G4double  angleIn = 0.5 * angle + e1;
  const G4double angleOut = 0.5 * angle + e2;
  
  // If no angle, means no strength, so even if there are pole face rotations, we don't
  // need the fringe elements. If there's only 1 segment we may still have a finite
  // angle and therefore finite strength and the pole face rotations will require fringe
  // elements
  if (!BDS::IsFinite(angle))
    {      
      BDSIntegratorType intType = BDS::GetDipoleIntegratorType(integratorSet, element);
      BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
						   brho,
						   intType,
						   st,
						   true,
						   fieldTiltOffset);
      // prepare one sbend segment
      auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, -angleIn, -angleOut);
      auto mgInfo = BDSComponentFactory::PrepareMagnetOuterInfo(element, -angleIn,
								-angleOut, yokeOnLeft);
      BDSMagnet* oneBend = new BDSMagnet(BDSMagnetType::sectorbend,
					 baseName,
					 arcLength,
					 bpInfo,
					 mgInfo,
					 vacuumField,
					 -angle); // angle 0, but should be minus for 3d cart.
      return oneBend;
    }

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

  BDSMagnetStrength* semiStrength = new BDSMagnetStrength(*st);
  (*semiStrength)["angle"]  = semiAngle;
  (*semiStrength)["length"] = semiArcLength;
  
  G4double zExtentIn  = 0;
  G4double zExtentOut = 0;
  G4bool fadeIn = true;
  G4bool fadeOut = true;

  // unlike an rbend, the sbend will mostly likely be split up into segments.
  // we must check that the faces of each segment (varying from e1 to e2) will
  // not overlap given the outer diamter.
  // calculate extent along z due poleface rotation at half the outer diameter.
  G4double outerDiameter = BDSComponentFactory::PrepareOuterDiameter(element);
  if (angleIn > 0)
    {zExtentIn = 0.5*outerDiameter*tan(angleIn - 0.5*std::abs(semiAngle));}
  else if (angleIn < 0)
    {zExtentIn = 0.5*outerDiameter*tan(0.5*std::abs(semiAngle) + angleIn);}
  if (angleOut > 0)
    {zExtentOut = 0.5*outerDiameter*tan(angleOut - 0.5*std::abs(semiAngle));}
  else if (angleOut < 0)
    {zExtentOut = 0.5*outerDiameter*tan(0.5*std::abs(semiAngle) + angleOut);}
  
  //decide if segment angles fade or not depending on the extents
  if (std::abs(zExtentIn) < semiArcLength/4.0)
    {fadeIn = false;}
  if (std::abs(zExtentOut) < semiArcLength/4.0)
    {fadeOut = false;}

  // field recipe for one segment of the sbend
  G4String centralName = baseName + "_even_ang";
    BDSIntegratorType intType = BDS::GetDipoleIntegratorType(integratorSet, element);
  BDSFieldInfo* semiVacuumField = new BDSFieldInfo(BDSFieldType::dipole,
						   brho,
						   intType,
						   semiStrength,
						   true,
                                                   fieldTiltOffset);
  
  auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, 0.5*semiAngle, 0.5*semiAngle);
  auto mgInfo = BDSComponentFactory::PrepareMagnetOuterInfo(element, 0.5*semiAngle, 0.5*semiAngle, yokeOnLeft);
  mgInfo->name = centralName;
  BDSMagnet* centralWedge = new BDSMagnet(BDSMagnetType::sectorbend,
					  centralName,
					  semiArcLength,
					  bpInfo,
					  mgInfo,
					  semiVacuumField,
					  -semiAngle); // minus for 3d cartesian conversion
  
  // check magnet outer info
  BDSMagnetOuterInfo* magnetOuterInfoCheck = BDSComponentFactory::PrepareMagnetOuterInfo(element, -angleIn, -angleOut, yokeOnLeft);
  // minus for conversion to 3d cartesian
  BDSComponentFactory::CheckBendLengthAngleWidthCombo(semiArcLength, -semiAngle,
						      magnetOuterInfoCheck->outerDiameter,
						      centralName);
  // clean up
  delete magnetOuterInfoCheck;
  
  // build incoming fringe field if required
  if (buildFringeIncoming)
    {
      BDSMagnetStrength* fringeStIn  = new BDSMagnetStrength(*st);
      (*fringeStIn)["length"]        = thinElementArcLength;
      (*fringeStIn)["angle"]         = oneFringeAngle;
      (*fringeStIn)["polefaceangle"] = e1;
      (*fringeStIn)["fringecorr"]    = CalculateFringeFieldCorrection(bendingRadius, e1, element->fint, element->hgap*CLHEP::m);
      G4String segmentName           = baseName + "_e1_fringe";
      G4double fringeAngleIn         = 0.5*oneFringeAngle - e1;
      G4double fringeAngleOut        = 0.5*oneFringeAngle + e1;
      BDSMagnet* startfringe = BDS::BuildDipoleFringe(element, fringeAngleIn, fringeAngleOut,
						      segmentName, fringeStIn, brho,
						      integratorSet);
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
  BDSMagnet* oneBend = nullptr;
  const G4int numSegmentsEitherSide = (nSBends - 1) / 2;
  for (G4int i = 0; i < nSBends; ++i)
    {
      G4String name = baseName;
      if (i < numSegmentsEitherSide)
        {// first half of magnet
          if (!BDS::IsFinite(e1)) // no pole face rotation so just repeat central segment
            {oneBend = centralWedge;}
          else if (fadeIn) // build incremented angled segment
            {
	      name += "_"+std::to_string(numberOfUniqueComponents);
	      numberOfUniqueComponents++;
	      BDS::UpdateSegmentAngles(i,nSBends,semiAngle,e1,e2,segmentAngleIn,segmentAngleOut);
	      oneBend = BDS::BuildSingleSBend(element, name, semiArcLength, semiAngle,
					      segmentAngleIn, segmentAngleOut, semiStrength,
					      brho, integratorSet, yokeOnLeft);
	    }
          else
            {// finite pole face, but not strong so build one angled, then repeat the rest to save memory
              if (i == 0) // the first one is unique
                {
		  name += "_"+std::to_string(numberOfUniqueComponents);
		  numberOfUniqueComponents++;
		  segmentAngleIn  = 0.5*semiAngle - e1; // whole pole face angle
		  segmentAngleOut = 0.5*semiAngle;      // even matching angle
		  oneBend = BDS::BuildSingleSBend(element, name, semiArcLength, semiAngle,
						  segmentAngleIn, segmentAngleOut, semiStrength,
						  brho, integratorSet, yokeOnLeft);
		}
              else // others afterwards are a repeat of the even angled one
                {oneBend = centralWedge;}
            }
        }
      else if (i > numSegmentsEitherSide) // i is zero counting
        {// second half of magnet
          if (!BDS::IsFinite(e2)) // no pole face rotation so just repeat central segment
            {oneBend = centralWedge;}
          else if (fadeOut) // build incremented angled segment
	    {
	      name += "_"+std::to_string(numberOfUniqueComponents);
	      numberOfUniqueComponents++;
	      BDS::UpdateSegmentAngles(i,nSBends,semiAngle,e1,e2,segmentAngleIn,segmentAngleOut);
	      oneBend = BDS::BuildSingleSBend(element, name, semiArcLength, semiAngle,
					      segmentAngleIn, segmentAngleOut, semiStrength,
					      brho, integratorSet, yokeOnLeft);
	    }
          else
            {// finite pole face, but not strong so build only one unique angled on output face
              if (i == (nSBends-1)) // last one
                {
		  name += "_"+std::to_string(numberOfUniqueComponents);
		  numberOfUniqueComponents++;
		  segmentAngleIn  = 0.5*semiAngle;
		  segmentAngleOut = 0.5*semiAngle - e2;
		  oneBend = BDS::BuildSingleSBend(element, name, semiArcLength, semiAngle,
						  segmentAngleIn, segmentAngleOut, semiStrength,
						  brho, integratorSet, yokeOnLeft);
		}
              else // after central, but before unique end piece - even angled.
                {oneBend = centralWedge;}
            }
        }
      else // the middle piece
        {oneBend = centralWedge;}

      // append to the line
      sbendline->AddComponent(oneBend);
    }
  
  //Last element should be fringe if poleface specified
  if (buildFringeOutgoing)
    {
      BDSMagnetStrength* fringeStOut  = new BDSMagnetStrength(*st);
      (*fringeStOut)["angle"]         = oneFringeAngle;
      (*fringeStOut)["polefaceangle"] = e2;
      (*fringeStOut)["fringecorr"]    = CalculateFringeFieldCorrection(bendingRadius, e2, element->fintx, element->hgap*CLHEP::m);
      (*fringeStOut)["length"]        = thinElementArcLength;
      G4double fringeAngleIn          = 0.5*oneFringeAngle + e2;
      G4double fringeAngleOut         = 0.5*oneFringeAngle - e2;
      G4String segmentName            = baseName + "_e2_fringe";
      
      BDSMagnet* endfringe = BDS::BuildDipoleFringe(element, fringeAngleIn, fringeAngleOut,
						    segmentName, fringeStOut, brho,
						    integratorSet);
      sbendline->AddComponent(endfringe);
    }
  return sbendline;
}

void BDS::UpdateSegmentAngles(const G4int index,
			      const G4int nSBends,
			      const G4double semiAngle,
			      const G4double e1,
			      const G4double e2,
			      G4double& segmentAngleIn,
			      G4double& segmentAngleOut)
{
  // nSBends is always odd.
  // index assumed 0 counting.
  G4bool central = index == (nSBends - 1) / 2;
  if (central)
    {// shouldn't happen, but just in case
      segmentAngleIn = 0.5*semiAngle;
      segmentAngleOut = 0.5*semiAngle;
      return;
    }
  
  G4bool       firstHalf = index + 1 < (nSBends + 1) / 2;
  G4int numberToFadeOver = (nSBends - 1) / 2;
  if (firstHalf)
    {// fade from e1 to 0
      G4double delta = e1 / (G4double) numberToFadeOver;
      G4double inputFaceAngle  = e1 - ((G4double)index * delta);
      G4double outputFaceAngle = e1 - ((G4double)(index+1) * delta);
      // if e1 was 0, each would have 1/2 of semiAngle
      segmentAngleIn  = 0.5*semiAngle - inputFaceAngle;
      segmentAngleOut = 0.5*semiAngle + outputFaceAngle;
    }
  else
    {// second half - fade from 0 to e2
      G4double delta = e2 / (G4double) numberToFadeOver;
      G4int secondHalfIndex = index - ((nSBends + 1) / 2);
      // here we fade from 0 contribution to maximum (opposite of above if statement)
      G4double inputFaceAngle  = e2 - ( (G4double)(numberToFadeOver - secondHalfIndex) * delta);
      G4double outputFaceAngle = e2 - ( (G4double)(numberToFadeOver - (secondHalfIndex + 1)) * delta);
      // if e1 was 0, each would have 1/2 of semiAngle
      segmentAngleIn  = 0.5*semiAngle + inputFaceAngle;
      segmentAngleOut = 0.5*semiAngle - outputFaceAngle;
    }

  return;
}

BDSMagnet* BDS::BuildSingleSBend(const GMAD::Element*     element,
				 const G4String           name,
				 const G4double           arcLength,
				 const G4double           angle,
				 const G4double           angleIn,
				 const G4double           angleOut,
				 const BDSMagnetStrength* strength,
				 const G4double           brho,
				 const BDSIntegratorSet*  integratorSet,
				 const G4bool             yokeOnLeft)
{
  BDSMagnetStrength* strengthCopy = new BDSMagnetStrength(*strength);
  auto  magnetOuterInfo = BDSComponentFactory::PrepareMagnetOuterInfo(element, angleIn, angleOut, yokeOnLeft);
  // set the name to the desired one rather than the one from the element
  magnetOuterInfo->name = name;
  
  G4Transform3D fieldTiltOffset = BDSComponentFactory::CreateFieldTransform(element);

  BDSIntegratorType intType = BDS::GetDipoleIntegratorType(integratorSet, element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       brho,
					       intType,
					       strengthCopy,
					       true,
					       fieldTiltOffset);

  auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, angleIn, angleOut);
  BDSMagnet* magnet = new BDSMagnet(BDSMagnetType::sectorbend,
				    name,
				    arcLength,
				    bpInfo,
				    magnetOuterInfo,
				    vacuumField,
				    -angle);
  
  return magnet;
}

BDSLine* BDS::BuildRBendLine(const Element*          element,
			     const Element*          prevElement,
			     const Element*          nextElement,
			     const G4double          brho,
			     BDSMagnetStrength*      st,
			     const BDSIntegratorSet* integratorSet)
{
  const G4String name = element->name;
  BDSLine* rbendline  = new BDSLine(name); // line for resultant rbend
  
  const G4bool          includeFringe = BDSGlobalConstants::Instance()->IncludeFringeFields();
  const G4double thinElementArcLength = BDSGlobalConstants::Instance()->ThinElementLength();
  const G4bool             yokeOnLeft = BDSComponentFactory::YokeOnLeft(element, st);
  G4bool          buildFringeIncoming = includeFringe;
  G4bool          buildFringeOutgoing = includeFringe;
  
  // Angle here is in the 'strength' convention of +ve angle -> -ve x deflection
  const G4double       angle = (*st)["angle"];
  const G4double   arcLength = (*st)["length"];
  G4double     bendingRadius = DBL_MAX; // default for zero angle
  // Avoid bad calculations for zero angle. Here we need bending radius to be in
  // correct global cartesian convention, hence -ve.
  if (BDS::IsFinite(angle))
    {bendingRadius = - brho / (*st)["field"];}
  
  G4Transform3D fieldTiltOffset = BDSComponentFactory::CreateFieldTransform(element);

  // face rotations
  // convention - +ve e1 / e2 reduces outside of bend
  // extra factor of -1 for 'strength' to cartesian
  G4double factor = angle < 0 ? -1 : 1; 
  const G4double e1 = -factor * element->e1 * CLHEP::rad;
  const G4double e2 = -factor * element->e2 * CLHEP::rad;

  // don't build the fringe element if there's no face angle - no physical effect
  if (!BDS::IsFinite(e1))
    {buildFringeIncoming = false;}
  if (!BDS::IsFinite(e2))
    {buildFringeIncoming = false;}

  // default face angles for an rbend are 0 - ie parallel faces, plus any pole face rotation
  // angle in and out of total rbend are nominally the face angles.
  G4double angleIn  = e1;
  G4double angleOut = e2;
  
  if (prevElement && prevElement->type == ElementType::_RBEND)
    {
      buildFringeIncoming = false;
      // gives it a face perpendicular to the reference trajectory like an sbend
      // this prevents overlaps if the next one is tilted
      angleIn = 0.5 * angle;
    }
  if (nextElement && nextElement->type == ElementType::_RBEND)
    {
      buildFringeOutgoing = false;
      // gives it a face perpendicular to the reference trajectory like an sbend
      // this prevents overlaps if the next one is tilted
      angleOut = 0.5 * angle;
    }
  
  // if we're building the fringe elements, we reduce the length of the central section
  // and proportion the bending by their length w.r.t. the full length of the total rbend.
  // angles of the faces are in order:
  // angleIn / fringeInOutputAngle / centralInputFaceAngle / centralOutputFaceAngle
  // fringeOutInputAngle / angleOut

  G4double fringeInOutputAngle    = 0;
  G4double centralInputFaceAngle  = angleIn;
  G4double centralOutputFaceAngle = angleOut;
  G4double fringeOutInputAngle    = 0;
  G4double centralArcLength       = arcLength; // full length to start with
  G4double centralAngle           = angle;
  G4double oneFringeAngle         = 0;
  if (BDS::IsFinite(angle))
    {oneFringeAngle = (thinElementArcLength / arcLength) * angle;}
  
  if (buildFringeIncoming)
    {
      centralArcLength      -= thinElementArcLength;
      centralAngle          -= oneFringeAngle;
      angleIn                = e1 + (0.5*oneFringeAngle - 0.5*angle);
      fringeInOutputAngle    = - (e1 + (0.5*oneFringeAngle - 0.5*angle));
      centralInputFaceAngle  = e1;
    }
  if (buildFringeOutgoing)
    {
      centralArcLength       -= thinElementArcLength;
      centralAngle           -= oneFringeAngle;
      centralOutputFaceAngle = e2;
      fringeOutInputAngle    = - (e2 + (0.5*oneFringeAngle - 0.5*angle));
      angleOut               = e2 + (0.5*oneFringeAngle - 0.5*angle);
    }
  
  if (buildFringeIncoming)
    {
      
      BDSMagnetStrength* fringeStIn  = new BDSMagnetStrength(*st);
      (*fringeStIn)["polefaceangle"] = e1;
      (*fringeStIn)["length"]        = thinElementArcLength;
      (*fringeStIn)["angle"]         = oneFringeAngle;
      (*fringeStIn)["fringecorr"]    = CalculateFringeFieldCorrection(bendingRadius, e1, element->fint, element->hgap*CLHEP::m);
      G4String fringeName            = name + "_e1_fringe";

      // element used for beam pipe materials etc - not strength, angle or length.
      BDSMagnet* startfringe = BDS::BuildDipoleFringe(element, angleIn, fringeInOutputAngle,
						      fringeName,
						      fringeStIn, brho,
						      integratorSet);
      rbendline->AddComponent(startfringe);
    }
  
  // override copied length and angle
  (*st)["length"] = centralArcLength;
  (*st)["angle"]  = centralAngle;

  BDSIntegratorType intType = BDS::GetDipoleIntegratorType(integratorSet, element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       brho,
					       intType,
					       st,
					       true,
					       fieldTiltOffset);

  auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, centralInputFaceAngle, centralOutputFaceAngle);
  auto mgInfo = BDSComponentFactory::PrepareMagnetOuterInfo(element, centralInputFaceAngle, centralOutputFaceAngle, yokeOnLeft);
  mgInfo->name = element->name;

  // Here we change from the strength angle convention of +ve angle corresponds to
  // deflection in negative x, to correct 3d +ve angle corresponds to deflection in
  // positive x. Hence angle sign flip for construction.
  BDSMagnet* oneBend = new BDSMagnet(BDSMagnetType::rectangularbend,
				     element->name,
				     centralArcLength,
				     bpInfo,
				     mgInfo,
				     vacuumField,
				     -centralAngle,
				     nullptr);
  
  rbendline->AddComponent(oneBend);
  
  //Last element should be fringe if poleface specified
  if (buildFringeOutgoing)
    { 
      BDSMagnetStrength* fringeStOut  = new BDSMagnetStrength();
      (*fringeStOut)["field"]         = (*st)["field"];
      (*fringeStOut)["polefaceangle"] = e2;
      (*fringeStOut)["length"]        = thinElementArcLength;
      (*fringeStOut)["angle"]         = oneFringeAngle;
      (*fringeStOut)["fringecorr"]    = CalculateFringeFieldCorrection(bendingRadius, e2, element->fintx, element->hgap*CLHEP::m);
      G4String fringeName             = name + "_e2_fringe";
      
      BDSMagnet* endfringe = BDS::BuildDipoleFringe(element, fringeOutInputAngle, angleOut,
						    fringeName,
						    fringeStOut, brho,
						    integratorSet);
      rbendline->AddComponent(endfringe);
    }
  
  return rbendline;
}

BDSMagnet* BDS::BuildDipoleFringe(const GMAD::Element*     element,
				  G4double                 angleIn,
				  G4double                 angleOut,
				  G4String                 name,
				  const BDSMagnetStrength* st,
				  G4double                 brho,
				  const BDSIntegratorSet*  integratorSet)
{
  BDSBeamPipeInfo* beamPipeInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, angleIn, angleOut);
  beamPipeInfo->beamPipeType = BDSBeamPipeType::circularvacuum;
  auto magnetOuterInfo = BDSComponentFactory::PrepareMagnetOuterInfo(element, angleIn, angleOut);
  magnetOuterInfo->geometryType   = BDSMagnetGeometryType::none;
  magnetOuterInfo->name           = name;
  magnetOuterInfo->buildEndPieces = false;
  
  G4Transform3D fieldTiltOffset = BDSComponentFactory::CreateFieldTransform(element);

  BDSIntegratorType intType = integratorSet->dipolefringe;
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       brho,
					       intType,
					       st,
					       true,
					       fieldTiltOffset);

  return new BDSMagnet(BDSMagnetType::dipolefringe,
		       name,
		       (*st)["length"],
		       beamPipeInfo,
		       magnetOuterInfo,
		       vacuumField,
		       -(*st)["angle"],
		       nullptr);
}

G4int BDS::CalculateNSBendSegments(const G4double length,
				   const G4double angle,
				   const G4double e1,
				   const G4double e2,
				   const G4double aperturePrecision)
{
  // Split a bend into equal segments such that the maximum distance between the
  // chord and arc is 1mm.
  
  // from formula: L/2 / N tan (angle/N) < precision. (L=physical length)
  // add poleface rotations onto angle as absolute number (just to be safe)
  G4double totalAngle = std::abs(angle) + std::abs(e1) + std::abs(e2);
  G4int nSBends = (G4int) ceil(std::sqrt(length*totalAngle/2/aperturePrecision));
  if (nSBends==0)
    {nSBends = 1;} // can happen in case angle = 0
  if (BDSGlobalConstants::Instance()->DontSplitSBends())
    {nSBends = 1;}  // use for debugging
  if (nSBends % 2 == 0)
    {nSBends += 1;} // always have odd number of poles for poleface rotations
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " splitting sbend into " << nSBends << " sbends" << G4endl;
#endif
  return nSBends;
}

G4double BDS::CalculateFringeFieldCorrection(G4double rho,
					     G4double polefaceAngle,
					     G4double fint,
					     G4double hgap)
{
  G4double gOverRho = 2 * hgap / rho;
  G4double corrValue = fint * gOverRho * (1.0 + std::pow(sin(polefaceAngle),2)) / cos(polefaceAngle);
  return corrValue;
}

BDSIntegratorType BDS::GetDipoleIntegratorType(const BDSIntegratorSet* integratorSet,
                                               const Element*          element)
{
  // default is dipole integrator
  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::dipole);

  // check for finite k1 and change integrator type if needed
  if (BDS::IsFinite(element->k1))
    {intType = integratorSet->Integrator(BDSFieldType::dipolequadrupole);}

  return intType;
}