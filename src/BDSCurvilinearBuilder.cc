#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSExtent.hh"
#include "BDSCurvilinearBuilder.hh"
#include "BDSCurvilinearFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSimpleComponent.hh"
#include "BDSTiltOffset.hh"
#include "BDSTunnelInfo.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals

#include <cmath>

BDSCurvilinearBuilder::BDSCurvilinearBuilder()
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
  for (BDSBeamline::const_iterator element = beamline->begin(); element != beamline->end(); element++)
    {
      G4String name = (*element)->GetName() + "_cl";
      BDSBeamlineElement* temp = CreateCurvilinearElement(name, element, element);
      if (temp)
	{result->AddBeamlineElement(temp);}
    }
  return result;
}

BDSBeamline* BDSCurvilinearBuilder::BuildCurvilinearBeamLine(BDSBeamline const* const beamline)
{
  BDSBeamline* result = new BDSBeamline();

  G4int    counter              = 0; // counter for naming

  G4double accumulatedAngle     = 0;
  G4bool   straightSoFar        = true;
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

  for (; currentElement != beamline->end(); currentElement++)
    {
      if (currentElement == beamline->begin())
	{straightSoFar = false;}
      
      // update name in one place although not needed every loop iteration
      G4String name = "cl_" + std::to_string(counter);
      
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
								   finishingElement);
	      result->AddBeamlineElement(piece);
	      counter++; // increment name counter
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
									   finishingElement);
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
								       currentElement);
		  result->AddBeamlineElement(piece);
		}
	    }
	  else
	    {// we're building 1:1 the current angled element
	      BDSBeamlineElement* piece = CreateCurvilinearElement(name,
								   currentElement,
								   currentElement);
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
								   currentElement);
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
  straightSoFar     = !BDS::IsFinite(angle);
}

BDSBeamlineElement* BDSCurvilinearBuilder::CreateCurvilinearElement(G4String                    elementName,
								    BDSBeamline::const_iterator startElement,
								    BDSBeamline::const_iterator finishElement)
{ 
  BDSSimpleComponent* component = nullptr;

  // we'll take the tilt from the first element - they should only ever be the same when used here
  G4bool tilted = BDS::IsFinite((*startElement)->GetTilt());
  
  if (startElement == finishElement)
    {// build 1:1
      G4double chordLength = (*startElement)->GetChordLength();
      G4double angle       = (*startElement)->GetAngle();
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
	  
	  G4double arcLength = (*startElement)->GetArcLength();
	  component = factory->CreateCurvilinearVolume(elementName,
						       arcLength,
						       chordLength,
						       curvilinearRadius,
						       angle,
						       to);
	}
    }
  else
    {// cover a few components
      G4ThreeVector positionStart = (*startElement)->GetReferencePositionStart();
      G4ThreeVector positionEnd   = (*finishElement)->GetReferencePositionEnd();
      G4double      chordLength   = (positionEnd - positionStart).mag();
      
      G4double accumulatedAngle = 0;
      for (auto it = startElement; it < finishElement; it++)
	{accumulatedAngle += (*it)->GetAngle();}
      
      if (!BDS::IsFinite(accumulatedAngle))
	{
	  component = factory->CreateCurvilinearVolume(elementName,
						       chordLength,
						       curvilinearRadius);
	}
      else
	{// angled - tilt matters
	  BDSTiltOffset* to = nullptr;
	  if (tilted)
	    {to = (*startElement)->GetTiltOffset();}

	  G4double meanBendingRadius = 0.5 * chordLength / sin(0.5*std::abs(accumulatedAngle));
	  G4double arcLength = meanBendingRadius * std::abs(accumulatedAngle);
	  
	  component = factory->CreateCurvilinearVolume(elementName,
						       arcLength,
						       chordLength,
						       curvilinearRadius,
						       accumulatedAngle,
						       to);	  
	}
    }

  return CreateElementFromComponent(component, startElement, finishElement);
}

BDSBeamlineElement* BDSCurvilinearBuilder::CreateElementFromComponent(BDSSimpleComponent* component,
								      BDSBeamline::const_iterator startElement,
								      BDSBeamline::const_iterator finishElement)
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
				      copyTiltOffset);
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
				      copyTiltOffset);
      
    }
  
  return result;
}
