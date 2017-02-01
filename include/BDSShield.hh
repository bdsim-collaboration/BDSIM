#ifndef BDSSHIELD_H
#define BDSSHIELD_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 types / globals

class BDSBeamPipeInfo;

class G4Material;

/**
 * @brief A square mask of solid material with a square aperture.
 *
 * Optionally may have a piece of beam pipe inside it.
 *
 * @author Laurie Nevay
 */

class BDSShield: public BDSAcceleratorComponent
{
public:
  BDSShield(G4String         name,
	    G4double         length,
	    G4double         outerDiameterIn,
	    G4double         xAper,
	    G4double         yAper,
	    G4Material*      materialIn,
	    BDSBeamPipeInfo* beamPipeInfoIn = nullptr);
  virtual ~BDSShield();

protected:
  /// Build a container volume for everything.
  virtual void BuildContainerLogicalVolume();

  /// Do common BDSAcceleratorComponent::Build then build and place the shield,
  /// then the beampipe if it exists.
  virtual void Build();

private:
  BDSShield() = delete;
  
  /// Build the outer shield geoemtry.
  void BuildShield();

  /// Build a beam pipe in the hole if required.
  void BuildBeamPipe();
  
  G4double         outerDiameter; ///< Outer size of shield.
  G4double         xAper;         ///< Inner horizontal full width of shield.
  G4double         yAper;         ///< Inner vertical full width of shield.
  G4Material*      material;      ///< Shield material.
};

#endif
