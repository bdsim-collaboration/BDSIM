#ifndef BDSFIELDMAGDECAPOLE_H
#define BDSFIELDMAGDECAPOLE_H

#include "BDSAuxiliaryNavigator.hh"

#include "G4Types.hh"
#include "G4MagneticField.hh"

/**
 * @brief Decapole magnetic field.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSFieldMagDecapole: public G4MagneticField, public BDSAuxiliaryNavigator
{
public:
  BDSFieldMagDecapole(G4double aBQuadPrime);
  virtual ~BDSFieldMagDecapole();

  virtual void  GetFieldValue( const G4double Point[4],
			       G4double *Bfield ) const;
  void SetBQuadPrime(G4double aBQuadPrime);
private:
  G4double itsBQuadPrime;
};

inline void BDSFieldMagDecapole::SetBQuadPrime(G4double aBQuadPrime)
{ itsBQuadPrime = aBQuadPrime;}

#endif
