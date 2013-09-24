/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 

#include "BDSDriftStepper.hh"
#include "G4ThreeVector.hh"
#include "G4LineSection.hh"
#include "G4TransportationManager.hh"

extern G4int event_number;


BDSDriftStepper::BDSDriftStepper(G4Mag_EqRhs *EqRhs)
   : G4MagIntegratorStepper(EqRhs,6)  // integrate over 6 variables only !!
                                       // position & velocity
{}


void BDSDriftStepper::AdvanceHelix( const G4double  yIn[],
				    G4ThreeVector /*Bfld*/,
				    G4double  h,
				    G4double  yDrift[])
{
  G4ThreeVector positionMove, endTangent;

  const G4double *pIn = yIn+3;
  G4ThreeVector v0= G4ThreeVector( pIn[0], pIn[1], pIn[2]);  


  G4double InitMag=v0.mag();

      positionMove  = (h/InitMag) * v0;


      yDrift[0]   = yIn[0] + positionMove.x(); 
      yDrift[1]   = yIn[1] + positionMove.y(); 
      yDrift[2]   = yIn[2] + positionMove.z(); 
				
      yDrift[3] = v0.x();
      yDrift[4] = v0.y();
      yDrift[5] = v0.z();


      // dump step information for particular event
      G4bool verboseStep       = BDSExecOptions::Instance()->GetVerboseStep();
      G4int verboseEventNumber = BDSExecOptions::Instance()->GetVerboseEventNumber();
      if(verboseStep && verboseEventNumber == event_number)
	{
	  int G4precision = G4cout.precision();
	  G4cout.precision(10);
	  G4cout<<" h="<<h/CLHEP::m<<G4endl;
	  G4cout<<"xIn="<<yIn[0]/CLHEP::m<<" yIn="<<yIn[1]/CLHEP::m<<
	    " zIn="<<yIn[2]/CLHEP::m<<" v0="<<v0<<G4endl;
	  G4cout<<"xOut="<<yDrift[0]/CLHEP::m<<" yOut="<<yDrift[1]/CLHEP::m<<
	    "zOut="<<yDrift[2]/CLHEP::m<<G4endl;
	  // set precision back
	  G4cout.precision(G4precision);
	}
}

void BDSDriftStepper::Stepper( const G4double yInput[],
		     const G4double[],
		     const G4double hstep,
		     G4double yOut[],
		     G4double yErr[]      )
{  
   const G4int nvar = 6 ;

   G4int i;
   for(i=0;i<nvar;i++) yErr[i]=0;

   //   G4cout<<G4endl;
   //     G4cout<<"yInput="<<yInput[0]<<" "<< yInput[1]<<" "<<yInput[2]<<G4endl;
   AdvanceHelix(yInput,(G4ThreeVector)0,hstep,yOut);

   //     G4cout<<"yOut="<<yOut[0]<<" "<< yOut[1]<<" "<<yOut[2]<<G4endl;

   //   G4cout<<"hstep="<<hstep<<G4endl;
}

G4double BDSDriftStepper::DistChord()   const 
{
return 0;
}

BDSDriftStepper::~BDSDriftStepper()
{}
