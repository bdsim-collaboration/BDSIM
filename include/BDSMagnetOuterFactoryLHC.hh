#ifndef BDSMAGNETOUTERFACTORYLHC_H
#define BDSMAGNETOUTERFACTORYLHC_H

#include "BDSGeometryComponent.hh"
#include "BDSMagnetOuterFactoryBase.hh"
#include "BDSBeamPipe.hh"

#include "globals.hh"           // geant4 globals / types
#include "G4Colour.hh"
#include "G4Material.hh"
#include "G4VSolid.hh"

/**
 * @brief The base factory for the LHC magnet geometry
 * factory.
 * 
 * This class is not a singleton unlike the other factories
 * as it will be constructed twice by each derived class
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSMagnetOuterFactoryLHC: public BDSMagnetOuterFactoryBase
{
public:
  BDSMagnetOuterFactoryLHC(G4bool isLeftOffset);
  ~BDSMagnetOuterFactoryLHC(){;};
  
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
protected:
  G4bool isLeftOffset;
  G4bool IsLeftOffset(){return isLeftOffset;}
  
private:
  //only really one function needed for this factory
  //private to this factory only
  void CreateCylindricalSolids(G4String      name,                 // name
			       G4double      length,               // length [mm]
			       BDSBeamPipe*  beamPipe,             // beampipe
			       G4double      boxSize);             // full width

  /// common construction tasks to all methods
  BDSGeometryComponent* CommonFinalConstructor(G4String    name,
					       G4double    length,        // length [mm]
					       G4double    boxSize,       // full width [mm]
					       G4Material* outerMaterial,
					       G4Colour*   colour);

  /// test inputs for no null pointers or overlapping volumes due to poorly defined sizes
  void TestInputParameters(BDSBeamPipe* beamPipe,
			   G4double&    boxSizeIn,
			   G4Material*& outerMaterialIn);

  G4double  lengthSafety;
  G4double  nSegmentsPerCircle; // for visualisation improvement
  G4VSolid* outerSolid;
  G4VSolid* containerSolid;
};

#endif
