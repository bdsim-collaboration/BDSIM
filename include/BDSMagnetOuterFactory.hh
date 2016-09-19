#ifndef BDSMAGNETOUTERFACTORY_H
#define BDSMAGNETOUTERFACTORY_H

#include "BDSBeamPipe.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetOuterFactoryBase.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMagnetType.hh"

#include "globals.hh"           // geant4 globals / types
#include "G4Material.hh"

/**
 * @brief The main interface for using the magnet outer factories.
 * 
 * Each function in this class mirrors one in BDSMagnetOuterFactoryBase 
 * but with the addition of a BDSMagnetGeometryType enum as an argument. 
 * This factory dispatches the call to the correct factory and should 
 * be the only one used by the caller.
 * 
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactory
{
public:
  /// Singleton accessor
  static BDSMagnetOuterFactory* Instance();
  
  ~BDSMagnetOuterFactory();

  BDSMagnetOuter* CreateMagnetOuter(BDSMagnetType       magnetType,
				    BDSMagnetOuterInfo* outerInfo,
				    G4double            outerLength,
				    G4double            chordLength,
				    BDSBeamPipe*        beampipe);
  
  /// sector bend outer volume
  virtual BDSMagnetOuter* CreateSectorBend(BDSMagnetGeometryType magnetType,   // magnet type
					   G4String     name,                  // name
					   G4double     length,                // length [mm]
					   BDSBeamPipe* beamPipe,              // beampipe
					   G4double     outerDiameter,         // full width
					   G4double     containerLength,       // full length to make AccComp container
					   G4double     angleIn,               // input face angle w.r.t. chord
					   G4double     angleOut,              // output face angle w.r.t. chord
					   G4bool       yokeOnLeft,            // build magnet yoke on left of bend
					   G4Material*  outerMaterial = nullptr// material for outer volume
					   );
  
  /// rectangular bend outer volume
  virtual BDSMagnetOuter* CreateRectangularBend(BDSMagnetGeometryType magnetType,   // magnet type
						G4String     name,                  // name
						G4double     length,                // length [mm]
						BDSBeamPipe* beamPipe,              // beampipe
						G4double     outerDiameter,         // full width
						G4double     containerLength,       // full length to make AccComp container
						G4double     angleIn,               // input face angle w.r.t. chord
						G4double     angleOut,              // output face angle w.r.t. chord
						G4bool       yokeOnLeft,            // build magnet yoke on left of bend
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

  BDSMagnetOuter* CreateExternal(G4String            name,
				 BDSMagnetOuterInfo* info,
				 G4double            length,
				 BDSBeamPipe*        beampipe);
  
  BDSGeometryComponent* CreateContainer(G4String             name,
					G4double             length,
					BDSGeometryExternal* external);

};



#endif
