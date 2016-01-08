#ifndef BDSFIELDMAGSEXTUPOLE_H
#define BDSFIELDMAGSEXTUPOLE_H

#include "G4Types.hh"
#include "G4MagneticField.hh"

#include "BDSAuxiliaryNavigator.hh"

class BDSFieldMagSextupole: public G4MagneticField, public BDSAuxiliaryNavigator
{
public:
  BDSFieldMagSextupole(G4double aBDblePrime);
  virtual ~BDSFieldMagSextupole();
  
  virtual void  GetFieldValue(const G4double Point[4],
			      G4double *Bfield) const;
  
  void SetBDblPrime(G4double aBDblePrime);
  
private:
  G4double itsBDblePrime;
};

inline void BDSFieldMagSextupole::SetBDblPrime(G4double aBDblePrime)
{ itsBDblePrime = aBDblePrime; }

#endif
