/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 25.12.2003
   Copyright (c) 2003 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSGlobalConstants.hh" 

#include "BDSSkewSextMagField.hh"

#include "G4Navigator.hh"
#include "G4TransportationManager.hh"

BDSSkewSextMagField::BDSSkewSextMagField(G4double aBDblePrime):
  itsBDblePrime(aBDblePrime){}

BDSSkewSextMagField::~BDSSkewSextMagField(){}

void BDSSkewSextMagField::GetFieldValue( const G4double Point[4],
		       G4double *Bfield ) const
{
  G4Navigator* SkewSextNavigator=
    G4TransportationManager::GetTransportationManager()->
    GetNavigatorForTracking();

 // gab_dec03>>
  G4ThreeVector LocalR, GlobalR;
  GlobalR.setX(Point[0]);
  GlobalR.setY(Point[1]);
  GlobalR.setZ(Point[2]);
  // LocalR = SkewSextNavigator->GetCurrentLocalCoordinate();
  G4AffineTransform GlobalAffine=SkewSextNavigator->
    GetGlobalToLocalTransform();
  LocalR=GlobalAffine.TransformPoint(GlobalR); 
  // gab_dec03<<


  Bfield[0]=(LocalR.x()*LocalR.x()-LocalR.y()*LocalR.y())*itsBDblePrime/2;
  Bfield[1]=-2*LocalR.x()*LocalR.y()*itsBDblePrime/2;
  Bfield[2]=0;

  // factor of 2 is actually 2-factorial.
}



