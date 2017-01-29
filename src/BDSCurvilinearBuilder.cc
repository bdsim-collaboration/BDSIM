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

  // lambda function to reset counters - avoids repetition. '[&]'
  // captures all variables required, by reference.
  auto Reset = [&](){
    accumulatedArcLength = 0;
    accumulatedAngle     = 0;
    straightSoFar        = false;
    currentTilt          = 0;
    startingElement      = currentElement + 1;
    finishingElement     = startingElement;
  };

  for (; currentElement != beamline->end(); currentElement++)
    {
      G4cout << "start:   " << (*startingElement)->GetPlacementName() << G4endl;
      G4cout << "current: " << (*currentElement)->GetPlacementName() << G4endl;
      G4cout << "finish:  " << (*finishingElement)->GetPlacementName() << G4endl << G4endl;
      
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
	  G4bool   signflip     = std::signbit(accumulatedAngle) != std::signbit(currentAngle);
	  if (straightSoFar || signflip)
	    {
	      // occurs if we've passed a straight element and now hit an angled one; or
	      // if the we're going from bending one way to another.
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
	  else if (tooShort)
	    {// Accumulate
	      if (tilted) // have to be careful about accumulating if we have tilts involved
		{// check tilt - should always have valid tilt instance in this case
		  BDSTiltOffset* to = (*currentElement)->GetTiltOffset();
		  G4double tilt = to->GetTilt();
		  
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
		      currentTilt = tilt;
		      tiltedSoFar = true;
		      Accumulate(*currentElement, accumulatedArcLength, accumulatedAngle, straightSoFar);
		      finishingElement = currentElement;
		    }
		}
	      else
		{
		  Accumulate(*currentElement, accumulatedArcLength, accumulatedAngle, straightSoFar);
		  finishingElement = currentElement;
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
      else // Accumulate all straight sections
	{
	  Accumulate(*currentElement, accumulatedArcLength, accumulatedAngle, straightSoFar);
	  finishingElement = currentElement;
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
      G4RotationMatrix* rotRefStart = new G4RotationMatrix(*((*startElement)->GetReferenceRotationStart()));
      G4RotationMatrix* rotRefEnd   = new G4RotationMatrix(*((*finishElement)->GetReferenceRotationEnd()));

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
