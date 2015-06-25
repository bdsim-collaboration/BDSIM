/* BDSIM

19 May 2008 by Marchiori G.
17 Apr 2006 by Ilya Agapov

*/

#ifndef BDSMULTIPOLEMAGFIELD_H
#define BDSMULTIPOLEMAGFIELD_H

#include <list>
#include "G4Types.hh"
#include "G4MagneticField.hh"

class BDSMultipoleMagField: public G4MagneticField
{
public:  // with description

  BDSMultipoleMagField(std::list<G4double> knl, std::list<G4double> ksl);
  virtual ~BDSMultipoleMagField();
  // Constructor and destructor. No actions.


  virtual void  GetFieldValue( const G4double *Point,
			       G4double *Bfield ) const;

public:
  
  std::list<G4double> bn; // field normal coefficients = kn * Brho
  std::list<G4double> bs; // field skew coefficients = ks * Brho

};


#endif 
