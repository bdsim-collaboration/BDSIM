#include "BDSBeamline.hh"
#include "BDSDebug.hh"
#include "BDSTiltOffset.hh"
#include "BDSTunnelBuilder.hh"
#include "BDSTunnelFactory.hh"
#include "BDSTunnelSegment.hh"
#include "BDSUtilities.hh"  // for is finite function

#include "globals.hh"

#include <sstream>
#include <utility>

BDSTunnelBuilder* BDSTunnelBuilder::_instance = 0;

BDSTunnelBuilder* BDSTunnelBuilder::Instance()
{
  if (_instance == 0)
    {_instance = new BDSTunnelBuilder();}
  return _instance;
}

BDSTunnelBuilder::BDSTunnelBuilder()
{
  displacementTolerance = 50*CLHEP::cm;   // maximum displacemenet of beamline before split
  maxItems              = 50;             // maximum number of items before split
  maxLength             = 50*CLHEP::m;    // maximum length of tunnel segment
  maxAngle              = 100*CLHEP::mrad; // maximum angle before split
}

BDSTunnelBuilder::~BDSTunnelBuilder()
{  
  _instance = 0;
}

G4bool BDSTunnelBuilder::BreakTunnel(G4double cumulativeLength,
				     G4double cumulativeAngle,
				     G4int    cumulativeNItems,
				     G4double cumulativeDisplacementX,
				     G4double cumulativeDisplacementY)
{
  G4bool result = false;

  G4bool lengthTest = false;
  if (cumulativeLength > maxLength)
    {lengthTest = true;}

  G4bool angleTest = false;
  if (cumulativeAngle > maxAngle)
    {angleTest = true;}

  G4bool nItemsTest = false;
  if (cumulativeNItems > maxItems)
    {nItemsTest = true;}

  G4bool dispXTest = false;
  if (cumulativeDisplacementX > displacementTolerance)
    {dispXTest = true;}

  G4bool dispYTest = false;
  if (cumulativeDisplacementY > displacementTolerance)
    {dispYTest = true;}

  result = lengthTest || angleTest || nItemsTest || dispXTest || dispYTest;

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "testing cumulative parameters" << G4endl;
  G4cout << "Cumulative Length (mm):    " << cumulativeLength << " > " << maxLength << " test-> " << lengthTest << G4endl;
  G4cout << "Cumulative Angle (rad):    " << cumulativeAngle  << " > " << maxAngle  << " test-> " << angleTest  << G4endl;
  G4cout << "# of items:                " << cumulativeNItems << " > " << maxItems  << " test-> " << nItemsTest << G4endl;
  G4cout << "Cumulative displacement X: " << cumulativeDisplacementX << " > " << displacementTolerance
	 << " test-> " << dispXTest << G4endl;
  G4cout << "Cumulative displacement Y: " << cumulativeDisplacementY << " > " << displacementTolerance
	 << " test-> " << dispYTest << G4endl;
  G4cout << "Result:                    " << result << G4endl;
#endif
  return result;
}

