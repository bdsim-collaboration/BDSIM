#ifndef BDSMAGNETOUTERFACTORYCYLINDRICAL_H
#define BDSMAGNETOUTERFACTORYCYLINDRICAL_H

#include "BDSGeometryComponent.hh"
#include "BDSMagnetOuterFactoryBase.hh"
#include "BDSBeamPipe.hh"

#include "globals.hh"           // geant4 globals / types
#include "G4Colour.hh"
#include "G4Material.hh"
#include "G4VSolid.hh"

/**
 * @brief Factory that produces cylindrical magnet geometry.
 * 
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactoryCylindrical: public BDSMagnetOuterFactoryBase
{
public:
  static BDSMagnetOuterFactoryCylindrical* Instance(); /// singleton pattern
  
  virtual ~BDSMagnetOuterFactoryCylindrical();
  
  /// sector bend outer volume
  virtual BDSMagnetOuter* CreateSectorBend(G4String     name,            // name
					   G4double     length,          // length
					   BDSBeamPipe* beamPipe,        // beampipe
					   G4double     outerDiameter,   // full width
					   G4double     containerLength, // full length to make AccComp container
					   G4double     angleIn,         // input face angle w.r.t. chord
					   G4double     angleOut,        // output face angle w.r.t. chord
					   G4bool       yokeOnLeft,      // build magnet yoke on left of bend
					   G4Material*  outerMaterial = nullptr// material for outer volume
					   );
  
  /// rectangular bend outer volume
  virtual BDSMagnetOuter* CreateRectangularBend(G4String     name,              // name
						G4double     length,            // length
						BDSBeamPipe* beamPipe,          // beampipe
						G4double     outerDiameter,     // full width
						G4double     containerLength,   // full length to make AccComp container
						G4double     angleIn,           // input face angle w.r.t. chord
						G4double     angleOut,          // output face angle w.r.t. chord
						G4bool       yokeOnLeft,        // build magnet yoke on left of bend
						G4Material*  outerMaterial = nullptr// material for outer volume
						);
  
  /// quadrupole outer volume
  virtual BDSMagnetOuter* CreateQuadrupole(G4String     name,                  // name
					   G4double     length,                // length
					   BDSBeamPipe* beamPipe,              // beampipe
					   G4double     outerDiameter,         // full width
					   G4double     containerLength,       // full length to make AccComp container
					   G4Material*  outerMaterial = nullptr// material for outer volume
					   );


  /// sextupole outer volume
  virtual BDSMagnetOuter* CreateSextupole(G4String     name,                  // name
					  G4double     length,                // length
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     outerDiameter,         // full width
					  G4double     containerLength,       // full length to make AccComp container
					  G4Material*  outerMaterial = nullptr// material for outer volume
					  );
  
  /// octupole outer volume
  virtual BDSMagnetOuter* CreateOctupole(G4String     name,                  // name
					 G4double     length,                // length 
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr// material for outer volume
					 );
  
  /// decapole outer volume
  virtual BDSMagnetOuter* CreateDecapole(G4String     name,                  // name
					 G4double     length,                // length
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr// material for outer volume
					 );
  
  /// solenoid  outer volume
  virtual BDSMagnetOuter* CreateSolenoid(G4String     name,                  // name
					 G4double     length,                // length
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr// material for outer volume
					 );
  
  /// general multipole outer volume - could be any 2N order multipole
  virtual BDSMagnetOuter* CreateMultipole(G4String     name,                  // name
					  G4double     length,                // length
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     outerDiameter,         // full width
					  G4double     containerLength,       // full length to make AccComp container
					  G4Material*  outerMaterial = nullptr// material for outer volume
					  );

  /// RF cavity outer volume
  virtual BDSMagnetOuter* CreateRfCavity(G4String     name,                  // name
					 G4double     length,                // length 
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr// material for outer volume
					 );
  
  /// muon spoiler outer volume
  virtual BDSMagnetOuter* CreateMuSpoiler(G4String     name,                  // name
					  G4double     length,                // length
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     outerDiameter,         // full width
					  G4double     containerLength,       // full length to make AccComp container
					  G4Material*  outerMaterial = nullptr// material for outer volume
					  );

  /// horizontal and vertical kicker outer volume
  virtual BDSMagnetOuter* CreateKicker(G4String     name,                  // name
				       G4double     length,                // length
				       BDSBeamPipe* beamPipe,              // beampipe
				       G4double     outerDiameter,         // full width
				       G4double     containerLength,       // full length to make AccComp container
				       G4bool       vertical = true,       // is it a vertical kicker?
				       G4Material*  outerMaterial = nullptr// material for outer volume
				       );
  
private:
  BDSMagnetOuterFactoryCylindrical(); //private constructor as singleton
  static BDSMagnetOuterFactoryCylindrical* _instance;

  //only really one function needed for this factory
  //private to this factory only
  void CreateCylindricalSolids(G4String     name,                  // name
			       G4double     length,                // full length
			       BDSBeamPipe* beamPipe,              // beampipe
			       G4double     magnetContainerLength, // magnet object container full length
			       G4double     boxSize);              // full width

  void CreateCylindricalSolidsAngled(G4String      name,                  // name
				     G4double      length,                // full length
				     BDSBeamPipe*  beamPipe,              // beampipe
				     G4double      magnetContainerLength, // magnet object container full length
				     G4double      boxSize);              // full width

  /// Only the solids are unique, once we have those, the logical volumes and placement in the
  /// container are the same.  Group all this functionality together
  BDSMagnetOuter* CommonFinalConstructor(G4String    name,
					 G4double    length,        // length
					 G4double    outerDiameter, // full width
					 G4Material* outerMaterial,
					 G4Colour*   colour);

  /// test inputs for no null pointers or overlapping volumes due to poorly defined sizes
  void TestInputParameters(BDSBeamPipe* beamPipe,
			   G4double&    boxSizeIn,
			   G4Material*& outerMaterialIn);
  
};

#endif
