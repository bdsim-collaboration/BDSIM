#ifndef BDSMAGNETOUTERFACTORYNONE_H
#define BDSMAGNETOUTERFACTORYNONE_H

#include "BDSGeometryComponent.hh"
#include "BDSMagnetOuterFactoryBase.hh"
//#include "BDSBeamPipe.hh"

#include "globals.hh"           // geant4 globals / types
//#include "G4Material.hh"

class BDSBeamPipe;
class G4Material;

class BDSMagnetOuterFactoryNone: public BDSMagnetOuterFactoryBase
{
public:
  static BDSMagnetOuterFactoryNone* Instance(); /// singleton pattern
  
  ~BDSMagnetOuterFactoryNone();
  
  /// sector bend outer volume
  virtual BDSGeometryComponent* CreateSectorBend(G4String      name,                // name
						 G4double      length,              // length [mm]
						 BDSBeamPipe*  beamPipe,            // beampipe
						 G4double      boxSize,             // full width
						 G4double      angle,               // full bend angle [rad]
						 G4Material*   outerMaterial = NULL // material for outer volume
						 );

  /// rectangular bend outer volume
  virtual BDSGeometryComponent* CreateRectangularBend(G4String      name,                // name
						      G4double      length,              // length [mm]
						      BDSBeamPipe*  beamPipe,            // beampipe
						      G4double      boxSize,             // full width
						      G4double      angle,               // full bend angle [rad]
						      G4Material*   outerMaterial = NULL // material for outer volume
						      );
  
  /// quadrupole outer volume
  virtual BDSGeometryComponent* CreateQuadrupole(G4String      name,                 // name
						 G4double      length,               // length [mm]
						 BDSBeamPipe*  beamPipe,             // beampipe
						 G4double      boxSize,              // full width
						 G4Material*   outerMaterial = NULL  // material for outer volume
						 );


  /// sextupole outer volume
  virtual BDSGeometryComponent* CreateSextupole(G4String      name,                 // name
						G4double      length,               // length [mm]
						BDSBeamPipe*  beamPipe,             // beampipe
						G4double      boxSize,              // full width
						G4Material*   outerMaterial = NULL  // material for outer volume
						);

  /// octupole outer volume
  virtual BDSGeometryComponent* CreateOctupole(G4String      name,                  // name
					       G4double      length,                // length [mm]
					       BDSBeamPipe*  beamPipe,              // beampipe
					       G4double      boxSize,               // full width
					       G4Material*   outerMaterial = NULL   // material for outer volume
					       );

  /// decapole outer volume
  virtual BDSGeometryComponent* CreateDecapole(G4String      name,                  // name
					       G4double      length,                // length [mm]
					       BDSBeamPipe*  beamPipe,              // beampipe
					       G4double      boxSize,               // full width
					       G4Material*   outerMaterial = NULL   // material for outer volume
					       );

  /// solenoid  outer volume
  virtual BDSGeometryComponent* CreateSolenoid(G4String      name,                  // name
					       G4double      length,                // length [mm]
					       BDSBeamPipe*  beamPipe,              // beampipe
					       G4double      boxSize,               // full width
					       G4Material*   outerMaterial = NULL   // material for outer volume
					       );

  /// general multipole outer volume - could be any 2N order multipole
  virtual BDSGeometryComponent* CreateMultipole(G4String      name,                 // name
						G4double      length,               // length [mm]
						BDSBeamPipe*  beamPipe,             // beampipe
						G4double      boxSize,              // full width
						G4Material*   outerMaterial = NULL  // material for outer volume
						);

  /// RF cavity outer volume
  virtual BDSGeometryComponent* CreateRfCavity(G4String      name,                  // name
					       G4double      length,                // length [mm]
					       BDSBeamPipe*  beamPipe,              // beampipe
					       G4double      boxSize,               // full width
					       G4Material*   outerMaterial = NULL   // material for outer volume
					       );

  /// muon spoiler outer volume
  virtual BDSGeometryComponent* CreateMuSpoiler(G4String      name,                 // name
						G4double      length,               // length [mm]
						BDSBeamPipe*  beamPipe,             // beampipe
						G4double      boxSize,              // full width
						G4Material*   outerMaterial = NULL  // material for outer volume
						);

  /// horizontal and vertical kicker outer volume
  virtual BDSGeometryComponent* CreateKicker(G4String      name,                  // name
					     G4double      length,                // length [mm]
					     BDSBeamPipe*  beamPipe,              // beampipe
					     G4double      boxSize,               // full width
					     G4bool        vertical = true,       // is it a vertical kicker?
					     G4Material*   outerMaterial = NULL   // material for outer volume
					     );

private:
  BDSMagnetOuterFactoryNone(); //private constructor as singleton
  static BDSMagnetOuterFactoryNone* _instance;
};

#endif
