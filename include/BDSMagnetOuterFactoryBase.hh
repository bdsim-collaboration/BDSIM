#ifndef BDSMAGNETOUTERFACTORYBASE_H
#define BDSMAGNETOUTERFACTORYBASE_H

#include "BDSGeometryComponent.hh"
#include "BDSBeamPipe.hh"

#include "globals.hh"         // geant4 globals / types
#include "G4Material.hh"

#include <vector>

class G4UserLimits;
class G4VisAttributes;
class G4VPhysicalVolume;
class G4VSolid;

/**
 * @brief Abstract base class for magnet outer volume factories
 * 
 * Abstract base class that defines the interface of each factory
 * to build any type of outer logical volume required. 
 * 
 * Uses Geant4 default units by default, ie mm, rad (not metres)
 *
 * Strictly the naming should contain 'Outer' at the end to distinguish
 * that the function doesn't create the whole magnet, however this is
 * implied by the factory name and omitting it will reduce the name length.
 *
 * Each factory should implement a method for every type of magnet - ie for
 * each class that inherits BDSMagnet.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSMagnetOuterFactoryBase
{
public:
  /// NOTE each derived class is expected to be a singleton but this can't be specified here
  /// as it'd refer to the abstract base class - must be implemented in each derived class. 
  /// 'In' in argument variables is used to distinguish between that and possible
  /// internal member variables with the same name - avoiding 'itsVariable'.

  /// sector bend outer volume
  virtual BDSGeometryComponent* CreateSectorBend(G4String     name,                // name
						 G4double     length,              // length [mm]
						 BDSBeamPipe* beamPipe,            // beampipe
						 G4double     boxSize,             // full width
						 G4double     angle,               // full bend angle [rad]
						 G4Material*  outerMaterial = NULL // material for outer volume
						 ) = 0;

  /// rectangular bend outer volume
  virtual BDSGeometryComponent* CreateRectangularBend(G4String     name,                // name
						      G4double     length,              // length [mm]
						      BDSBeamPipe* beamPipe,            // beampipe
						      G4double     boxSize,             // full width
						      G4double     angle,               // full bend angle [rad]
						      G4Material*  outerMaterial = NULL // material for outer volume
						      ) = 0;

  
  /// quadrupole outer volume
  virtual BDSGeometryComponent* CreateQuadrupole(G4String     name,                 // name
						 G4double     length,               // length [mm]
						 BDSBeamPipe* beamPipe,             // beampipe
						 G4double     boxSize,              // full width
						 G4Material*  outerMaterial = NULL  // material for outer volume
						 ) = 0;


  /// sextupole outer volume
  virtual BDSGeometryComponent* CreateSextupole(G4String     name,                 // name
						G4double     length,               // length [mm]
						BDSBeamPipe* beamPipe,             // beampipe
						G4double     boxSize,              // full width
						G4Material*  outerMaterial = NULL  // material for outer volume
						) = 0;

  /// octupole outer volume
  virtual BDSGeometryComponent* CreateOctupole(G4String     name,                  // name
					       G4double     length,                // length [mm]
					       BDSBeamPipe* beamPipe,              // beampipe
					       G4double     boxSize,               // full width
					       G4Material*  outerMaterial = NULL   // material for outer volume
					       ) = 0;

  /// decapole outer volume
  virtual BDSGeometryComponent* CreateDecapole(G4String     name,                  // name
					       G4double     length,                // length [mm]
					       BDSBeamPipe* beamPipe,              // beampipe
					       G4double     boxSize,               // full width
					       G4Material*  outerMaterial = NULL   // material for outer volume
					       ) = 0;

  /// solenoid  outer volume
  virtual BDSGeometryComponent* CreateSolenoid(G4String     name,                  // name
					       G4double     length,                // length [mm]
					       BDSBeamPipe* beamPipe,              // beampipe
					       G4double     boxSize,               // full width
					       G4Material*  outerMaterial = NULL   // material for outer volume
					       ) = 0;

  /// general multipole outer volume - could be any 2N order multipole
  virtual BDSGeometryComponent* CreateMultipole(G4String     name,                 // name
						G4double     length,               // length [mm]
						BDSBeamPipe* beamPipe,             // beampipe
						G4double     boxSize,              // full width
						G4Material*  outerMaterial = NULL  // material for outer volume
						) = 0;

  /// RF cavity outer volume
  virtual BDSGeometryComponent* CreateRfCavity(G4String     name,                  // name
					       G4double     length,                // length [mm]
					       BDSBeamPipe* beamPipe,              // beampipe
					       G4double     boxSize,               // full width
					       G4Material*  outerMaterial = NULL   // material for outer volume
					       ) = 0;

  /// muon spoiler outer volume
  virtual BDSGeometryComponent* CreateMuSpoiler(G4String     name,                 // name
						G4double     length,               // length [mm]
						BDSBeamPipe* beamPipe,             // beampipe
						G4double     boxSize,              // full width
						G4Material*  outerMaterial = NULL  // material for outer volume
						) = 0;

  /// horizontal and vertical kicker outer volume
  virtual BDSGeometryComponent* CreateKicker(G4String     name,                  // name
					     G4double     length,                // length [mm]
					     BDSBeamPipe* beamPipe,              // beampipe
					     G4double     boxSize,               // full width
					     G4bool       vertical = true,       // is it a vertical kicker?
					     G4Material*  outerMaterial = NULL   // material for outer volume
					     ) = 0;

  /// Empty containers for next use - factories are never deleted so can't rely on scope
  virtual void CleanUp();

protected:
  BDSMagnetOuterFactoryBase();
  
  // geometric pointers that will be used to pass around components
  // within the factory (as different parts factorised so they can
  // be overridden by the derived classes.
  G4double           lengthSafety;
  G4bool             checkOverlaps;      // to avoid using globalconstants a lot
  G4double           nSegmentsPerCircle; // for visualisation improvement
  G4double           maxStepFactor;      // for user limits
  G4VSolid*          poleSolid; /// solid for an individual pole that will be placed multiple times
  G4VSolid*          yokeSolid; /// solid for outer part that connects all poles
  G4VSolid*          containerSolid;
  G4LogicalVolume*   poleLV;
  G4LogicalVolume*   yokeLV;
  G4LogicalVolume*   containerLV;
  G4VPhysicalVolume* yokePV;
  G4VisAttributes* outerVisAttributes;
  G4UserLimits* outerUserLimits;

  std::vector<G4LogicalVolume*>   allLogicalVolumes;
  std::vector<G4VPhysicalVolume*> allPhysicalVolumes;
  std::vector<G4RotationMatrix*>  allRotationMatrices;
  std::vector<G4VSolid*>          allSolids;
  std::vector<G4VisAttributes*>   allVisAttributes;
  std::vector<G4UserLimits*>      allUserLimits;
};

#endif
