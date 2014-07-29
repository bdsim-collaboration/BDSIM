#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 
#include "globals.hh"

#include "BDSDriftStepper.hh"
#include "G4ThreeVector.hh"
#include "BDSTeleporterStepper.hh"
#include "BDSDebug.hh"

extern G4int event_number;

BDSTeleporterStepper::BDSTeleporterStepper(G4Mag_EqRhs *EqRhs)
   : G4MagIntegratorStepper(EqRhs,6)  // integrate over 6 variables only !!
                                       // position & velocity
{
  //verboseStep        = BDSExecOptions::Instance()->GetVerboseStep();
  verboseStep        = true;
  verboseEventNumber = BDSExecOptions::Instance()->GetVerboseEventNumber();
  nvar               = 6;
  turnnumberrecord.push_back((G4int)-1);
  teleporterdelta    = BDSGlobalConstants::Instance()->GetTeleporterDelta();
  //#ifdef DEBUG
  G4cout << "BDSTeleporterStepper Constructor " << G4endl;
  G4cout << "teleporterdelta " << teleporterdelta/CLHEP::m << " m" << G4endl;
  //#endif
}


void BDSTeleporterStepper::AdvanceHelix( const G4double  yIn[],
					 G4ThreeVector /*Bfld*/,
					 G4double  h,
					 G4double  yOut[])
{
  // keep a record of the turn number for this particle
  // only adjust it's position if the particle on this turn
  // hasn't been shifted already - ie its turn number
  // won't have been registered in the vector member
  // turnnumberrecord
  turnstaken = BDSGlobalConstants::Instance()->GetTurnsTaken();
  G4cout << " TELEPORTER STEPPER BEING USED" << G4endl;
  
#ifdef DEBUG
  G4cout << "turnstaken " << turnstaken << G4endl;
#endif
  //  if (turnstaken == turnnumberrecord.back())
  if (1 == 1)
    {
      //do the adjustment
      //register this turn
      turnnumberrecord.push_back(turnstaken);
      G4cout << "registering this turn in teleporter" << G4endl;
      // accelerator is built along z axis
      // bends bend normally in x axis
      // for a flat machine, the delta.y() should be <10^-9 ie rounding error 
      yOut[0] = yIn[0] + teleporterdelta.x();
      yOut[1] = yIn[1] + teleporterdelta.y();
      yOut[2] = yIn[2] + h; // move it along the step length
      yOut[3] = yIn[3];
      yOut[4] = yIn[4];
      yOut[5] = yIn[5];
    }
  else
    {
      //move the particle along the step without affecting anyting
      yOut[0] = yIn[0];
      yOut[1] = yIn[1];
      yOut[2] = yIn[2] + h;
      yOut[3] = yIn[3];
      yOut[4] = yIn[4];
      yOut[5] = yIn[5];
    }
  
  // dump step information for particular event
  //if(verboseStep || verboseEventNumber == event_number)
  if(1 == 1)
    {
      G4ThreeVector inA  = G4ThreeVector(yIn[0],yIn[1],yIn[2]);
      G4ThreeVector inB  = G4ThreeVector(yIn[3],yIn[4],yIn[5]);
      G4ThreeVector outA = G4ThreeVector(yOut[0],yOut[1],yOut[2]);
      G4ThreeVector outB = G4ThreeVector(yOut[3],yOut[4],yOut[5]);
      int G4precision = G4cout.precision();
      G4cout.precision(10);
      G4cout << __METHOD_NAME__ << G4endl;
      G4cout << "h (step length) " << h/CLHEP::m << G4endl;
      G4cout << "Input x,y,z     " << inA/CLHEP::m << G4endl;
      G4cout << "Input px,py,pz  " << inB/CLHEP::m << G4endl;
      G4cout << "Output x,y,z    " << outA/CLHEP::m << G4endl;
      G4cout << "Output px,py,pz " << outB/CLHEP::m << G4endl;
      G4cout.precision(G4precision);
    }
}

void BDSTeleporterStepper::Stepper( const G4double yInput[],
				    const G4double[],
				    const G4double hstep,
				    G4double yOut[],
				    G4double yErr[])
{  
  //const G4int nvar = 6 ;
  G4int i;
  for(i=0;i<nvar;i++) yErr[i]=0;
  
  AdvanceHelix(yInput,(G4ThreeVector)0,hstep,yOut);
}

BDSTeleporterStepper::~BDSTeleporterStepper()
{}
