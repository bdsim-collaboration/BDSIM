/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Changed StringFromInt to be the BDSGlobal version
*/

#include "BDSGlobalConstants.hh" 

#include "BDSOctupole.hh"

#include "BDSOctMagField.hh"
#include "BDSOctStepper.hh"

#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

//============================================================

BDSOctupole::BDSOctupole(G4String aName, G4double aLength, 
			 G4double bpRad, G4double FeRad,
			 G4double BTrpPrime, G4double tilt, 
			 G4double outR, 
                         std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                         G4String aTunnelMaterial, G4String aMaterial):
  BDSMultipole(aName, aLength, bpRad, FeRad, blmLocZ, blmLocTheta, aTunnelMaterial, aMaterial),
  itsBTrpPrime(BTrpPrime)
{
  SetOuterRadius(outR);
  itsTilt=tilt;
}

void BDSOctupole::Build() {
  BDSMultipole::Build();
  if(BDSGlobalConstants::Instance()->GetIncludeIronMagFields())
    {
      G4double polePos[4];
      G4double Bfield[3];
      
      //coordinate in GetFieldValue
      polePos[0]=-BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*sin(CLHEP::pi/8);
      polePos[1]=BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*cos(CLHEP::pi/8);
      polePos[2]=0.;
      polePos[3]=-999.;//flag to use polePos rather than local track
      
      itsMagField->GetFieldValue(polePos,Bfield);
      G4double BFldIron=
	sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
	BDSGlobalConstants::Instance()->GetMagnetPoleSize()/
	(BDSGlobalConstants::Instance()->GetComponentBoxSize()/2-
	 BDSGlobalConstants::Instance()->GetMagnetPoleRadius());
      
      // Magnetic flux from a pole is divided in two directions
      BFldIron/=2.;
      
      BuildOuterFieldManager(8, BFldIron,CLHEP::pi/8);
    }
}

void BDSOctupole::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0,1,1));
  itsVisAttributes->SetForceSolid(true);
}

void BDSOctupole::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  itsMagField=new BDSOctMagField(itsBTrpPrime);
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);
  
  itsStepper=new BDSOctStepper(itsEqRhs);
  BDSOctStepper* octStepper = dynamic_cast<BDSOctStepper*>(itsStepper);
  octStepper->SetBTrpPrime(itsBTrpPrime);
}

BDSOctupole::~BDSOctupole()
{
}
