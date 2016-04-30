#ifndef BDSMAGNETOUTERFACTORYPOLESBASE_H
#define BDSMAGNETOUTERFACTORYPOLESBASE_H

#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterFactoryBase.hh"
#include "BDSBeamPipe.hh"

#include "globals.hh"           // geant4 globals / types
#include "G4Material.hh"
#include "G4TwoVector.hh"
#include "G4VSolid.hh"

#include <vector>

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
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactoryPolesBase: public BDSMagnetOuterFactoryBase
{
public:
  BDSMagnetOuterFactoryPolesBase();
  virtual ~BDSMagnetOuterFactoryPolesBase(){;}
  
  /// sector bend outer volume
  virtual BDSMagnetOuter* CreateSectorBend(G4String     name,            // name
					   G4double     length,          // length [mm]
					   BDSBeamPipe* beamPipe,        // beampipe
					   G4double     outerDiameter,   // full width
					   G4double     containerLength, // full length to make AccComp container
					   G4double     angleIn,         // input face angle w.r.t. chord
					   G4double     angleOut,        // output face angle w.r.t. chord
					   G4Material*  outerMaterial = nullptr // material for outer volume
					   );

  /// rectangular bend outer volume
  virtual BDSMagnetOuter* CreateRectangularBend(G4String     name,              // name
						G4double     length,            // length [mm]
						BDSBeamPipe* beamPipe,          // beampipe
						G4double     outerDiameter,     // full width
						G4double     containerDiameter, // full width to make AccComp contianer
						G4double     containerLength,   // full length to make AccComp container
						G4double     angleIn,           // input face angle w.r.t. chord
						G4double     angleOut,          // output face angle w.r.t. chord
						G4Material*  outerMaterial = nullptr // material for outer volume
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
  
protected:
  // geometry parameters
  /// The fraction of the distance from the beam pipe to the outerDiameter/2 that each pole
  /// will take - always < 1
  const G4double poleFraction;

  /// Fraction of 2pi/Npoles that the pole will occupy - always < 1
  const G4double poleAngularFraction;

  /// Fraction of length from pole tip to outerDiameter that pole tip ellisoid will
  /// take up
  const G4double poleTipFraction;

  /// Fraction of length from pole tip to outerDiamter that the expanding section of
  /// the pole will take up. There's the tip, this bit, then a straight bit.
  const G4double poleAnnulusFraction;

  /// Bends are often wider in the bending plane. As we only have one parameter to pass
  /// which is outerDimaeter, make the non bending dimension a fixed (< 1) fraction of
  /// the outerDimaeter.
  const G4double bendHeightFraction;

  /// Factor by which the pole length is multiplied for the raw pole length before it's
  /// intersected with the inside of the yoke. Where the pole would normally stop at
  /// yokeStartRadius - lengthSaftey, it runs to yokeStartRadius x poleStopFactor.
  const G4double poleStopFactor;
  
  //length will be calculated in each derived class
  //will be less than outer radius but long enough so poles can be
  //boolean added to return yoke - this is different for each type
  G4double poleStartRadius;
  G4double poleFinishRadius;
  G4double yokeStartRadius;
  G4double yokeFinishRadius;
  G4double magnetContainerRadius;
  G4double poleSquareWidth;
  G4double poleSquareStartRadius;
  G4double poleAngle;
  
  G4VSolid* coilLeftSolid;
  G4VSolid* coilRightSolid;
  G4LogicalVolume* coilLeftLV;
  G4LogicalVolume* coilRightLV;

  std::vector<G4TwoVector> leftPoints;
  std::vector<G4TwoVector> rightPoints;

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
					   G4int         order,
					   G4double      magnetContainerRadius);

  /// Place the poles and yoke in the container volume
  virtual void PlaceComponents(G4String name,
			       G4int    order);

  /// Common construction tasks to all methods - assemble yoke and poles in container
  virtual BDSMagnetOuter* CommonConstructor(G4String     name,
					    G4double     length,
					    BDSBeamPipe* beamPipe,
					    G4int        order,
					    G4double     outerDiameter,
					    G4Material*  outerMaterial,
					    G4double     magnetContainerLength);
  
  /// Test inputs for no null pointers or overlapping volumes due to poorly defined sizes
  void TestInputParameters(BDSBeamPipe* beamPipe,
			   G4double&    boxSizeIn,
			   G4Material*& outerMaterialIn);

  /// Kicker constructor - only difference between h and vkick is the 90 degree rotation
  /// If it isn't vertical, then it's a horizontal kicker
  virtual BDSMagnetOuter* KickerConstructor(G4String     name,
					    G4double     length,
					    G4double     angle,
					    BDSBeamPipe* beamPipe,
					    G4double     outerDiameter,
					    G4Material*  outerMaterial,
					    G4bool       isVertical);
  
};

#endif
