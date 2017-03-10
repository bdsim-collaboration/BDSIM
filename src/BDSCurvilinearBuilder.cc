#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSExtent.hh"
#include "BDSCurvilinearBuilder.hh"
#include "BDSCurvilinearFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSimpleComponent.hh"
#include "BDSSamplerType.hh"
#include "BDSTiltOffset.hh"
#include "BDSTunnelInfo.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals

#include <cmath>

BDSCurvilinearBuilder::BDSCurvilinearBuilder():
  paddingLength(0)
{
  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance(); // shortcut
  curvilinearRadius = globals->SamplerDiameter()*0.5;
  if (globals->BuildTunnel() || globals->BuildTunnelStraight())
    {// query the default tunnel model
      BDSExtent tunnelExtent = globals->TunnelInfo()->IndicativeExtent();
      tunnelExtent = tunnelExtent.Shift(globals->TunnelOffsetX(), globals->TunnelOffsetY());
      G4double maxTunnelR = tunnelExtent.MaximumAbs();
      curvilinearRadius = std::max(curvilinearRadius, maxTunnelR);
    }
  checkOverlaps    = globals->CheckOverlaps();
  lengthSafety     = globals->LengthSafety();
  minimumLength    = 1*CLHEP::mm;
  bonusChordLength = 1*CLHEP::m;

  factory = new BDSCurvilinearFactory();
}

BDSCurvilinearBuilder::~BDSCurvilinearBuilder()
{
  delete factory;
}

BDSBeamline* BDSCurvilinearBuilder::BuildCurvilinearBeamLine1To1(BDSBeamline const* const beamline,
								 const G4bool circular)
{
  paddingLength = beamline->PaddingLength();
  
  BDSBeamline* result = new BDSBeamline();

  if (!circular)
    {//prepend small section to machine
      BDSBeamlineElement* bonusBit = CreateBonusSectionStart(beamline);
      result->AddBeamlineElement(bonusBit);
    }
  
  G4int i = 0;
  for (BDSBeamline::const_iterator element = beamline->begin(); element != beamline->end(); element++)
    {
      G4String name = (*element)->GetName() + "_cl_" + std::to_string(i);
      i++;
      BDSBeamlineElement* temp = CreateCurvilinearElement(name, element, element, i);
      if (temp)
	{result->AddBeamlineElement(temp);}
    }

  if (!circular)
    {// append small section to machine
      BDSBeamlineElement* bonusBit = CreateBonusSectionEnd(beamline);
      if (bonusBit)
	{
	  result->AddBeamlineElement(bonusBit);}
    }
  return result;
}

BDSBeamlineElement* BDSCurvilinearBuilder::CreateCurvilinearElement(G4String                    elementName,
								    BDSBeamline::const_iterator startElement,
								    BDSBeamline::const_iterator finishElement,
								    G4int index)
{ 
  BDSSimpleComponent* component = nullptr;

  // we'll take the tilt from the first element - they should only ever be the same when used here
  G4bool tilted = BDS::IsFinite((*startElement)->GetTilt());
  // variables to be defined to create component
  G4double chordLength=0.0, angle=0.0, arcLength=0.0;
  
  if (startElement == finishElement)
    {// build 1:1
      chordLength = (*startElement)->GetChordLength() + paddingLength;
      angle       = (*startElement)->GetAngle();
      if (Angled(*startElement))
	{
	  // Not strictly accurate to add on paddingLength to arcLength, but close for now.
	  arcLength = (*startElement)->GetArcLength() + paddingLength;
	}
    }
  else
    {// cover a few components
      G4ThreeVector positionStart = (*startElement)->GetReferencePositionStart();
      G4ThreeVector positionEnd   = (*finishElement)->GetReferencePositionEnd();
      chordLength                 = (positionEnd - positionStart).mag() + paddingLength;
      
      G4double accumulatedAngle = 0;
      for (auto it = startElement; it < finishElement; it++)
	{accumulatedAngle += (*it)->GetAngle();}
      
      angle = accumulatedAngle;
      if (BDS::IsFinite(angle))
	{
	  G4double meanBendingRadius = 0.5 * chordLength / sin(0.5*std::abs(angle));
	  arcLength                  = meanBendingRadius * std::abs(angle);
	}
    }

  if (!BDS::IsFinite(angle))
    {// straight
      component = factory->CreateCurvilinearVolume(elementName,
						   chordLength,
						   curvilinearRadius);
    }
  else
    {// angled - tilt matters
      BDSTiltOffset* to = nullptr;
      if (tilted)
	{to = (*startElement)->GetTiltOffset();}
      
      component = factory->CreateCurvilinearVolume(elementName,
						   arcLength,
						   chordLength,
						   curvilinearRadius,
						   angle,
						   to);
    }

  return CreateElementFromComponent(component, startElement, finishElement, index);
}

