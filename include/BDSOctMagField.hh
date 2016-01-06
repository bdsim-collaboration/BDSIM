#ifndef BDSOCTMAGFIELD_H
#define BDSOCTMAGFIELD_H

#include "G4Types.hh"
#include "G4MagneticField.hh"

#include "BDSAuxiliaryNavigator.hh"

class BDSOctMagField: public G4MagneticField, public BDSAuxiliaryNavigator
{
public:
  BDSOctMagField(G4double aBTrpPrime);
  virtual ~BDSOctMagField();
  
  virtual void  GetFieldValue(const G4double Point[4],
			      G4double *Bfield ) const;
  
  void SetBTrpPrime(G4double aBTrpPrime);
  
private:
  G4double itsBTrpPrime;
};

inline void BDSOctMagField::SetBTrpPrime(G4double aBTrpPrime)
{ itsBTrpPrime = aBTrpPrime;}

#endif /* BDSOCTMAGFIELD */
