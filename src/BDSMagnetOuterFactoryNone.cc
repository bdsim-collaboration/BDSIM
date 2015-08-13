#include "BDSMagnetOuterFactoryNone.hh"

#include "BDSBeamPipe.hh"
#include "BDSDebug.hh"
#include "BDSGeometryComponent.hh"

#include "globals.hh"                      // geant4 globals / types
#include "G4LogicalVolume.hh"
#include "G4Material.hh"

#include <cmath>
#include <utility>                         // for std::pair
#include <algorithm>                       // for std::max

BDSMagnetOuterFactoryNone* BDSMagnetOuterFactoryNone::_instance = nullptr;

BDSMagnetOuterFactoryNone* BDSMagnetOuterFactoryNone::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSMagnetOuterFactoryNone();}
  return _instance;
}

BDSMagnetOuterFactoryNone::BDSMagnetOuterFactoryNone()
{
}

BDSMagnetOuterFactoryNone::~BDSMagnetOuterFactoryNone()
{
  _instance = nullptr;
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
  return nullptr;
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
  return nullptr;
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
  return nullptr;
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
  return nullptr;
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
  return nullptr;
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
  return nullptr;
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
  return nullptr;
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
  return nullptr;
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
  return nullptr;
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
  return nullptr;
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
  return nullptr;
}
