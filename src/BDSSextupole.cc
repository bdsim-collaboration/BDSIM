/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#include "BDSSextupole.hh"

#include "BDSGlobalConstants.hh" 
#include "BDSBeamPipeInfo.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetType.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSSextMagField.hh"
#include "BDSSextStepper.hh"

#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

class BDSTiltOffset;

BDSSextupole::BDSSextupole(G4String           name,
			   G4double           length,
			   G4double           bDblPrime,
			   BDSBeamPipeInfo*   beamPipeInfo,
			   BDSMagnetOuterInfo magnetOuterInfo):
  BDSMagnet(BDSMagnetType::sextupole, name, length,
	    beamPipeInfo, magnetOuterInfo),
   itsBDblPrime(bDblPrime)
{;}

void BDSSextupole::Build()
{
  BDSMagnet::Build();
  if(BDSGlobalConstants::Instance()->GetIncludeIronMagFields())
    {
      G4double polePos[4];
      G4double Bfield[3];
      
      //coordinate in GetFieldValue
      polePos[0]=-BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*sin(CLHEP::pi/6);
      polePos[1]=BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*cos(CLHEP::pi/6);
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
      
      BuildOuterFieldManager(6, BFldIron,CLHEP::pi/6);
    }
}

void BDSSextupole::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  itsMagField=new BDSSextMagField(1*itsBDblPrime); //L Deacon testing field sign 4/7/12
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);

  BDSSextStepper* sextStepper=new BDSSextStepper(itsEqRhs);
  sextStepper->SetBDblPrime(itsBDblPrime);
  itsStepper = sextStepper;
}
