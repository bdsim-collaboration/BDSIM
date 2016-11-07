/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSLASERWIRE_H
#define BDSLASERWIRE_H

#include "globals.hh"  // geant4 types / globals
#include "BDSAcceleratorComponent.hh"

class BDSLaserWire :public BDSAcceleratorComponent
{
public:
  BDSLaserWire(G4String aName,
	       G4double aLength,
	       G4double aWavelength,
	       G4ThreeVector aDirection);
  ~BDSLaserWire();

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
