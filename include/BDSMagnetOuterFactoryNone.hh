#ifndef BDSMAGNETOUTERFACTORYNONE_H
#define BDSMAGNETOUTERFACTORYNONE_H

#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterFactoryBase.hh"

#include "globals.hh"           // geant4 globals / types

class BDSBeamPipe;
class G4Material;

class BDSMagnetOuterFactoryNone: public BDSMagnetOuterFactoryBase
{
public:
  static BDSMagnetOuterFactoryNone* Instance(); /// singleton pattern
  
  ~BDSMagnetOuterFactoryNone();
  
  /// sector bend outer volume
  virtual BDSMagnetOuter* CreateSectorBend(G4String     name,                  // name
					   G4double     length,                // length [mm]
					   BDSBeamPipe* beamPipe,              // beampipe
					   G4double     outerDiameter,         // full width
					   G4double     containerLength,       // full length to make AccComp container
					   G4double     angle,                 // full bend angle [rad]
					   G4double     e1,                    // Poleface Rotation   
					   G4double     e2,                    // Poleface Rotation
					   G4Material*  outerMaterial = nullptr// material for outer volume
					   );

  /// rectangular bend outer volume
  virtual BDSMagnetOuter* CreateRectangularBend(G4String     name,                  // name
						G4double     length,                // length [mm]
						BDSBeamPipe* beamPipe,              // beampipe
						G4double     outerDiameter,         // full width
						G4double     containerDiameter,     // full width to make AccComp contianer
						G4double     containerLength,       // full length to make AccComp container
						G4double     angle,                 // full bend angle [rad]
						G4Material*  outerMaterial = nullptr// material for outer volume
						);
  
  /// quadrupole outer volume
  virtual BDSMagnetOuter* CreateQuadrupole(G4String     name,                  // name
					   G4double     length,                // length [mm]
					   BDSBeamPipe* beamPipe,              // beampipe
					   G4double     outerDiameter,         // full width
					   G4double     containerLength,       // full length to make AccComp container
					   G4Material*  outerMaterial = nullptr// material for outer volume
					   );
  

  /// sextupole outer volume
  virtual BDSMagnetOuter* CreateSextupole(G4String     name,                  // name
					  G4double     length,                // length [mm]
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     outerDiameter,         // full width
					  G4double     containerLength,       // full length to make AccComp container
					  G4Material*  outerMaterial = nullptr// material for outer volume
					  );

  /// octupole outer volume
  virtual BDSMagnetOuter* CreateOctupole(G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr// material for outer volume
					 );
  
  /// decapole outer volume
  virtual BDSMagnetOuter* CreateDecapole(G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr// material for outer volume
					 );
  
  /// solenoid  outer volume
  virtual BDSMagnetOuter* CreateSolenoid(G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr// material for outer volume
					 );
  
  /// general multipole outer volume - could be any 2N order multipole
  virtual BDSMagnetOuter* CreateMultipole(G4String     name,                  // name
					  G4double     length,                // length [mm]
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     outerDiameter,         // full width
					  G4double     containerLength,       // full length to make AccComp container
					  G4Material*  outerMaterial = nullptr// material for outer volume
					  );
  
  /// RF cavity outer volume
  virtual BDSMagnetOuter* CreateRfCavity(G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr// material for outer volume
					 );

  /// muon spoiler outer volume
  virtual BDSMagnetOuter* CreateMuSpoiler(G4String     name,                  // name
					  G4double     length,                // length [mm]
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     outerDiameter,         // full width
					  G4double     containerLength,       // full length to make AccComp container
					  G4Material*  outerMaterial = nullptr// material for outer volume
					  );
  
  /// horizontal and vertical kicker outer volume
  virtual BDSMagnetOuter* CreateKicker(G4String     name,                  // name
				       G4double     length,                // length [mm]
				       BDSBeamPipe* beamPipe,              // beampipe
				       G4double     outerDiameter,         // full width
				       G4double     containerLength,       // full length to make AccComp container
				       G4bool       vertical = true,       // is it a vertical kicker?
				       G4Material*  outerMaterial = nullptr// material for outer volume
				       );
  
private:
  BDSMagnetOuterFactoryNone(); //private constructor as singleton
  static BDSMagnetOuterFactoryNone* _instance;
};

#endif
