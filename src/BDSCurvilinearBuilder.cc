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
  checkOverlaps = globals->CheckOverlaps();
  lengthSafety  = globals->LengthSafety();
  minimumLength = 1*CLHEP::mm;

  factory = new BDSCurvilinearFactory();
}

BDSCurvilinearBuilder::~BDSCurvilinearBuilder()
{
  delete factory;
}

BDSBeamline* BDSCurvilinearBuilder::BuildCurvilinearBeamLine1To1(BDSBeamline const* const beamline)
{
  BDSBeamline* result = new BDSBeamline();
  G4int i = 0;
  for (BDSBeamline::const_iterator element = beamline->begin(); element != beamline->end(); element++)
    {
      if ((*element)->GetType() == "teleporter" || (*element)->GetType() == "terminator")
	{continue;}
      G4String name = (*element)->GetName() + "_cl_" + std::to_string(i);
      i++;
      BDSBeamlineElement* temp = CreateCurvilinearElement(name, element, element, i);
      if (temp)
	{result->AddBeamlineElement(temp);}
    }
  return result;
}

BDSBeamline* BDSCurvilinearBuilder::BuildCurvilinearBeamLine(BDSBeamline const* const beamline)
{
  BDSBeamline* result = new BDSBeamline();
  // Get the padding length from the beam line class
  paddingLength = beamline->PaddingLength();

  // Although elements will be premanufactured here and added to a beam line without
  // using the AddElement method that calculates the coordinates. We want the curvilinear
  // world to be slightly bigger than each element, hence the padding in the mass world,
  // and so we expand the length of each curvilinear element.
  
  G4int    counter              = 0; // counter for naming

  G4double accumulatedAngle     = 0;
  G4bool   straightSoFar        = false;
  G4double currentTilt          = 0;
  G4bool   tiltedSoFar          = false;
  
  BDSBeamline::const_iterator startingElement  = beamline->begin();
  BDSBeamline::const_iterator finishingElement = beamline->begin();
  BDSBeamline::const_iterator currentElement   = beamline->begin();

  // lambda function to reset counters - avoids repetition. '[&]'
  // captures all variables required, by reference.
  auto Reset = [&](){
    accumulatedAngle     = (*currentElement)->GetAngle();
    straightSoFar        = false;
    currentTilt          = (*currentElement)->GetTilt();
    startingElement      = currentElement;
    finishingElement     = startingElement;
  };

  G4String name = "";
  auto GenerateName = [&](){name = "cl_" + std::to_string(counter);};

  for (; currentElement != beamline->end(); currentElement++)
    {
      // update name in one place although not needed every loop iteration
      GenerateName();
      
      const G4bool angled   = Angled(*currentElement);
      const G4bool tooShort = TooShort(*currentElement);
      const G4bool tilted   = BDS::IsFinite((*currentElement)->GetTilt());
      const G4bool last     = *currentElement == beamline->back();

      if (angled)
	{
	  G4double currentAngle = (*currentElement)->GetAngle();
	  G4bool   signflip     = std::signbit(accumulatedAngle) != std::signbit(currentAngle);
	  if (straightSoFar || signflip)
	    {
	      // occurs if we've passed a straight element and now hit an angled one; or
	      // if we're going from bending one way to another.
	      // make from startingElement to currentElement-1 so only straight components
	      // this means we keep the best accuracy for coordinates for the straight section
	      //finishingElement = currentElement - 1;
	      BDSBeamlineElement* piece = CreateCurvilinearElement(name,
								   startingElement,
								   finishingElement,
								   counter);
	      result->AddBeamlineElement(piece);
	      counter++; // increment name counter
	      GenerateName();
	      Reset();
	    }
	  if (tooShort)
	    {// Accumulate
	      if (tilted) // have to be careful about accumulating if we have tilts involved
		{// check tilt
		  G4double tilt = (*currentElement)->GetTilt();
		  
		  if (!tiltedSoFar || BDS::IsFinite(std::abs(tilt - currentTilt)))
		    {// change in tilt - build up to this one
		      //finishingElement = currentElement - 1;
		      BDSBeamlineElement* piece = CreateCurvilinearElement(name,
									   startingElement,
									   finishingElement,
									   counter);
		      result->AddBeamlineElement(piece);
		      counter++; // increment name counter
		      Reset();
		    }
		  else
		    {
		      Accumulate(*currentElement, accumulatedAngle, straightSoFar);
		      finishingElement = currentElement;
		    }
		  // update tilt knowledge in both cases - do after Reset()
		  currentTilt = tilt;
		  tiltedSoFar = true; // now tilted
		}
	      else
		{
		  Accumulate(*currentElement, accumulatedAngle, straightSoFar);
		  finishingElement = currentElement;
		}
	      if (last)
		{
		  BDSBeamlineElement* piece = CreateCurvilinearElement(name,
								       startingElement,
								       currentElement,
								       counter);
		  result->AddBeamlineElement(piece);
		}
	    }
	  else
	    {// we're building 1:1 the current angled element
	      BDSBeamlineElement* piece = CreateCurvilinearElement(name,
								   currentElement,
								   currentElement,
								   counter);
	      result->AddBeamlineElement(piece);
	      counter++; // increment name counter
	      Reset();
	    }
	}
      else
	{// Accumulate all straight sections
	  if (!straightSoFar && (currentElement != beamline->begin()))
	    {Reset();}
	  Accumulate(*currentElement, accumulatedAngle, straightSoFar);
	  finishingElement = currentElement;
	  if (last)
	    {
	      BDSBeamlineElement* piece = CreateCurvilinearElement(name,
								   startingElement,
								   currentElement,
								   counter);
	      result->AddBeamlineElement(piece);
	    }
	}
    }
  
  return result;
}

void BDSCurvilinearBuilder::Accumulate(BDSBeamlineElement const* const element,
				       G4double& accumulatedAngle,
				       G4bool&   straightSoFar) const
{
  G4double angle    = element->GetAngle();
  accumulatedAngle += angle;
  straightSoFar     = !Angled(element);
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
      if (Angled(*startElement)) {
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
      if (BDS::IsFinite(angle)) {
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
