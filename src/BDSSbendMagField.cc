/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#include <limits>
#include "BDSGlobalConstants.hh"
#include "G4UniformMagField.hh"
#include "globals.hh"
#include "BDSSbendMagField.hh"

BDSSbendMagField::BDSSbendMagField(const G4ThreeVector& aField,
			       const G4double length,
			       const G4double angle) 
  :BDSUniformMagField(aField)
{
  if(angle!=0.)
    {    // original
      //    itsLocalRadius=length/angle;
      itsLocalRadius=-length/angle;// minus sign for correct machine convention
      // check for synchrotron radiation factors
#ifdef BDSDEBUG
      G4double B_inferred= 
        (BDSGlobalConstants::Instance()->GetBeamMomentum()/CLHEP::GeV)/
        (0.299792458 * (CLHEP::GeV/CLHEP::tesla/CLHEP::m)*itsLocalRadius/CLHEP::m);
      G4cout<<"B_inferred="<<B_inferred/CLHEP::tesla<<
	" aField="<<aField/CLHEP::tesla<<G4endl;
#endif
    } else {
    itsLocalRadius = std::numeric_limits<double>::max();
#ifdef BDSDEBUG
    G4double B_inferred=0;
    G4cout<<"B_inferred="<<B_inferred/CLHEP::tesla<<
      " aField="<<aField/CLHEP::tesla<<G4endl;
#endif
  }
}

BDSSbendMagField::~BDSSbendMagField()
{}


