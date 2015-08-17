#include "BDSMagnetOuterFactoryNone.hh"

#include "BDSBeamPipe.hh"
#include "BDSDebug.hh"
#include "BDSMagnetOuter.hh"

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

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateSectorBend(G4String      /*name*/,
							    G4double      /*length*/,
							    BDSBeamPipe*  /*beamPipe*/,
							    G4double      /*outerDiameter*/,
							    G4double      /*containerLength*/,
							    G4double      /*angle*/,
							    G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateRectangularBend(G4String      /*name*/,
								 G4double      /*length*/,
								 BDSBeamPipe*  /*beamPipe*/,
								 G4double      /*outerDiameter*/,
								 G4double      /*containerLength*/,
								 G4double      /*angle*/,
								 G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateQuadrupole(G4String      /*name*/,
							    G4double      /*length*/,
							    BDSBeamPipe*  /*beamPipe*/,
							    G4double      /*outerDiameter*/,
							    G4double      /*containerLength*/,
							    G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateSextupole(G4String      /*name*/,
							   G4double      /*length*/,
							   BDSBeamPipe*  /*beamPipe*/,
							   G4double      /*outerDiameter*/,
							   G4double      /*containerLength*/,
							   G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateOctupole(G4String      /*name*/,
							  G4double      /*length*/,
							  BDSBeamPipe*  /*beamPipe*/,
							  G4double      /*outerDiameter*/,
							  G4double      /*containerLength*/,
							  G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateDecapole(G4String      /*name*/,
							  G4double      /*length*/,
							  BDSBeamPipe*  /*beamPipe*/,
							  G4double      /*outerDiameter*/,
							  G4double      /*containerLength*/,
							  G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateSolenoid(G4String      /*name*/,
							  G4double      /*length*/,
							  BDSBeamPipe*  /*beamPipe*/,
							  G4double      /*outerDiameter*/,
							  G4double      /*containerLength*/,
							  G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateMultipole(G4String      /*name*/,
							   G4double      /*length*/,
							   BDSBeamPipe*  /*beamPipe*/,
							   G4double      /*outerDiameter*/,
							   G4double      /*containerLength*/,
							   G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateRfCavity(G4String      /*name*/,
							  G4double      /*length*/,
							  BDSBeamPipe*  /*beamPipe*/,
							  G4double      /*outerDiameter*/,
							  G4double      /*containerLength*/,
							  G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateMuSpoiler(G4String      /*name*/,
							   G4double      /*length*/,
							   BDSBeamPipe*  /*beamPipe*/,
							   G4double      /*outerDiameter*/,
							   G4double      /*containerLength*/,
							   G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateKicker(G4String      /*name*/,
							G4double      /*length*/,
							BDSBeamPipe*  /*beamPipe*/,
							G4double      /*outerDiameter*/,
							G4double      /*containerLength*/,
							G4bool        /*vertical*/,
							G4Material*   /*outerMaterial*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return nullptr;
}
