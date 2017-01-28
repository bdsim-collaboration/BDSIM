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
	  counter++; // increment name counter
	}
      else if (angled)
	{
	  if (straightSoFar)
	    {// only occurs if we've passed a straight element and now hit an angled one
	      // make from startingElement to currentElement-1 so only straight components
	      // this means we keep the best accuracy for coordinates for the straight section
	      finishingElement = currentElement;
	      finishingElement--;
	      BDSBeamlineElement* piece = CreateCurvilinearElement(name,
								   startingElement,
								   finishingElement);
	      result->AddBeamlineElement(piece);
	      counter++; // increment name counter

	      // reset
	      accumulatedArcLength = 0;
	      accumulatedAngle     = 0;
	      straightSoFar        = false;
	      currentTilt          = 0;
	      tiltedSoFar          = false;
	      startingElement      = currentElement;
	      startingElement++; // start from next element
	    }
	  else if (tooShort)
	    {// Accumulate
	      if (tilted)
		{// check tilt
		  // should always be valid
		  BDSTiltOffset* to = (*currentElement)->GetTiltOffset();
		  G4double tilt = to->GetTilt();
		  
		  if (!tiltedSoFar || BDS::IsFinite(std::abs(tilt - currentTilt)))
		    {// change in tilt - build up to this one
		      finishingElement = currentElement;
		      finishingElement--;
		      BDSBeamlineElement* piece = CreateCurvilinearElement(name,
									   startingElement,
									   finishingElement);
		      result->AddBeamlineElement(piece);
		      counter++; // increment name counter
		      
		      // reset
		      accumulatedArcLength = 0;
		      accumulatedAngle     = 0;
		      straightSoFar        = false;
		      currentTilt          = 0;
		      startingElement      = currentElement;
		      startingElement++; // start from next element
		    }
		  else
		    {
		      currentTilt = tilt;
		      tiltedSoFar = true;
		      Accumulate(*currentElement, accumulatedArcLength, accumulatedAngle, straightSoFar);}
		}
	      else
		{Accumulate(*currentElement, accumulatedArcLength, accumulatedAngle, straightSoFar);}
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
  
  if (startElement == finishElement)
    {// build 1:1
      G4double arcLength = (*startElement)->GetArcLength();
      G4double angle     = (*startElement)->GetAngle();
      component = factory->CreateCurvilinearVolume(elementName,
						   arcLength,
						   curvilinearRadius);
    }
  else
    {// cover a few components
      G4double accumulatedArcLength = 0;
      G4double accumulatedAngle     = 0;
      G4bool   straightSoFar        = false; // dummy variable to use accumualte function
      for (BDSBeamline::const_iterator currentElement = startElement; currentElement != finishElement; currentElement++)
	{Accumulate(*currentElement, accumulatedArcLength, accumulatedAngle, straightSoFar);}
      
      if (!BDS::IsFinite(accumulatedAngle))
	{
	  component = factory->CreateCurvilinearVolume(elementName,
						       accumulatedArcLength,
						       curvilinearRadius);
	}
      else
	{// angled
	  G4bool tilted = false;
	  BDSTiltOffset* accumulatedTiltOffset = nullptr;
	  //component = factory->CreateCurvilinearVolume(
	  
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