std::pair<BDSBeamline*,BDSBeamline*> BDSTunnelBuilder::BuildTunnelAndSupports(BDSBeamline* flatBeamline)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  
  BDSTunnelInfo* defaultModel = BDSGlobalConstants::Instance()->TunnelInfo();
  G4double       offsetX      = BDSGlobalConstants::Instance()->TunnelOffsetX();
  G4double       offsetY      = BDSGlobalConstants::Instance()->TunnelOffsetY();
  
  BDSBeamline* tunnelLine   = new BDSBeamline();
  BDSBeamline* supportsLine = new BDSBeamline();

  // temporary variables to use as we go along
  G4int    nTunnelSegments            = 0;
  G4double cumulativeLength           = 0; // integrated length since last tunnel break
  G4double cumulativeAngle            = 0; // integrated angle since last tunnel break
  G4int    cumulativeNItems           = 0; // integraed number of accelerator components since last tunnel break
  G4double cumulativeDisplacementX    = 0; // integrated offset from initial point - horizontal
  G4double cumulativeDisplacementY    = 0; // integrated offset from initial point - vertical
  BDSGeometryComponent* tunnelSegment = NULL;
  BDSTunnelFactory*     tf            = BDSTunnelFactory::Instance(); // shortcut

  // iterator to the BDSBeamlineElement where the previous tunnel section finished
  BDSBeamlineIterator previousEndElement = flatBeamline->begin();

  // iterator to the BDSBeamlineElement where the current tunnel section will begin
  BDSBeamlineIterator startElement       = flatBeamline->begin();

  // iterator to the BDSBeamlineElement where the cuurent tunnel section will end
  BDSBeamlineIterator endElement         = flatBeamline->begin();

  BDSBeamlineIterator it = flatBeamline->begin();
  for (; it != flatBeamline->end(); ++it)
    {
      G4bool isEnd   = it == (flatBeamline->end() - 1);
#ifdef BDSDEBUG
      if (isEnd)
	{G4cout << "End of beam line - forcing break in tunnel" << G4endl;}
#endif
      G4bool breakIt = BreakTunnel(cumulativeLength,
				   cumulativeAngle,
				   cumulativeNItems,
				   cumulativeDisplacementX,
				   cumulativeDisplacementY);
      if (breakIt || isEnd)
	{
	  // work out tunnel parameters
	  std::stringstream name;
	  name << "tunnel_" << nTunnelSegments;

	  // calculate start central point of tunnel
	  G4ThreeVector startPoint         = (*startElement)->GetReferencePositionStart();
	  G4ThreeVector startOffsetLocal   = G4ThreeVector(offsetX, offsetY, 0);
	  G4RotationMatrix* startRot       = (*startElement)->GetReferenceRotationStart();	  
	  G4ThreeVector startOffsetGlobal  = startOffsetLocal.transform(*startRot);
	  startPoint                      += startOffsetGlobal;

	  // calculate end central point of tunnel
	  G4ThreeVector endPoint           = (*endElement)->GetReferencePositionEnd();
	  G4ThreeVector endOffsetLocal     = G4ThreeVector(offsetX, offsetY, 0);
	  G4RotationMatrix* endRot         = (*endElement)->GetReferenceRotationEnd();
	  G4ThreeVector endOffsetGlobal    = endOffsetLocal.transform(*endRot);
	  endPoint                        += endOffsetGlobal;

	  // calculate length
	  G4double segmentLength = (endPoint - startPoint).mag();

	  // decide whether angled or not
	  G4bool isAngled = BDS::IsFinite(cumulativeAngle);

#ifdef BDSDEBUG
	  G4cout << __METHOD_NAME__ << "determined tunnel segment to have the following parameters:" << G4endl;
	  G4cout << "Start element name:   " << (*startElement)->GetName() << G4endl;
	  G4cout << "End element name:     " << (*endElement)->GetName()   << G4endl;
	  G4cout << "Start point (global): " << startPoint                 << G4endl;
	  G4cout << "End point (global):   " << endPoint                   << G4endl;
	  G4cout << "Has a finite angle:   " << isAngled                   << G4endl;
	  G4cout << "Segment length:       " << segmentLength              << G4endl;
	  G4cout << "Total angle:          " << cumulativeAngle            << G4endl;
#endif
	  
	  // create tunnel segment
	  if (isAngled)
	    { // use the angled faces
	      tunnelSegment = tf->CreateTunnelSectionAngledInOut(defaultModel->type,          // type
								 name.str(),                  // name
								 segmentLength,               // length
								 cumulativeAngle*0.5,         // input angle
								 cumulativeAngle*0.5,         // output angle
								 defaultModel->thickness,     // thickness
								 defaultModel->soilThickness, // soil thickness
								 defaultModel->material,      // material
								 defaultModel->soilMaterial,  // soil material
								 defaultModel->buildFloor,    // build floor?
								 defaultModel->floorOffset,   // floor offset
								 defaultModel->aper1,         // 1st aperture param
								 defaultModel->aper2,         // 2nd aperture param
								 defaultModel->visible);      // is it visible 
	    }
	  else
	    { // straight section
	      tunnelSegment = tf->CreateTunnelSection(defaultModel->type,          // type
						      name.str(),                  // name
						      segmentLength,               // length
						      defaultModel->thickness,     // thickness
						      defaultModel->soilThickness, // soil thickness
						      defaultModel->material,      // material
						      defaultModel->soilMaterial,  // soil material
						      defaultModel->buildFloor,    // build floor?
						      defaultModel->floorOffset,   // floor offset
						      defaultModel->aper1,         // 1st aperture param
						      defaultModel->aper2,         // 2nd aperture param
						      defaultModel->visible);      // is it visible
	    }
	  
	  // store segment in tunnel beam line
	  // set length correctly - for normal accelerator components, we get the
	  // arc length from the user, but in this case, we really start with the
	  // chord length.
	  G4double arcLength = segmentLength;
	  if (isAngled)
	    {
	      G4double radiusOfCurvature = (segmentLength * 0.5) / sin(cumulativeAngle*0.5);
	      G4double arcLength         = radiusOfCurvature * cumulativeAngle;
#ifdef BDSDEBUG
	      G4cout << __METHOD_NAME__ << "segment length: " << segmentLength << G4endl;
	      G4cout << __METHOD_NAME__ << "arc length:     " << arcLength     << G4endl;
#endif
	    }

	  BDSTunnelSegment* ts = new BDSTunnelSegment(name.str(),
						      arcLength,
						      cumulativeAngle,
						      tunnelSegment);

	  if (tunnelLine->empty())
	    {
	      BDSTiltOffset* tos = new BDSTiltOffset(offsetX,offsetY,0);
	      tunnelLine->AddComponent(ts,tos);
	    }
	  else
	    {tunnelLine->AddComponent(ts);} // append to tunnel beam line
	  
	  // update / reset counters & iterators
	  nTunnelSegments   += 1;
	  cumulativeLength   = 0;
	  cumulativeAngle    = 0;
	  cumulativeNItems   = 0;
	  startElement       = endElement; // next segment will begin where this one finishes
	  previousEndElement = endElement; // mark the end of thhis element as the prevous end
	}
      else
	{
	  cumulativeLength += (*it)->GetAcceleratorComponent()->GetChordLength();
	  cumulativeAngle  += (*it)->GetAcceleratorComponent()->GetAngle();
	  cumulativeNItems += 1;
	  endElement++; // advance the potential end element iterator
	}
    }
  return std::make_pair(supportsLine,tunnelLine);
}
