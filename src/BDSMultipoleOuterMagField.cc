/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 25.12.2003
   Copyright (c) 2003 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSGlobalConstants.hh" 

#include "globals.hh"
#include "BDSMultipoleOuterMagField.hh"

#include "G4Navigator.hh"
#include "G4TransportationManager.hh"

BDSMultipoleOuterMagField::BDSMultipoleOuterMagField(G4int nPoles,
						     G4double aField,
						     G4double aPhiOffset)
  :itsNPoles(nPoles),itsIronField(aField),itsPhiOffset(aPhiOffset)
{itsSectorPhi=CLHEP::twopi/G4double(nPoles);
// G4cout<<" BDSMultipoleOuterMagField: itsIronField="<<itsIronField/tesla<<G4endl;
}

BDSMultipoleOuterMagField::~BDSMultipoleOuterMagField(){}

void BDSMultipoleOuterMagField::GetFieldValue(const G4double *Point,
					      G4double *Bfield ) const
{
  G4Navigator* MultipoleOuterNavigator=
    G4TransportationManager::GetTransportationManager()->
    GetNavigatorForTracking();

  // gab_dec03>>  
  G4ThreeVector GlobalPosition= G4ThreeVector( Point[0], Point[1], Point[2]);  
  G4AffineTransform GlobalAffine=MultipoleOuterNavigator->
    GetGlobalToLocalTransform();  
  //  G4ThreeVector LocalR = MultipoleOuterNavigator->				//			       GetCurrentLocalCoordinate();
  G4ThreeVector LocalR=GlobalAffine.TransformPoint(GlobalPosition); 
  // gab_dec03<<

  G4double BFactor=itsIronField/LocalR.mag();

  G4double phi = LocalR.phi()-itsPhiOffset;
  // extra term for dipoles, because of rotation required during positioning
  // of trapezoids
  if (itsNPoles==2)phi+=CLHEP::pi;

  // define sectors relative to the y-axis
  phi=CLHEP::halfpi-phi;

  if(phi<0)phi+=CLHEP::twopi;
  if(phi>CLHEP::twopi)phi-=CLHEP::twopi;

  G4int nSector=G4int(phi/itsSectorPhi);

  BFactor*=pow(-1.0,nSector);


  Bfield[0]= LocalR.y()*BFactor;
  Bfield[1]= -LocalR.x()*BFactor;
  Bfield[2]=0;

  // extra term for dipoles, because of rotation required during positioning
  // of trapezoids
  if (itsNPoles==2)Bfield[1]*=-1;

}



