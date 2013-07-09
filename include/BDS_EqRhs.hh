/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
// class BDS_EqRhs

#ifndef BDSEQRHS_DEF
#define BDSEQRHS_DEF

#include "globals.hh"
#include "G4EquationOfMotion.hh"
#include "G4MagneticField.hh"   
#include "G4Mag_EqRhs.hh"
#include "BDSSbendMagField.hh"

class BDS_EqRhs: public G4Mag_EqRhs
{
public: // with description

  BDS_EqRhs( BDSSbendMagField *magField );
  virtual ~BDS_EqRhs();

  G4double GetKappa();
  G4double GetLocalRadius();
  void EvaluateRhsGivenB( const  G4double y[],
			  const  G4double B[3],
			  G4double dydx[] ) const;
 
protected:
  BDSSbendMagField* itsField;

// private:
//   G4double LocalRadius;
};

inline G4double BDS_EqRhs::GetLocalRadius()
{
  return itsField->GetLocalRadius();
}

inline void BDS_EqRhs::EvaluateRhsGivenB( const  G4double /*y*/[],
					  const  G4double /*B*/[3],
					  G4double /*dydx*/[] ) const
{;}

#endif /* BDSEQRHS_DEF */
