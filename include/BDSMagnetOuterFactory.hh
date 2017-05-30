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

  /// Main interface to creating a magnet outer piece of geometry. Specified by magnet type,
  /// the recipe, the length of the magnet outer section, the length of the appropriately fitting
  /// containter volume that's also constructed and w.r.t. an already constructed beam pipe.
  BDSMagnetOuter* CreateMagnetOuter(BDSMagnetType       magnetType,
				    BDSMagnetOuterInfo* outerInfo,
				    G4double            outerLength,
				    G4double            chordLength,
				    BDSBeamPipe*        beampipe);
  
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