BDSBeamlineElement* BDSCurvilinearBuilder::CreateElementFromComponent(BDSSimpleComponent* component,
								      BDSBeamline::const_iterator startElement,
								      BDSBeamline::const_iterator finishElement,
								      G4int index)
{
  BDSTiltOffset* copyTiltOffset = nullptr;
  BDSTiltOffset* existingTiltOffset = (*startElement)->GetTiltOffset();
  if (existingTiltOffset)
    {copyTiltOffset = new BDSTiltOffset(*existingTiltOffset);}

  BDSBeamlineElement* result = nullptr;
  
  if (startElement == finishElement)
    {// 1:1
      BDSBeamlineElement* element = *startElement; // convenience
      
      result = new BDSBeamlineElement(component,
				      element->GetReferencePositionStart(),
				      element->GetReferencePositionMiddle(),
				      element->GetReferencePositionEnd(),
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
				      BDSSamplerType::none,
				      "",
				      index);
    }
  else
    {//must cover a few components
      G4double      sStart      = (*startElement)->GetSPositionStart();
      G4double      sEnd        = (*finishElement)->GetSPositionEnd();
      G4double      sMid        = 0.5 * (sEnd + sStart);
      G4ThreeVector posRefStart = (*startElement)->GetReferencePositionStart();
      G4ThreeVector posRefEnd   = (*finishElement)->GetReferencePositionEnd();
      G4ThreeVector posRefMid   = 0.5 * (posRefStart + posRefEnd);
      G4RotationMatrix* rotRefStart = (*startElement)->GetReferenceRotationStart();
      G4RotationMatrix* rotRefEnd   = (*finishElement)->GetReferenceRotationEnd();

      G4ThreeVector delta = posRefMid - posRefStart;
      G4ThreeVector newUnitZ      = delta.unit();
      G4ThreeVector unitXPrevious = G4ThreeVector(1,0,0).transform(*rotRefStart);
      G4ThreeVector newUnitY      = newUnitZ.cross(unitXPrevious).unit();
      G4ThreeVector unitYPrevious = G4ThreeVector(0,1,0).transform(*rotRefStart);
      G4ThreeVector newUnitX      = unitYPrevious.cross(newUnitZ).unit();

      // create mid point rotation matrix from unit vectors at mid point
      G4RotationMatrix rotRefMid = G4RotationMatrix(newUnitX, newUnitY, newUnitZ);

      result = new BDSBeamlineElement(component,
				      posRefStart,
				      posRefMid,
				      posRefEnd,
				      new G4RotationMatrix(*rotRefStart),
				      new G4RotationMatrix(rotRefMid),
				      new G4RotationMatrix(*rotRefEnd),
				      posRefStart,
				      posRefMid,
				      posRefEnd,
				      new G4RotationMatrix(*rotRefStart),
				      new G4RotationMatrix(rotRefMid),
				      new G4RotationMatrix(*rotRefEnd),
				      sStart,
				      sMid,
				      sEnd,
				      copyTiltOffset,
				      BDSSamplerType::none,
				      "",
				      index);
      
    }
  
  return result;
}


BDSBeamlineElement* BDSCurvilinearBuilder::CreateBonusSectionStart(BDSBeamline const* const beamline)
{  
  // we're ingnoring any possible angled face of the curvilinear geometry
  BDSSimpleComponent* component = factory->CreateCurvilinearVolume("cl_start",
								   bonusChordLength,
								   curvilinearRadius);

  const BDSBeamlineElement* firstElement = beamline->GetFirstItem();
  const G4RotationMatrix*   rotStart     = firstElement->GetReferenceRotationStart();

  G4ThreeVector unitDir = G4ThreeVector(0,0,1).transform(*rotStart);
  G4ThreeVector endPos  = firstElement->GetReferencePositionStart() - unitDir*0.5*paddingLength;
  G4ThreeVector midPos  = endPos - unitDir*0.5*bonusChordLength;
  G4ThreeVector staPos  = endPos - unitDir*bonusChordLength;

  G4double sStart = firstElement->GetSPositionStart();

  BDSBeamlineElement*result = new BDSBeamlineElement(component,
						     staPos,
						     midPos,
						     endPos,
						     new G4RotationMatrix(*rotStart),
						     new G4RotationMatrix(*rotStart),
						     new G4RotationMatrix(*rotStart),
						     staPos,
						     midPos,
						     endPos,
						     new G4RotationMatrix(*rotStart),
						     new G4RotationMatrix(*rotStart),
						     new G4RotationMatrix(*rotStart),
						     sStart - bonusChordLength,
						     sStart - 0.5*bonusChordLength,
						     sStart,
						     nullptr,
						     BDSSamplerType::none,
						     "",
						     -1); // artificial index of -1 for before beam line
  return result;
}

BDSBeamlineElement* BDSCurvilinearBuilder::CreateBonusSectionEnd(BDSBeamline const* const beamline)
{
  // we're ingnoring any possible angled face of the curvilinear geometry
  BDSSimpleComponent* component = factory->CreateCurvilinearVolume("cl_end",
								   bonusChordLength,
								   curvilinearRadius);

  const BDSBeamlineElement* lastElement = beamline->GetLastItem();
  const G4RotationMatrix*   rotEnd      = lastElement->GetReferenceRotationEnd();
  const G4int lastIndex = lastElement->GetIndex();

  G4ThreeVector unitDir = G4ThreeVector(0,0,1).transform(*rotEnd);
  G4ThreeVector staPos  = lastElement->GetReferencePositionEnd() + unitDir*0.5*paddingLength;
  G4ThreeVector midPos  = staPos + unitDir*0.5*bonusChordLength;
  G4ThreeVector endPos  = staPos + unitDir*bonusChordLength;

  G4double sStart = lastElement->GetSPositionEnd();

  BDSBeamlineElement*result = new BDSBeamlineElement(component,
						     staPos,
						     midPos,
						     endPos,
						     new G4RotationMatrix(*rotEnd),
						     new G4RotationMatrix(*rotEnd),
						     new G4RotationMatrix(*rotEnd),
						     staPos,
						     midPos,
						     endPos,
						     new G4RotationMatrix(*rotEnd),
						     new G4RotationMatrix(*rotEnd),
						     new G4RotationMatrix(*rotEnd),
						     sStart,
						     sStart + 0.5*bonusChordLength,
						     sStart + bonusChordLength,
						     nullptr,
						     BDSSamplerType::none,
						     "",
						     lastIndex + 1); // artificial index of -1 for before beam line
  return result;

}
