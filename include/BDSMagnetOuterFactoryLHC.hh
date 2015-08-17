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
  virtual BDSMagnetOuter* CreateSectorBend(G4String     name,                  // name
					   G4double     length,                // length [mm]
					   BDSBeamPipe* beamPipe,              // beampipe
					   G4double     outerDiameter,         // full width
					   G4double     containerLength,       // full length to make AccComp container
					   G4double     angle,                 // full bend angle [rad]
					   G4Material*  outerMaterial = nullptr// material for outer volume
					   );

  /// rectangular bend outer volume
  virtual BDSMagnetOuter* CreateRectangularBend(G4String     name,                  // name
						G4double     length,                // length [mm]
						BDSBeamPipe* beamPipe,              // beampipe
						G4double     outerDiameter,         // full width
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
protected:
  G4bool isLeftOffset;
  G4bool IsLeftOffset(){return isLeftOffset;}
  
private:
  /// test inputs for no null pointers or overlapping volumes due to poorly defined sizes
  void TestInputParameters(BDSBeamPipe* beamPipe,
			   G4double&    boxSizeIn,
			   G4Material*& outerMaterialIn);
  
};

#endif
