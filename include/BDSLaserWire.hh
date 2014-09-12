/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSLaserWire_h
#define BDSLaserWire_h 1

#include"globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"

#include "G4FieldManager.hh"
#include "BDSLaserCompton.hh"

class BDSLaserWire :public BDSAcceleratorComponent
{
public:
  BDSLaserWire(G4String aName,G4double aLength,
	       G4double aWavelength, G4ThreeVector aDirection);
  ~BDSLaserWire();

  inline void SetLaserDirection(G4ThreeVector aDirection);
  inline G4ThreeVector GetLaserDirection();

  inline void SetLaserWavelength(G4double aWavelength);
  inline G4double GetLaserWavelength();

private:
  virtual void BuildMarkerLogicalVolume();
  virtual void SetVisAttributes();

  BDSLaserCompton* itsLaserCompton; // not used atm, JS
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
