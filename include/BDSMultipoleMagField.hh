#ifndef BDSMULTIPOLEMAGFIELD_H
#define BDSMULTIPOLEMAGFIELD_H

#include <list>
#include "G4Types.hh"
#include "G4MagneticField.hh"

#include "BDSAuxiliaryNavigator.hh"

class BDSMultipoleMagField: public G4MagneticField, public BDSAuxiliaryNavigator
{
public:
  BDSMultipoleMagField(std::list<G4double> knl, std::list<G4double> ksl);
  virtual ~BDSMultipoleMagField();

  virtual void  GetFieldValue( const G4double *Point,
			       G4double *Bfield ) const;

private:
  std::list<G4double> bn; // field normal coefficients = kn * Brho
  std::list<G4double> bs; // field skew coefficients = ks * Brho
};


#endif 
