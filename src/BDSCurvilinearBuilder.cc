#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSCurvilinearBuilder.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSimpleComponent.hh"
#include "BDSTiltOffset.hh"
#include "BDSTunnelInfo.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"

#include <cmath>

BDSCurvilinearBuilder::BDSCurvilinearBuilder()
{
  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance(); // shortcut
  curvilinearRadius = globals->SamplerDiameter()*0.5;
  if (globals->BuildTunnel() || globals->BuildTunnelStraight())
    {// query the default tunnel model
      BDSExtent tunnelExtent = globals->TunnelInfo()->IndicativeExtent();
      tunnelExtent = tunnelExtent.Shift(globals->TunnelOffsetX(), globals->TunnelOffsetY());
      G4double maxTunnelR =  tunnelExtent.MaximumAbs();
      curvilinearRadius = std::max(curvilinearRadius, maxTunnelR);
    }
  checkOverlaps = globals->CheckOverlaps();
  lengthSafety  = globals->LengthSafety();
}

BDSCurvilinearBuilder::~BDSCurvilinearBuilder()
{;}

BDSBeamline* BDSCurvilinearBuilder::BuildCurvilinearBeamLine1To1(BDSBeamline const* const beamline)
{
  BDSBeamline* result = new BDSBeamline();
  for (const auto& element : *beamline)
    {
      BDSSimpleComponent* temp = BuildCurvilinearComponent(element);
      if (temp)
	{
	  BDSBeamlineElement* tempEl = BuildBeamLineElement(temp, element);
	  result->AddBeamlineElement(tempEl);
	}
    }
  return result;
}

BDSBeamline* BDSCurvilinearBuilder::BuildCurvilinearBeamLine(BDSBeamline const* const beamline)
{
  BDSBeamline* result = new BDSBeamline();

  G4double accumulatedArcLength = 0;
  G4double accumulatedAngle     = 0;
  G4bool   straightSoFar        = true;

  BDSBeamline::const_iterator startingElement  = beamline->begin();
  BDSBeamline::const_iterator finishingElement = beamline->begin();
  BDSBeamline::const_iterator nextElement      = beamline->begin() + 1;
  BDSBeamline::const_iterator currentElement   = beamline->begin();
  
  for (; currentElement != beamline->end(); currentElement++)
    {
      const G4bool   isStraight = IsStraight(*currentElement);
      const G4double arcLength  = (*currentElement)->GetArcLength();
      if (arcLength < 1*CLHEP::cm) // short element - avoid by accumulating
	{
	  //Accumulate(currentElement);
	}
      else if (isStraight && straightSoFar) // keep adding straight sections while straight
	{
	  // Accumulate(currentElement);
	}
      else
	{
	  //Break();
	}

      if (!isStraight)
	{straightSoFar = false;}

    }
  
  for (const auto& element : *beamline)
    {
      const G4double arcLength = element->GetArcLength();
      const G4double angle     = element->GetAngle();
      // if too short



    }
  
  return result;
}

G4bool BDSCurvilinearBuilder::IsStraight(BDSBeamlineElement const* const element)
{
  return !BDS::IsFinite(element->GetAngle());
}

BDSBeamlineElement* BDSCurvilinearBuilder::BuildBeamLineElement(BDSSimpleComponent* component,
								BDSBeamlineElement const* const element)
{
  BDSTiltOffset* copyTiltOffset = nullptr;
  BDSTiltOffset* existingTiltOffset = element->GetTiltOffset();
  if (existingTiltOffset)
    {copyTiltOffset = new BDSTiltOffset(*existingTiltOffset);}
  BDSBeamlineElement* result = new BDSBeamlineElement(component,
						      element->GetPositionStart(),
						      element->GetPositionMiddle(),
						      element->GetPositionEnd(),
						      new G4RotationMatrix(*(element->GetRotationStart())),
						      new G4RotationMatrix(*(element->GetRotationMiddle())),
						      new G4RotationMatrix(*(element->GetRotationEnd())),
						      element->GetReferencePositionStart(),
						      element->GetReferencePositionMiddle(),
						      element->GetReferencePositionEnd(),
						      new G4RotationMatrix(*(element->GetReferenceRotationStart())),
						      new G4RotationMatrix(*(element->GetReferenceRotationMiddle())),
						      new G4RotationMatrix(*(element->GetReferenceRotationEnd())),
						      element->GetSPositionStart(),
						      element->GetSPositionMiddle(),
						      element->GetSPositionEnd(),
						      copyTiltOffset);
  return result;
}

