#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSCurvilinearBuilder.hh"
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
  
  G4double accumulatedArcLength = 0;
  G4double accumulatedAngle     = 0;
  G4bool   straightSoFar        = false;
  G4double currentTilt          = 0;
  G4bool   tiltedSoFar          = false;
  
  BDSBeamline::const_iterator startingElement  = beamline->begin();
  BDSBeamline::const_iterator finishingElement = beamline->begin();
  BDSBeamline::const_iterator currentElement   = beamline->begin();

  // lambda function to reset counters - avoids repetition. '[&]' capture all by reference as required
  auto Reset = [&](){
    accumulatedArcLength = 0;
    accumulatedAngle     = 0;
    straightSoFar        = false;
    currentTilt          = 0;
    startingElement      = currentElement + 1;
  };

  for (; currentElement != beamline->end(); currentElement++)
    {
      // update name in one place although not needed every loop iteration
      G4String name = "cl_" + std::to_string(counter);
      
      const G4bool angled   = Angled(*currentElement);
      const G4bool tooShort = TooShort(*currentElement);
      const G4bool tilted   = Tilted(*currentElement);
      
      if (*currentElement == beamline->back())
	{
	  finishingElement = currentElement;
	  BDSBeamlineElement* piece = CreateCurvilinearElement(name,
							       startingElement,
							       finishingElement);
	  result->AddBeamlineElement(piece);
	  // don't bother incrementing or resetting as it's the end of the line
	}
      else if (angled)
	{
	  G4double currentAngle = (*currentElement)->GetAngle();
	  G4bool signflip = std::signbit(accumulatedAngle) != std::signbit(currentAngle);
	  if (straightSoFar || signflip)
	    {
	      // occurs if we've passed a straight element and now hit an angled one; or
	      // if the we're going from bending one way to another.
	      // make from startingElement to currentElement-1 so only straight components
	      // this means we keep the best accuracy for coordinates for the straight section
	      finishingElement = currentElement - 1;
	      BDSBeamlineElement* piece = CreateCurvilinearElement(name,
								   startingElement,
								   finishingElement);
	      result->AddBeamlineElement(piece);
	      counter++; // increment name counter
	      Reset();
	    }
	  else if (tooShort)
	    {// Accumulate
	      if (tilted) // have to be careful about accumulating if we have tilts involved
		{// check tilt - should always have valid tilt instance in this case
		  BDSTiltOffset* to = (*currentElement)->GetTiltOffset();
		  G4double tilt = to->GetTilt();
		  
		  if (!tiltedSoFar || BDS::IsFinite(std::abs(tilt - currentTilt)))
		    {// change in tilt - build up to this one
		      finishingElement = currentElement - 1;
		      BDSBeamlineElement* piece = CreateCurvilinearElement(name,
									   startingElement,
									   finishingElement);
		      result->AddBeamlineElement(piece);
		      counter++; // increment name counter
		      Reset();
		    }
		  else
		    {
		      currentTilt = tilt;
		      tiltedSoFar = true;
		      Accumulate(*currentElement, accumulatedArcLength, accumulatedAngle, straightSoFar);
		    }
		}
	      else
		{Accumulate(*currentElement, accumulatedArcLength, accumulatedAngle, straightSoFar);}
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
	  Accumulate(*currentElement, accumulatedArcLength, accumulatedAngle, straightSoFar);
	}
    }
  
  return result;
}

G4bool BDSCurvilinearBuilder::Tilted(BDSBeamlineElement const* const element) const
{
  BDSTiltOffset* to = element->GetTiltOffset();
  if (to)
    {
      G4double t = to->GetTilt();
      if (BDS::IsFinite(t))
	{return true;}
      else
	{return false;}
    }
  else
    {return false;}
}

void BDSCurvilinearBuilder::Accumulate(BDSBeamlineElement const* const element,
				       G4double& accumulatedArcLength,
				       G4double& accumulatedAngle,
				       G4bool&   straightSoFar) const
{
  G4double angle        = element->GetAngle();
  accumulatedArcLength += element->GetArcLength();
  accumulatedAngle     += angle;
  if (BDS::IsFinite(angle))
    {straightSoFar = false;}
  else
    {straightSoFar = true;}
}

BDSBeamlineElement* BDSCurvilinearBuilder::CreateCurvilinearElement(G4String                    elementName,
								    BDSBeamline::const_iterator startElement,
								    BDSBeamline::const_iterator finishElement)
{
  BDSSimpleComponent* component = nullptr;

  // we'll take the tilt from the first element - they should only ever be the same when used here
  G4bool tilted = Tilted(*startElement);
  
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
      G4double chordLength = (positionEnd - positionStart).mag();
      
      G4double accumulatedAngle     = 0;
      for (auto currentElement = startElement; currentElement != finishElement; currentElement++)
	{accumulatedAngle += (*currentElement)->GetAngle();}
      
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

	  G4double meanBendingRadius = 0.5 * chordLength / sin(0.5*accumulatedAngle);
	  G4double arcLength = meanBendingRadius * accumulatedAngle;
	  
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
  return nullptr;
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

BDSSimpleComponent* BDSCurvilinearBuilder::BuildCurvilinearComponent(BDSBeamlineElement const* const element) const
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

  if (!BDS::IsFinite(angle))
    {// straight section
      return factory->CreateCurvilinearVolume(name,
					      chordLength,
					      curvilinearRadius);
    }
  else
    {// angled section
      return factory->CreateCurvilinearVolume(name,
					      arcLength,
					      chordLength,
					      curvilinearRadius,
					      angle,
					      element->GetTiltOffset());
    }
}
