#ifndef BDSMAGNETOUTERFACTORYPOLESBASE_H
#define BDSMAGNETOUTERFACTORYPOLESBASE_H

#include "BDSGeometryComponent.hh"
#include "BDSMagnetOuterFactoryBase.hh"
#include "BDSBeamPipe.hh"

#include "globals.hh"           // geant4 globals / types
#include "G4Colour.hh"
#include "G4Material.hh"
#include "G4VSolid.hh"

/**
 * @brief Factory class for outer volume of magnets. Produces magnets
 * with 2N-poles around the beampipe with a yoke of various shapes.
 * 
 * Most magnets are 2N poles, but sector and r-bends as well as 
 * muon spoilers, and h/v kickers are unique.
 *
 * NOTE this is a base class in that there are derived classes
 * with different outer shapes - all have poles, but the return 
 * yoke can for example, circular, square or faceted (with 4N facets)
 *
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */


class BDSMagnetOuterFactoryPolesBase: public BDSMagnetOuterFactoryBase
{
public:
  //static BDSMagnetOuterFactoryPoles* Instance(); /// singleton pattern
  BDSMagnetOuterFactoryPolesBase();
  
  /// sector bend outer volume
  virtual BDSGeometryComponent* CreateSectorBend(G4String      name,                // name
						 G4double      length,              // length [mm]
						 BDSBeamPipe*  beamPipe,            // beampipe
						 G4double      boxSize,             // full width
						 G4double      angle,               // full bend angle [rad]
						 G4Material*   outerMaterial = nullptr // material for outer volume
						 );

  /// rectangular bend outer volume
  virtual BDSGeometryComponent* CreateRectangularBend(G4String      name,                // name
						      G4double      length,              // length [mm]
						      BDSBeamPipe*  beamPipe,            // beampipe
						      G4double      boxSize,             // full width
						      G4double      angle,               // full bend angle [rad]
						      G4Material*   outerMaterial = nullptr // material for outer volume
						      );
  
  /// quadrupole outer volume
  virtual BDSGeometryComponent* CreateQuadrupole(G4String      name,                 // name
						 G4double      length,               // length [mm]
						 BDSBeamPipe*  beamPipe,             // beampipe
						 G4double      boxSize,              // full width
						 G4Material*   outerMaterial = nullptr  // material for outer volume
						 );


  /// sextupole outer volume
  virtual BDSGeometryComponent* CreateSextupole(G4String      name,                 // name
						G4double      length,               // length [mm]
						BDSBeamPipe*  beamPipe,             // beampipe
						G4double      boxSize,              // full width
						G4Material*   outerMaterial = nullptr  // material for outer volume
						);

  /// octupole outer volume
  virtual BDSGeometryComponent* CreateOctupole(G4String      name,                  // name
					       G4double      length,                // length [mm]
					       BDSBeamPipe*  beamPipe,              // beampipe
					       G4double      boxSize,               // full width
					       G4Material*   outerMaterial = nullptr   // material for outer volume
					       );

  /// decapole outer volume
  virtual BDSGeometryComponent* CreateDecapole(G4String      name,                  // name
					       G4double      length,                // length [mm]
					       BDSBeamPipe*  beamPipe,              // beampipe
					       G4double      boxSize,               // full width
					       G4Material*   outerMaterial = nullptr   // material for outer volume
					       );

  /// solenoid  outer volume
  virtual BDSGeometryComponent* CreateSolenoid(G4String      name,                  // name
					       G4double      length,                // length [mm]
					       BDSBeamPipe*  beamPipe,              // beampipe
					       G4double      boxSize,               // full width
					       G4Material*   outerMaterial = nullptr   // material for outer volume
					       );

  /// general multipole outer volume - could be any 2N order multipole
  virtual BDSGeometryComponent* CreateMultipole(G4String      name,                 // name
						G4double      length,               // length [mm]
						BDSBeamPipe*  beamPipe,             // beampipe
						G4double      boxSize,              // full width
						G4Material*   outerMaterial = nullptr  // material for outer volume
						);

