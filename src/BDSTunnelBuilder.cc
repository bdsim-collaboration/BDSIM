#include "BDSBeamline.hh"
#include "BDSDebug.hh"
#include "BDSTunnelBuilder.hh"

#include "globals.hh"

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
}

G4bool BDSTunnelBuilder::BreakTunnel(G4double cumulativeLength,
				     G4double cumulativeAngle,
				     G4int    cumulativeNItems)
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

  result = lengthTest || angleTest || nItemsTest;

  return result;
}

std::pair<BDSBeamline*,BDSBeamline*> BDSTunnelBuilder::BuildTunnelAndSupports(BDSBeamline* flatBeamline)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSBeamline* tunnelLine   = new BDSBeamline();
  BDSBeamline* supportsLine = new BDSBeamline();

  G4double cumulativeLength = 0;
  G4double cumulativeAngle  = 0;
  G4int    cumulativeNItems = 0;

  BDSBeamlineIterator it = flatBeamline->begin();
  for (; it != flatBeamline->end(); ++it)
    {
      if (BreakTunnel(cumulativeLength,cumulativeAngle,cumulativeNItems))
	{
	  // work out tunnel parameters
	  // create tunnel segment
	  // place in tunnel beam line
	  // reset counters
	  cumulativeLength = 0;
	  cumulativeAngle  = 0;
	  cumulativeNItems = 0;
	}
      else
	{
	  cumulativeLength += (*it)->GetAcceleratorComponent()->GetChordLength();
	  cumulativeAngle  += (*it)->GetAcceleratorComponent()->GetAngle();
	  cumulativeNItems += 1;
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
											   /*
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
