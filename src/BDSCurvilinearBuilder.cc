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
      G4double maxTunnelR =  tunnelExtent.MaximumAbs();
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
      const G4bool angled   = Angled(*currentElement);
      const G4bool tooShort = TooShort(*currentElement);

      if (angled)
	{
	  if (straightSoFar)
	    {
	      // make from startingElement to currentElement-1
	    }
	  if (tooShort)
	    {
	      // Accumulate
	    }

	}
      else
	{// straight
	  if (tooShort)
	    {
	      // Accumulate
	    }

	}

    }
  
  return result;
}

BDSBeamlineElement* BDSCurvilinearBuilder::BuildCurvilinearElement(BDSBeamlineElement const* const startElement,
								   BDSBeamlineElement const* const finishElement) const
{
  if (startElement == finishElement)
    {// build 1:1
      ;
    }
  else
    {// cover a few components
      ;
    }
  
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
