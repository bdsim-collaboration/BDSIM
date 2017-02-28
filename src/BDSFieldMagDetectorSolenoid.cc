#include "BDSFieldMagDetectorSolenoid.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include <cmath>

BDSFieldMagDetectorSolenoid::BDSFieldMagDetectorSolenoid(G4double BIn,
							 G4double BOut,
							 G4double radiusIn,
							 G4double radiusOut,
							 G4double zMin,
							 G4double zMax):
  itsBIn(BIn),
  itsBOut(BOut),
  itsRadiusIn(radiusIn),
  itsRadiusOut(radiusOut),
  itsZMin(zMin),
  itsZMax(zMax)
{;}

G4ThreeVector BDSFieldMagDetectorSolenoid::GetField(const G4ThreeVector &position,
						    const G4double       /*t*/) const
{
  G4ThreeVector result(0,0,0);
  G4double zField   = 0;
  G4double localRad = std::sqrt(std::pow(position.y(),2)+std::pow(position.x(),2));

  if( (position.z() > itsZMin) && (position.z() < itsZMax) )
    {
      if(localRad<itsRadiusIn)
	{zField = itsBIn;}
      else if(localRad<itsRadiusOut)
	{zField = itsBOut;}
      else
	{zField=0;}
    }
  else
    {zField=0;}

  result[2] = zField;
  return result;
}



