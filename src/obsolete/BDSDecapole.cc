/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#include "BDSGlobalConstants.hh" 

#include "BDSDecapole.hh"
#include "BDSDecMagField.hh"
#include "BDSDecStepper.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"

#include "G4Mag_UsualEqRhs.hh"

BDSDecapole::BDSDecapole(G4String            name,
			 G4double            length,
			 G4double            bQuadPrime,
			 BDSBeamPipeInfo*    beamPipeInfo,
			 BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(BDSMagnetType::decapole, name, length,
	    beamPipeInfo,magnetOuterInfo),
  itsBQuadPrime(bQuadPrime)
{;}

void BDSDecapole::Build() {
  BDSMagnet::Build();
  if(BDSGlobalConstants::Instance()->IncludeIronMagFields())
    {
      G4double polePos[4];
      G4double Bfield[3];
      
      //coordinate in GetFieldValue
      polePos[0]=-BDSGlobalConstants::Instance()->MagnetPoleRadius()*sin(CLHEP::pi/10);
      polePos[1]= BDSGlobalConstants::Instance()->MagnetPoleRadius()*cos(CLHEP::pi/10);
      polePos[2]=0.;
      polePos[3]=-999.;//flag to use polePos rather than local track
      
      itsMagField->GetFieldValue(polePos,Bfield);
      G4double BFldIron=
	sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
            BDSGlobalConstants::Instance()->MagnetPoleSize()/
	(BDSGlobalConstants::Instance()->ComponentBoxSize()/2-
            BDSGlobalConstants::Instance()->MagnetPoleRadius());
      
      // Magnetic flux from a pole is divided in two directions
      BFldIron/=2.;
      
      BuildOuterFieldManager(10, BFldIron,CLHEP::pi/10);
    }
}

void BDSDecapole::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  itsMagField=new BDSDecMagField(itsBQuadPrime);
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);
  
  BDSDecStepper* decStepper=new BDSDecStepper(itsEqRhs);
  decStepper->SetBQuadPrime(itsBQuadPrime);
  itsStepper = decStepper;
}
