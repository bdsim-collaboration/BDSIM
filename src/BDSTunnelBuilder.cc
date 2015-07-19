#include "BDSBeamline.hh"
#include "BDSDebug.hh"
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
{;}

BDSTunnelBuilder::~BDSTunnelBuilder()
{  
  _instance = 0;
  displacementTolerance = 50*CLHEP::cm;
}

G4bool BDSTunnelBuilder::BreakTunnel(G4double cumulativeLength,
				     G4double cumulativeAngle,
				     G4int    cumulativeNItems,
				     G4double cumulativeDisplacementX,
				     G4double cumulativeDisplacementY)
{
  G4bool result = false;

  G4bool lengthTest = false;
  if (cumulativeLength > 50*CLHEP::m)
    {lengthTest = true;}

  G4bool angleTest = false;
  if (cumulativeAngle > 10*CLHEP::mrad)
    {angleTest = true;}

  G4bool nItemsTest = false;
  if (cumulativeNItems > 10)
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
  G4cout << "Cumulative Length (mm): " << cumulativeLength << " test-> " << lengthTest << G4endl;
  G4cout << "Cumulative Angle (rad): " << cumulativeAngle  << " test-> " << angleTest  << G4endl;
  G4cout << "# of items:             " << cumulativeNItems << " test-> " << nItemsTest << G4endl;
  G4cout << "Cumulative displacement X: " << cumulativeDisplacementX << " test-> " << dispXTest << G4endl;
  G4cout << "Cumulative displacement Y: " << cumulativeDisplacementY << " test-> " << dispYTest << G4endl;
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
      G4bool breakIt = BreakTunnel(cumulativeLength,
				   cumulativeAngle,
				   cumulativeNItems,
				   cumulativeDisplacementX,
				   cumulativeDisplacementY);
      if (breakIt)
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
								 defaultModel->aper2);        // 2nd aperture param
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
						      defaultModel->aper2);        // 2nd aperture param
	    }
	  
	  // store segment in tunnel beam line
	  // set length correctly - for normal accelerator components, we get the
	  // arc length from the user, but in this case, we really start with the
	  // chord length.
	  G4double radiusOfCurvature = (segmentLength * 0.5) / cos(cumulativeAngle*0.5);
	  G4double arcLength         = radiusOfCurvature * cumulativeAngle;
	  BDSTunnelSegment* ts = new BDSTunnelSegment(name.str(),
						      arcLength,
						      cumulativeAngle,
						      tunnelSegment);
	  
	  // update / reset counters
	  nTunnelSegments += 1;
	  cumulativeLength = 0;
	  cumulativeAngle  = 0;
	  cumulativeNItems = 0;
	}
      else
	{
	  cumulativeLength += (*it)->GetAcceleratorComponent()->GetChordLength();
	  cumulativeAngle  += (*it)->GetAcceleratorComponent()->GetAngle();
	  cumulativeNItems += 1;
	  endElement++; // advance the potential end element iterator
	}
    }

  
  /*
  // build demo section of tunnel to check it works
  BDSTunnelType tunnelType = BDS::DetermineTunnelType("elliptical");
  G4Material*   soilMaterial = BDSMaterials::Instance()->GetMaterial("soil");
  G4Material*   concrete     = BDSMaterials::Instance()->GetMaterial("concrete");
  G4double      tunnelLength = 50*CLHEP::m;
  
  BDSGeometryComponent* aPieceOfTunnel = BDSTunnelFactory::Instance()->CreateTunnelSectionAngledInOut(tunnelType,
												      "tunny",
												      tunnelLength, // length
												      0.2, //angle in
												      0.3, //angle out
												      0.5*CLHEP::m, // t thickness
												      5*CLHEP::m, // s thickness
												      concrete,
												      soilMaterial,
												      true,
												      1*CLHEP::m,
												      2*CLHEP::m,
												      1.5*CLHEP::m);
											   
  BDSGeometryComponent* aPieceOfTunnel = BDSTunnelFactory::Instance()->CreateTunnelSection(tunnelType,
											   "tunny",
											   tunnelLength, // length
											   0.5*CLHEP::m, // t thickness
											   5*CLHEP::m,   // s thickness
											   concrete,     // t material
											   soilMaterial, // s material
											   true,         // floor?
											   1*CLHEP::m,   // floor offset
											   2*CLHEP::m,   // tunnel 1
											   1.5*CLHEP::m);// tunnel 2 

  
  new G4PVPlacement(0,  // its rotation
		    G4ThreeVector(0.5*CLHEP::m,-0.25*CLHEP::m,0.5*tunnelLength), // its position
		    "tunnel_pv",	    // its name
		    aPieceOfTunnel->GetContainerLogicalVolume(), // its logical volume
		    worldPV,	    // its mother  volume
		    false,	    // no boolean operation
		    0,            // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());//overlap checking
  */


  
  
  return std::make_pair(supportsLine,tunnelLine);
}
