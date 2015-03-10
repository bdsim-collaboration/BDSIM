#ifndef BDSMAGNETOUTERFACTORY_H
#define BDSMAGNETOUTERFACTORY_H

#include "BDSGeometryComponent.hh"

#include "BDSBeamPipe.hh"
#include "BDSMagnetOuterFactoryBase.hh"
#include "BDSMagnetGeometryType.hh"

#include "globals.hh"           // geant4 globals / types
#include "G4Material.hh"

class BDSMagnetOuterFactory
{
public:
  static BDSMagnetOuterFactory* Instance(); /// singleon pattern
  
  ~BDSMagnetOuterFactory();

  /// sector bend outer volume
  virtual BDSGeometryComponent* CreateSectorBend(BDSMagnetGeometryType magnetType,          // magnet type
						 G4String      name,                // name
						 G4double      length,              // length [mm]
						 BDSBeamPipe*  beamPipe,            // beampipe
						 G4double      boxSize,             // full width
						 G4double      angle,               // full bend angle [rad]
						 G4Material*   outerMaterial = NULL // material for outer volume
						 );

  /// rectangular bend outer volume
  virtual BDSGeometryComponent* CreateRectangularBend(BDSMagnetGeometryType magnetType,          // magnet type
						      G4String      name,                // name
						      G4double      length,              // length [mm]
						      BDSBeamPipe*  beamPipe,            // beampipe
						      G4double      boxSize,             // full width
						      G4double      angle,               // full bend angle [rad]
						      G4Material*   outerMaterial = NULL // material for outer volume
						      );
  
  /// quadrupole outer volume
  virtual BDSGeometryComponent* CreateQuadrupole(BDSMagnetGeometryType magnetType,           // magnet type
						 G4String      name,                 // name
						 G4double      length,               // length [mm]
						 BDSBeamPipe*  beamPipe,             // beampipe
						 G4double      boxSize,              // full width
						 G4Material*   outerMaterial = NULL  // material for outer volume
						 );


  /// sextupole outer volume
  virtual BDSGeometryComponent* CreateSextupole(BDSMagnetGeometryType magnetType,           // magnet type
						G4String      name,                 // name
						G4double      length,               // length [mm]
						BDSBeamPipe*  beamPipe,             // beampipe
						G4double      boxSize,              // full width
						G4Material*   outerMaterial = NULL  // material for outer volume
						);

  /// octupole outer volume
  virtual BDSGeometryComponent* CreateOctupole(BDSMagnetGeometryType magnetType,            // magnet type
					       G4String      name,                  // name
					       G4double      length,                // length [mm]
					       BDSBeamPipe*  beamPipe,              // beampipe
					       G4double      boxSize,               // full width
					       G4Material*   outerMaterial = NULL   // material for outer volume
					       );

  /// decapole outer volume
  virtual BDSGeometryComponent* CreateDecapole(BDSMagnetGeometryType magnetType,            // magnet type
					       G4String      name,                  // name
					       G4double      length,                // length [mm]
					       BDSBeamPipe*  beamPipe,              // beampipe
					       G4double      boxSize,               // full width
					       G4Material*   outerMaterial = NULL   // material for outer volume
					       );

  /// solenoid  outer volume
  virtual BDSGeometryComponent* CreateSolenoid(BDSMagnetGeometryType magnetType,            // magnet type
					       G4String      name,                  // name
					       G4double      length,                // length [mm]
					       BDSBeamPipe*  beamPipe,              // beampipe
					       G4double      boxSize,               // full width
					       G4Material*   outerMaterial = NULL   // material for outer volume
					       );

  /// general multipole outer volume - could be any 2N order multipole
  virtual BDSGeometryComponent* CreateMultipole(BDSMagnetGeometryType magnetType,           // magnet type
						G4String      name,                 // name
						G4double      length,               // length [mm]
						BDSBeamPipe*  beamPipe,             // beampipe
						G4double      boxSize,              // full width
						G4Material*   outerMaterial = NULL  // material for outer volume
						);

  /// RF cavity outer volume
  virtual BDSGeometryComponent* CreateRfCavity(BDSMagnetGeometryType magnetType,            // magnet type
					       G4String      name,                  // name
					       G4double      length,                // length [mm]
					       BDSBeamPipe*  beamPipe,              // beampipe
					       G4double      boxSize,               // full width
					       G4Material*   outerMaterial = NULL   // material for outer volume
					       );

  /// muon spoiler outer volume
  virtual BDSGeometryComponent* CreateMuSpoiler(BDSMagnetGeometryType magnetType,           // magnet type
						G4String      name,                 // name
						G4double      length,               // length [mm]
						BDSBeamPipe*  beamPipe,             // beampipe
						G4double      boxSize,              // full width
						G4Material*   outerMaterial = NULL  // material for outer volume
						);

  /// horizontal and vertical kicker outer volume
  virtual BDSGeometryComponent* CreateKicker(BDSMagnetGeometryType magnetType,            // magnet type
					     G4String      name,                  // name
					     G4double      length,                // length [mm]
					     BDSBeamPipe*  beamPipe,              // beampipe
					     G4double      boxSize,               // full width
					     G4bool        vertical = true,       // is it a vertical kicker?
					     G4Material*   outerMaterial = NULL   // material for outer volume
					     );

private:
  BDSMagnetOuterFactory();
  static BDSMagnetOuterFactory* _instance;
  BDSMagnetOuterFactoryBase* GetAppropriateFactory(BDSMagnetGeometryType magnetTypeIn);

};



#endif
