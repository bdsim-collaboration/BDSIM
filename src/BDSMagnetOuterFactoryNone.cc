#include "BDSMagnetOuterFactoryNone.hh"

#include "BDSBeamPipe.hh"
#include "BDSDebug.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetColours.hh"
#include "BDSMaterials.hh"
#include "BDSSDManager.hh"
#include "BDSUtilities.hh"                 // for calculateorientation

#include "globals.hh"                      // geant4 globals / types
#include "G4Colour.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include <cmath>
#include <utility>                         // for std::pair
#include <algorithm>                       // for std::max

class BDSBeamPipe;
class G4Material;


BDSMagnetOuterFactoryNone* BDSMagnetOuterFactoryNone::_instance = 0;

BDSMagnetOuterFactoryNone* BDSMagnetOuterFactoryNone::Instance()
{
  if (_instance == 0)
    {_instance = new BDSMagnetOuterFactoryNone();}
  return _instance;
}

BDSMagnetOuterFactoryNone::BDSMagnetOuterFactoryNone()
{
}

BDSMagnetOuterFactoryNone::~BDSMagnetOuterFactoryNone()
{
  _instance = 0;
}

BDSGeometryComponent* BDSMagnetOuterFactoryNone::CreateSectorBend(G4String      /*name*/,
								  G4double      /*length*/,
								  BDSBeamPipe*  /*beamPipe*/,
								  G4double      /*boxSize*/,
								  G4double      /*angle*/,
								  G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return NULL;
}

BDSGeometryComponent* BDSMagnetOuterFactoryNone::CreateRectangularBend(G4String      /*name*/,
								       G4double      /*length*/,
								       BDSBeamPipe*  /*beamPipe*/,
								       G4double      /*boxSize*/,
								       G4double      /*angle*/,
								       G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return NULL;
}

BDSGeometryComponent* BDSMagnetOuterFactoryNone::CreateQuadrupole(G4String      /*name*/,
								  G4double      /*length*/,
								  BDSBeamPipe*  /*beamPipe*/,
								  G4double      /*boxSize*/,
								  G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return NULL;
}

BDSGeometryComponent* BDSMagnetOuterFactoryNone::CreateSextupole(G4String      /*name*/,
								 G4double      /*length*/,
								 BDSBeamPipe*  /*beamPipe*/,
								 G4double      /*boxSize*/,
								 G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return NULL;
}

BDSGeometryComponent* BDSMagnetOuterFactoryNone::CreateOctupole(G4String      /*name*/,
								G4double      /*length*/,
								BDSBeamPipe*  /*beamPipe*/,
								G4double      /*boxSize*/,
								G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return NULL;
}

BDSGeometryComponent* BDSMagnetOuterFactoryNone::CreateDecapole(G4String      /*name*/,
								G4double      /*length*/,
								BDSBeamPipe*  /*beamPipe*/,
								G4double      /*boxSize*/,
								G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return NULL;
}

BDSGeometryComponent* BDSMagnetOuterFactoryNone::CreateSolenoid(G4String      /*name*/,
								G4double      /*length*/,
								BDSBeamPipe*  /*beamPipe*/,
								G4double      /*boxSize*/,
								G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return NULL;
}

BDSGeometryComponent* BDSMagnetOuterFactoryNone::CreateMultipole(G4String      /*name*/,
								 G4double      /*length*/,
								 BDSBeamPipe*  /*beamPipe*/,
								 G4double      /*boxSize*/,
								 G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return NULL;
}

BDSGeometryComponent* BDSMagnetOuterFactoryNone::CreateRfCavity(G4String      /*name*/,
								G4double      /*length*/,
								BDSBeamPipe*  /*beamPipe*/,
								G4double      /*boxSize*/,
								G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return NULL;
}

BDSGeometryComponent* BDSMagnetOuterFactoryNone::CreateMuSpoiler(G4String      /*name*/,
								 G4double      /*length*/,
								 BDSBeamPipe*  /*beamPipe*/,
								 G4double      /*boxSize*/,
								 G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return NULL;
}

BDSGeometryComponent* BDSMagnetOuterFactoryNone::CreateKicker(G4String      /*name*/,
							      G4double      /*length*/,
							      BDSBeamPipe*  /*beamPipe*/,
							      G4double      /*boxSize*/,
							      G4bool        /*vertical*/,
							      G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return NULL;
}
