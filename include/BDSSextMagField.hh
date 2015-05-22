/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#ifndef BDSSEXTMAGFIELD
#define BDSSEXTMAGFIELD

#include "G4Types.hh"
#include "BDSMagField.hh"

class BDSSextMagField : public BDSMagField
{
public:  // with description

  BDSSextMagField(G4double aBDblePrime);
  virtual ~BDSSextMagField();
  // Constructor and destructor. No actions.


  virtual void  GetFieldValue( const G4double Point[4],
			       G4double *Bfield ) const;
  void SetBDblPrime(G4double aBDblePrime);
private:
  G4double itsBDblePrime;

};

inline void BDSSextMagField::SetBDblPrime(G4double aBDblePrime)
{ itsBDblePrime = aBDblePrime; }

#endif /* BDSSEXTMAGFIELD */
