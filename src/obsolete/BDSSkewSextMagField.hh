/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#ifndef BDSSKEWSEXTMAGFIELD
#define BDSSKEWSEXTMAGFIELD

#include "G4Types.hh"
#include "G4MagneticField.hh"

class BDSSkewSextMagField : public G4MagneticField
{
public:  // with description

  BDSSkewSextMagField(G4double aBDblePrime);
  virtual ~BDSSkewSextMagField();
  // Constructor and destructor. No actions.


  virtual void  GetFieldValue( const G4double Point[4],
			       G4double *Bfield ) const;

private:
  G4double itsBDblePrime;

};


#endif /* BDSSKEWSEXTMAGFIELD */
