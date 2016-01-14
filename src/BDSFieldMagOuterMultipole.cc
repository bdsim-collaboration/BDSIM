#include "BDSFieldMagOuterMultipole.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSFieldMagOuterMultipole::BDSFieldMagOuterMultipole(const G4int    nPolesIn,
						     const G4double fieldStrengthIn,
						     const G4double phiOffset):
  nPoles(nPolesIn),
  fieldStrength(fieldStrengthIn),
  phiOffset(phiOffset)
{
  itsSectorPhi=CLHEP::twopi/G4double(nPoles);
}

BDSFieldMagOuterMultipole::~BDSFieldMagOuterMultipole(){}

void BDSFieldMagOuterMultipole::GetFieldValue(const G4double point[4],
					      G4double* field) const
{
  G4ThreeVector localPosition = ConvertToLocal(point);
  G4double      BFactor       = fieldStrength/localPosition.mag();
  G4double      phi           = localPosition.phi() - phiOffset;

  // extra term for dipoles, because of rotation required during positioning
  // of trapezoids
  if (nPoles==2)
    {phi += CLHEP::pi;}

  // define sectors relative to the y-axis
  phi = CLHEP::halfpi - phi;

  if(phi < 0)
    {phi += CLHEP::twopi;}
  if(phi > CLHEP::twopi)
    {phi -= CLHEP::twopi;}

  G4int nSector = G4int(phi/itsSectorPhi);

  BFactor *= pow(-1.0,nSector);

  G4ThreeVector localField;
  localField[0] = localPosition.y()*BFactor;
  localField[1] = -localPosition.x()*BFactor;
  localField[2] = 0;

  // extra term for dipoles, because of rotation required during positioning
  // of trapezoids
  if (nPoles==2)
    {localField[1] *= -1;}
  
  OutputGlobalField(localField, field);
}



