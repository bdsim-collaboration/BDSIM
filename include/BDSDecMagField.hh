#ifndef BDSDECMAGFIELD
#define BDSDECMAGFIELD
/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#include "G4Types.hh"
#include "BDSMagField.hh"

class BDSDecMagField : public BDSMagField
{
public:  // with description

  BDSDecMagField(G4double aBQuadPrime);
  virtual ~BDSDecMagField();
  // Constructor and destructor. No actions.


  virtual void  GetFieldValue( const G4double Point[4],
			       G4double *Bfield ) const;
  void SetBQuadPrime(G4double aBQuadPrime);
private:
  G4double itsBQuadPrime;
};

inline void BDSDecMagField::SetBQuadPrime(G4double aBQuadPrime)
{ itsBQuadPrime = aBQuadPrime;}

#endif /* BDSDECMAGFIELD */
