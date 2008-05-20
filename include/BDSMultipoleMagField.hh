/* BDSIM

19 May 2008 by Marchiori G.
17 Apr 2006 by Ilya Agapov

*/

//==============================================================


#ifndef BDSMultipoleMagField_h
#define BDSMultipoleMagField_h

#include "G4Types.hh"
#include "G4MagneticField.hh"

class BDSMultipoleMagField : public G4MagneticField
{
public:  // with description

  BDSMultipoleMagField(list<G4double> knl, list<G4double> ksl);
  virtual ~BDSMultipoleMagField();
  // Constructor and destructor. No actions.


  virtual void  GetFieldValue( const G4double *Point,
			       G4double *Bfield ) const;

public:
  
  list<G4double> bn; // field normal coefficients = kn * Brho
  list<G4double> bs; // field skew coefficients = ks * Brho

};


#endif 
