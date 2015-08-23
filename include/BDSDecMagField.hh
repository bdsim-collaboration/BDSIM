#ifndef BDSDECMAGFIELD_H
#define BDSDECMAGFIELD_H

#include "BDSAuxiliaryNavigator.hh"

#include "G4Types.hh"
#include "G4MagneticField.hh"

class BDSDecMagField: public G4MagneticField, public BDSAuxiliaryNavigator
{
public:
  BDSDecMagField(G4double aBQuadPrime);
  virtual ~BDSDecMagField();

  virtual void  GetFieldValue( const G4double Point[4],
			       G4double *Bfield ) const;
  void SetBQuadPrime(G4double aBQuadPrime);
private:
  G4double itsBQuadPrime;
};

inline void BDSDecMagField::SetBQuadPrime(G4double aBQuadPrime)
{ itsBQuadPrime = aBQuadPrime;}

#endif /* BDSDECMAGFIELD */
