#ifndef BDSSEXTMAGFIELD
#define BDSSEXTMAGFIELD

#include "G4Types.hh"
#include "G4MagneticField.hh"

#include "BDSAuxiliaryNavigator.hh"

class BDSSextMagField: public G4MagneticField, BDSAuxiliaryNavigator
{
public:
  BDSSextMagField(G4double aBDblePrime);
  virtual ~BDSSextMagField();
  
  virtual void  GetFieldValue(const G4double Point[4],
			      G4double *Bfield) const;
  
  void SetBDblPrime(G4double aBDblePrime);
  
private:
  G4double itsBDblePrime;
};

inline void BDSSextMagField::SetBDblPrime(G4double aBDblePrime)
{ itsBDblePrime = aBDblePrime; }

#endif /* BDSSEXTMAGFIELD */
