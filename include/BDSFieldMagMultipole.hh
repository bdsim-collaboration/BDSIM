#ifndef BDSFIELDMAGMULTIPOLE_H
#define BDSFIELDMAGMULTIPOLE_H

#include <list>
#include "G4Types.hh"
#include "G4MagneticField.hh"

#include "BDSAuxiliaryNavigator.hh"

class BDSFieldMagMultipole: public G4MagneticField, public BDSAuxiliaryNavigator
{
public:
  BDSFieldMagMultipole(std::list<G4double> knl, std::list<G4double> ksl);
  virtual ~BDSFieldMagMultipole();

  virtual void  GetFieldValue( const G4double *Point,
			       G4double *Bfield ) const;

private:
  std::list<G4double> bn; // field normal coefficients = kn * Brho
  std::list<G4double> bs; // field skew coefficients = ks * Brho
};


#endif 