  /// RF cavity outer volume
  virtual BDSGeometryComponent* CreateRfCavity(G4String      name,                  // name
					       G4double      length,                // length [mm]
					       BDSBeamPipe*  beamPipe,              // beampipe
					       G4double      boxSize,               // full width
					       G4Material*   outerMaterial = nullptr   // material for outer volume
					       );

  /// muon spoiler outer volume
  virtual BDSGeometryComponent* CreateMuSpoiler(G4String      name,                 // name
						G4double      length,               // length [mm]
						BDSBeamPipe*  beamPipe,             // beampipe
						G4double      boxSize,              // full width
						G4Material*   outerMaterial = nullptr  // material for outer volume
						);

  /// horizontal and vertical kicker outer volume
  virtual BDSGeometryComponent* CreateKicker(G4String      name,                  // name
					     G4double      length,                // length [mm]
					     BDSBeamPipe*  beamPipe,              // beampipe
					     G4double      boxSize,               // full width
					     G4bool        vertical = true,       // is it a vertical kicker?
					     G4Material*   outerMaterial = nullptr   // material for outer volume
					     );
  
protected:
  // geometry parameters
  /// The fraction of the distance from the beam pipe to the outerDiameter/2 that each pole
  /// will take - always < 1
  G4double poleFraction;

  /// Fraction of 2pi/Npoles that the pole will occupy - always < 1
  G4double poleAngularFraction;

  /// Fraction of length from pole tip to outerDiameter that pole tip ellisoid will
  /// take up
  G4double poleTipFraction;
  
  //length will be calculated in each derived class
  //will be less than outer radius but long enough so poles can be
  //boolean added to return yoke - this is different for each type
  G4double poleStartRadius;
  G4double poleFinishRadius;
  G4double yokeStartRadius;
  G4double yokeFinishRadius;

  // functions

  /// Empty containers for next use - this class is never deleted so can't rely on scope
  virtual void CleanUp();

  /// Calculate the length of the pole and yoke radii based on the design
  virtual void CalculatePoleAndYoke(G4double     outerDiameter,
				    BDSBeamPipe* beamPipe,
				    G4double     order);
  
  //NOTE the poles are not joined (boolean union) to the outer yoke - there is a
  //gap of length safety. This won't affect physics results and speeds up tracking
  //as the solid is not a boolean of order Npoles + 1
  
  /// Create pole for magnet of order N where npoles = Nx2
  virtual void CreatePoleSolid(G4String      name,                 // name
			       G4double      length,               // length [mm]
			       G4int         order);               // Nx2 poles
  
  /// Create yoke that connects poles and container to put them in
  virtual void CreateYokeAndContainerSolid(G4String      name,
					   G4double      length,
					   G4int         order);

  /// Build the logical volumes from the solids assigning materials and colours and cuts
  virtual void CreateLogicalVolumes(G4String    name,
				    G4double    length,
				    G4int       order,
				    G4Material* outerMaterial);

  /// Place the poles and yoke in the container volume
  virtual void PlaceComponents(G4String name,
			       G4int    order);

  /// Common construction tasks to all methods - assemble yoke and poles in container
  virtual BDSGeometryComponent* CommonConstructor(G4String     name,
						  G4double     length,
						  BDSBeamPipe* beamPipe,
						  G4int        order,
						  G4double     outerDiameter,
						  G4Material*  outerMaterial);
  
  /// Test inputs for no null pointers or overlapping volumes due to poorly defined sizes
  void TestInputParameters(BDSBeamPipe* beamPipe,
			   G4double&    boxSizeIn,
			   G4Material*& outerMaterialIn);

  /// Kicker constructor - only difference between h and vkick is the 90 degree rotation
  /// If it isn't vertical, then it's a horizontal kicker
  virtual BDSGeometryComponent* KickerConstructor(G4String     name,
						  G4double     length,
						  G4double     angle,
						  BDSBeamPipe* beamPipe,
						  G4double     outerDiameter,
						  G4Material*  outerMaterial,
						  G4bool       isVertical);
  
};

#endif
