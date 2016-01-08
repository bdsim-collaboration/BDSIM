#ifndef BDSFIELDMAGOCTUPOLE_H
#define BDSFIELDMAGOCTUPOLE_H

#include "G4Types.hh"
#include "G4MagneticField.hh"

#include "BDSAuxiliaryNavigator.hh"

class BDSFieldMagOctupole: public G4MagneticField, public BDSAuxiliaryNavigator
{
public:
  BDSFieldMagOctupole(G4double aBTrpPrime);
  virtual ~BDSFieldMagOctupole();
  
  virtual void  GetFieldValue(const G4double Point[4],
			      G4double *Bfield ) const;
  
  void SetBTrpPrime(G4double aBTrpPrime);
  
private:
  G4double itsBTrpPrime;
};

inline void BDSFieldMagOctupole::SetBTrpPrime(G4double aBTrpPrime)
{ itsBTrpPrime = aBTrpPrime;}

#endif
