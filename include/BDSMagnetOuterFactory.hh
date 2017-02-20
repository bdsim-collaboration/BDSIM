#ifndef BDSMAGNETOUTERFACTORY_H
#define BDSMAGNETOUTERFACTORY_H

#include "BDSMagnetOuter.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMagnetType.hh"

#include "globals.hh"           // geant4 globals / types

class BDSBeamPipe;
class BDSMagnetOuterFactoryBase;
class BDSMagnetOuterInfo;
class G4Material;

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
  BDSMagnetOuter* CreateSectorBend(BDSMagnetGeometryType magnetType,   // magnet type
				   G4String     name,                  // name
				   G4double     length,                // length [mm]
				   BDSBeamPipe* beamPipe,              // beampipe
				   G4double     outerDiameter,         // full width
				   G4double     containerLength,       // full length to make AccComp container
				   G4double     angleIn,               // input face angle w.r.t. chord
				   G4double     angleOut,              // output face angle w.r.t. chord
				   G4bool       yokeOnLeft,            // build magnet yoke on left of bend
				   G4Material*  outerMaterial = nullptr,// material for outer volume
				   G4bool       buildEndPiece = false
				   );
  
  /// rectangular bend outer volume
  BDSMagnetOuter* CreateRectangularBend(BDSMagnetGeometryType magnetType,   // magnet type
					G4String     name,                  // name
					G4double     length,                // length [mm]
					BDSBeamPipe* beamPipe,              // beampipe
					G4double     outerDiameter,         // full width
					G4double     containerLength,       // full length to make AccComp container
					G4double     angleIn,               // input face angle w.r.t. chord
					G4double     angleOut,              // output face angle w.r.t. chord
					G4bool       yokeOnLeft,            // build magnet yoke on left of bend
					G4Material*  outerMaterial = nullptr,// material for outer volume
					G4bool       buildEndPiece = false
					);
  
  /// quadrupole outer volume
  BDSMagnetOuter* CreateQuadrupole(BDSMagnetGeometryType magnetType,   // magnet type
				   G4String     name,                  // name
				   G4double     length,                // length [mm]
				   BDSBeamPipe* beamPipe,              // beampipe
				   G4double     outerDiameter,         // full width
				   G4double     containerLength,       // full length to make AccComp container
				   G4Material*  outerMaterial = nullptr,// material for outer volume
				   G4bool       buildEndPiece = false
				   );
  

  /// sextupole outer volume
  BDSMagnetOuter* CreateSextupole(BDSMagnetGeometryType magnetType,   // magnet type
				  G4String     name,                  // name
				  G4double     length,                // length [mm]
				  BDSBeamPipe* beamPipe,              // beampipe
				  G4double     outerDiameter,         // full width
				  G4double     containerLength,       // full length to make AccComp container
				  G4Material*  outerMaterial = nullptr,// material for outer volume
				  G4bool       buildEndPiece = false
				  );
  
  /// octupole outer volume
  BDSMagnetOuter* CreateOctupole(BDSMagnetGeometryType magnetType,   // magnet type
				 G4String     name,                  // name
				 G4double     length,                // length [mm]
				 BDSBeamPipe* beamPipe,              // beampipe
				 G4double     outerDiameter,         // full width
				 G4double     containerLength,       // full length to make AccComp container
				 G4Material*  outerMaterial = nullptr,// material for outer volume
				 G4bool       buildEndPiece = false
				 );
  
  /// decapole outer volume
  BDSMagnetOuter* CreateDecapole(BDSMagnetGeometryType magnetType,   // magnet type
				 G4String     name,                  // name
				 G4double     length,                // length [mm]
				 BDSBeamPipe* beamPipe,              // beampipe
				 G4double     outerDiameter,         // full width
				 G4double     containerLength,       // full length to make AccComp container
				 G4Material*  outerMaterial = nullptr,// material for outer volume
				 G4bool       buildEndPiece = false
				 );
  
  /// solenoid  outer volume
  BDSMagnetOuter* CreateSolenoid(BDSMagnetGeometryType magnetType,   // magnet type
				 G4String     name,                  // name
				 G4double     length,                // length [mm]
				 BDSBeamPipe* beamPipe,              // beampipe
				 G4double     outerDiameter,         // full width
				 G4double     containerLength,       // full length to make AccComp container
				 G4Material*  outerMaterial = nullptr,// material for outer volume
				 G4bool       buildEndPiece = false
				 );
  
  /// general multipole outer volume - could be any 2N order multipole
  BDSMagnetOuter* CreateMultipole(BDSMagnetGeometryType magnetType,   // magnet type
				  G4String     name,                  // name
				  G4double     length,                // length [mm]
				  BDSBeamPipe* beamPipe,              // beampipe
				  G4double     outerDiameter,         // full width
				  G4double     containerLength,       // full length to make AccComp container
				  G4Material*  outerMaterial = nullptr,// material for outer volume
				  G4bool       buildEndPiece = false
				  );
  
  /// RF cavity outer volume
  BDSMagnetOuter* CreateRfCavity(BDSMagnetGeometryType magnetType,   // magnet type
				 G4String     name,                  // name
				 G4double     length,                // length [mm]
				 BDSBeamPipe* beamPipe,              // beampipe
				 G4double     outerDiameter,         // full width
				 G4double     containerLength,       // full length to make AccComp container
				 G4Material*  outerMaterial = nullptr,// material for outer volume
				 G4bool       buildEndPiece = false
				 );
  
  /// muon spoiler outer volume
  BDSMagnetOuter* CreateMuSpoiler(BDSMagnetGeometryType magnetType,   // magnet type
				  G4String     name,                  // name
				  G4double     length,                // length [mm]
				  BDSBeamPipe* beamPipe,              // beampipe
				  G4double     outerDiameter,         // full width
				  G4double     containerLength,       // full length to make AccComp container
				  G4Material*  outerMaterial = nullptr,// material for outer volume
				  G4bool       buildEndPiece = false
				  );
  
  /// horizontal and vertical kicker outer volume
  BDSMagnetOuter* CreateKicker(BDSMagnetGeometryType magnetType,   // magnet type
			       G4String     name,                  // name
			       G4double     length,                // length [mm]
			       BDSBeamPipe* beamPipe,              // beampipe
			       G4double     outerDiameter,         // full width
			       G4double     containerLength,       // full length to make AccComp container
			       G4bool       vertical = true,       // is it a vertical kicker?
			       G4Material*  outerMaterial = nullptr,// material for outer volume
			       G4bool       buildEndPiece = false
			       );
  
private:
  BDSMagnetOuterFactory();
  static BDSMagnetOuterFactory* instance; ///< Singleton instance.

  /// Get the appropriate derived factory for the required magnet style.
  BDSMagnetOuterFactoryBase* GetAppropriateFactory(BDSMagnetGeometryType magnetTypeIn);

  BDSMagnetOuter* CreateExternal(G4String            name,
				 BDSMagnetOuterInfo* info,
				 G4double            length,
				 BDSBeamPipe*        beampipe);

  /// Create a container based on an external geometry component. Exists here as the
  /// using the external factory exists here and doesn't belong in a derived factory.
  /// Uses the beam pipe instance to get the surface normals as the container must match.
  /// Here, length is the full length of the final object desired.
  BDSGeometryComponent* CreateContainer(G4String             name,
					G4double             length,
					BDSGeometryExternal* external,
					BDSBeamPipe*         beampipe);

};



#endif
