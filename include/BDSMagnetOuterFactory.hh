#ifndef BDSMAGNETOUTERFACTORY_H
#define BDSMAGNETOUTERFACTORY_H

#include "BDSBeamPipe.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterFactoryBase.hh"
#include "BDSMagnetGeometryType.hh"

#include "globals.hh"           // geant4 globals / types
#include "G4Material.hh"

/**
 * @brief The main interface for using the magnet outer factories.
 * 
 * Each function in this class mirrors
 * one in BDSMagnetOuterFactoryBase but with the addition of a BDSMagnetGeometryType enum as an argument. This
 * factory dispatches the call to the correct factory and should be the only one used by the caller.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSMagnetOuterFactory
{
public:
  static BDSMagnetOuterFactory* Instance(); /// singleton pattern
  
  ~BDSMagnetOuterFactory();
  
  /// sector bend outer volume
  virtual BDSMagnetOuter* CreateSectorBend(BDSMagnetGeometryType magnetType,   // magnet type
					   G4String     name,                  // name
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
  virtual BDSMagnetOuter* CreateRectangularBend(BDSMagnetGeometryType magnetType,   // magnet type
						G4String     name,                  // name
						G4double     length,                // length [mm]
						BDSBeamPipe* beamPipe,              // beampipe
						G4double     outerDiameter,         // full width
						G4double     containerDiameter,     // full width to make AccComp container
						G4double     containerLength,       // full length to make AccComp container
						G4double     angle,                 // full bend angle [rad]
						G4Material*  outerMaterial = nullptr// material for outer volume
						);
  
  /// quadrupole outer volume
  virtual BDSMagnetOuter* CreateQuadrupole(BDSMagnetGeometryType magnetType,   // magnet type
					   G4String     name,                  // name
					   G4double     length,                // length [mm]
					   BDSBeamPipe* beamPipe,              // beampipe
					   G4double     outerDiameter,         // full width
					   G4double     containerLength,       // full length to make AccComp container
					   G4Material*  outerMaterial = nullptr// material for outer volume
					   );
  

  /// sextupole outer volume
  virtual BDSMagnetOuter* CreateSextupole(BDSMagnetGeometryType magnetType,   // magnet type
					  G4String     name,                  // name
					  G4double     length,                // length [mm]
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     outerDiameter,         // full width
					  G4double     containerLength,       // full length to make AccComp container
					  G4Material*  outerMaterial = nullptr// material for outer volume
					  );
  
  /// octupole outer volume
  virtual BDSMagnetOuter* CreateOctupole(BDSMagnetGeometryType magnetType,   // magnet type
					 G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr// material for outer volume
					 );
  
  /// decapole outer volume
  virtual BDSMagnetOuter* CreateDecapole(BDSMagnetGeometryType magnetType,   // magnet type
					 G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr// material for outer volume
					 );
  
  /// solenoid  outer volume
  virtual BDSMagnetOuter* CreateSolenoid(BDSMagnetGeometryType magnetType,   // magnet type
					 G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr// material for outer volume
					 );
  
  /// general multipole outer volume - could be any 2N order multipole
  virtual BDSMagnetOuter* CreateMultipole(BDSMagnetGeometryType magnetType,   // magnet type
					  G4String     name,                  // name
					  G4double     length,                // length [mm]
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     outerDiameter,         // full width
					  G4double     containerLength,       // full length to make AccComp container
					  G4Material*  outerMaterial = nullptr// material for outer volume
					  );
  
  /// RF cavity outer volume
  virtual BDSMagnetOuter* CreateRfCavity(BDSMagnetGeometryType magnetType,   // magnet type
					 G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr// material for outer volume
					 );
  
  /// muon spoiler outer volume
  virtual BDSMagnetOuter* CreateMuSpoiler(BDSMagnetGeometryType magnetType,   // magnet type
					  G4String     name,                  // name
					  G4double     length,                // length [mm]
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     outerDiameter,         // full width
					  G4double     containerLength,       // full length to make AccComp container
					  G4Material*  outerMaterial = nullptr// material for outer volume
					  );
  
  /// horizontal and vertical kicker outer volume
  virtual BDSMagnetOuter* CreateKicker(BDSMagnetGeometryType magnetType,   // magnet type
				       G4String     name,                  // name
				       G4double     length,                // length [mm]
				       BDSBeamPipe* beamPipe,              // beampipe
				       G4double     outerDiameter,         // full width
				       G4double     containerLength,       // full length to make AccComp container
				       G4bool       vertical = true,       // is it a vertical kicker?
				       G4Material*  outerMaterial = nullptr// material for outer volume
				       );

private:
  BDSMagnetOuterFactory();
  static BDSMagnetOuterFactory* _instance;
  BDSMagnetOuterFactoryBase* GetAppropriateFactory(BDSMagnetGeometryType magnetTypeIn);

};



#endif
