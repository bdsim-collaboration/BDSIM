#ifndef BDSLASERWIRE_H
#define BDSLASERWIRE_H

#include "globals.hh"  // geant4 types / globals
#include "BDSAcceleratorComponent.hh"

/**
 * @brief A laser wire scanner.
 * 
 * This builds a box with the default aperture in BDSGlobalConstants
 * and with the desired length. The box is the sole piece of geometry
 * and contains the special "LaserVac" material the provides the 'target'
 * of photons from the laser that the beam will scatter from.
 */

class BDSLaserWire :public BDSAcceleratorComponent
{
public:
  BDSLaserWire(G4String aName,
	       G4double aLength,
	       G4double aWavelength,
	       G4ThreeVector aDirection);
  virtual ~BDSLaserWire();

  inline void SetLaserDirection(G4ThreeVector aDirection);
  inline G4ThreeVector GetLaserDirection();

  inline void SetLaserWavelength(G4double aWavelength);
  inline G4double GetLaserWavelength();

private:
  virtual void BuildContainerLogicalVolume();

  G4ThreeVector itsLaserDirection;
  G4double itsLaserWavelength;
};

inline void BDSLaserWire::SetLaserDirection(G4ThreeVector aDirection)
{itsLaserDirection=aDirection;}

inline G4ThreeVector BDSLaserWire::GetLaserDirection()
{return itsLaserDirection;}

inline void BDSLaserWire::SetLaserWavelength(G4double aWavelength)
{itsLaserWavelength=aWavelength;}

inline G4double BDSLaserWire::GetLaserWavelength()
{return itsLaserWavelength;}

#endif
