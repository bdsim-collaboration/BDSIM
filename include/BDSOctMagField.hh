/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#ifndef BDSOCTMAGFIELD
#define BDSOCTMAGFIELD

#include "G4Types.hh"
#include "BDSMagField.hh"

class BDSOctMagField : public BDSMagField
{
public:  // with description

  BDSOctMagField(G4double aBTrpPrime);
  virtual ~BDSOctMagField();
  // Constructor and destructor. No actions.


  virtual void  GetFieldValue( const G4double Point[4],
			       G4double *Bfield ) const;
  void SetBTrpPrime(G4double aBTrpPrime);
private:
  G4double itsBTrpPrime;

};

inline void BDSOctMagField::SetBTrpPrime(G4double aBTrpPrime)
{ itsBTrpPrime = aBTrpPrime;}

#endif /* BDSOCTMAGFIELD */
