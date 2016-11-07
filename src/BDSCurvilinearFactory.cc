#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSCurvilinearFactory.hh"
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
#include "G4VSolid.hh"

#include <cmath>

BDSCurvilinearFactory* BDSCurvilinearFactory::instance = nullptr;

BDSCurvilinearFactory* BDSCurvilinearFactory::Instance()
{
  if (!instance)
    {instance = new BDSCurvilinearFactory();}
  return instance;
}

BDSCurvilinearFactory::BDSCurvilinearFactory()
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
}

BDSCurvilinearFactory::~BDSCurvilinearFactory()
{
  instance = nullptr;
}

BDSBeamline* BDSCurvilinearFactory::BuildCurvilinearBeamLine(BDSBeamline const* const beamline)
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

BDSBeamlineElement* BDSCurvilinearFactory::BuildBeamLineElement(BDSSimpleComponent* component,
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
						      copyTiltOffset,
						      element->GetSamplerType(),
						      element->GetSamplerName(),
						      element->GetIndex());
  return result;
}

BDSSimpleComponent* BDSCurvilinearFactory::BuildCurvilinearComponent(BDSBeamlineElement const* const element)
{
  G4double chordLength = element->GetChordLength();
  if (!BDS::IsFinite(chordLength))
    {return nullptr;}

  G4double arcLength = element->GetArcLength();
  G4double     angle = element->GetAngle();
  G4String      name = element->GetName();

  G4VSolid* solid = nullptr;
  G4ThreeVector inputface  = G4ThreeVector(0, 0,-1);
  G4ThreeVector outputface = G4ThreeVector(0, 0, 1);
  G4double     radiusLocal = curvilinearRadius;
  if (!BDS::IsFinite(angle))
    {
      //angle is zero - build a box
      solid = new G4Box(name + "_cl_solid", // name
			radiusLocal,        // x half width
			radiusLocal,        // y half width
			chordLength*0.5);   // z half width
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

      solid = new G4CutTubs(name + "_cl_solid", // name
			    0,                  // inner radius
			    radiusLocal,        // outer radius
			    chordLength*0.5,    // half length (z)
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
  