BDSSimpleComponent* BDSCurvilinearBuilder::BuildCurvilinearComponent(BDSBeamlineElement const* const element)
{
  G4double chordLength = element->GetChordLength();
  if (!BDS::IsFinite(chordLength))
    {return nullptr;}

  // don't build for certain types.
  const G4String elementType = element->GetType();
  if (elementType == "teleporter" || elementType == "terminator")
    {return nullptr;}

  G4double arcLength = element->GetArcLength();
  G4double     angle = element->GetAngle();
  G4String      name = element->GetName();

  G4VSolid* solid = nullptr;
  G4ThreeVector inputface  = G4ThreeVector(0, 0,-1);
  G4ThreeVector outputface = G4ThreeVector(0, 0, 1);
  G4double     radiusLocal = curvilinearRadius;
  if (!BDS::IsFinite(angle))
    { // angle is zero
      G4double halfLength = chordLength * 0.5 - lengthSafety;
      solid = new G4Tubs(name + "_cl_solid", // name
			 0,                  // inner radius
			 radiusLocal,        // outer radius
			 halfLength,         // z half width
			 0,                  // start angle
			 CLHEP::twopi);      // sweep angle
    }
  else
    {
      // angle is finite!
      // factor of 0.8 here is arbitrary tolerance as g4 cut tubs seems to fail
      // with cutting entranace / exit planes close to limit.
      // s = r*theta -> r = s/theta
      G4double radiusFromAngleLength =  std::abs(chordLength / angle) * 0.8;
      radiusLocal = std::min(curvilinearRadius, radiusFromAngleLength);
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "taking smaller of: sampler radius: " << curvilinearRadius
	     << " mm, max possible radius: " << radiusFromAngleLength << " mm" << G4endl;
#endif
      std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(-0.5*angle, -0.5*angle);
      inputface = faces.first;
      outputface = faces.second;

      BDSTiltOffset* to = element->GetTiltOffset();
      if (to)
	{// could be nullptr
	  G4double tilt = to->GetTilt();
	  if (BDS::IsFinite(tilt))
	    {// rotate normal faces
	      inputface = inputface.rotateZ(tilt);
	      outputface = outputface.rotateZ(tilt);
	    }
	}
      G4double halfLength = chordLength * 0.5 - lengthSafety;
      solid = new G4CutTubs(name + "_cl_solid", // name
			    0,                  // inner radius
			    radiusLocal,        // outer radius
			    halfLength,         // half length (z)
			    0,                  // rotation start angle
			    CLHEP::twopi,       // rotation sweep angle
			    inputface,          // input face normal vector
			    outputface);        // output face normal vector
    }

  // nullptr for material ONLY ok in parallel world!
  G4LogicalVolume* lv =  new G4LogicalVolume(solid,            // solid
					     nullptr,          // material
					     name + "_cl_lv"); // name

  // always debug visualisation for read out geometry - only viewed via explicit commands
  lv->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());

  G4ThreeVector inputFaceLocal  = BDS::RotateToReferenceFrame(inputface, angle);
  G4ThreeVector outputFaceLocal = BDS::RotateToReferenceFrame(outputface, -angle);
  
  BDSSimpleComponent* result = new BDSSimpleComponent(name + "_cl",
						      arcLength,
						      angle,
						      solid,
						      lv,
						      inputFaceLocal,
						      outputFaceLocal);

  BDSExtent extent = BDSExtent(radiusLocal, radiusLocal, chordLength*0.5);
  result->SetExtent(extent);
  return result;
}
  
