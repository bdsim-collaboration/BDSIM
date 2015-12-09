#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSTeleporterStepper.hh"

#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"

BDSTeleporterStepper::BDSTeleporterStepper(G4Mag_EqRhs* eqRHS):
  G4MagIntegratorStepper(eqRHS, 6)
{
#ifdef BDSDEBUG
  G4cout << "BDSTeleporterStepper Constructor " << G4endl;
#endif
  verboseStep        = BDSExecOptions::Instance()->GetVerboseStep();
  verboseEventNumber = BDSExecOptions::Instance()->GetVerboseEventNumber();
  nvar               = 6;
  teleporterdelta    = BDSGlobalConstants::Instance()->GetTeleporterDelta();
#ifdef BDSDEBUG
  verboseStep = true;
#endif
}

void BDSTeleporterStepper::Stepper(const G4double yIn[],
				   const G4double /*dxdy*/[],
				   const G4double h,
				   G4double yOut[],
				   G4double yErr[])
{
  for(G4int i=0;i<nvar;i++)
    {yErr[i]=0;}

  G4int turnstaken = BDSGlobalConstants::Instance()->GetTurnsTaken();
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "turnstaken: " << turnstaken << G4endl;
#endif
  //if (turnstaken != turnnumberrecord.back())
  if (turnstaken > 0)
    {
      //do the adjustment
      //register this turn
      //turnnumberrecord.push_back(turnstaken);
      //G4cout << "registering this turn in teleporter" << G4endl;
      // accelerator is built along z axis
      // bends bend normally in x axis
      // for a flat machine, the delta.y() should be <10^-9 ie rounding error 
      yOut[0] = yIn[0] - teleporterdelta.x();
      yOut[1] = yIn[1] - teleporterdelta.y();
      yOut[2] = yIn[2] + h; // move it along the step length
      yOut[3] = yIn[3];
      yOut[4] = yIn[4];
      yOut[5] = yIn[5];
    }
  else
    {
      //move the particle along the step without affecting anything (ensure periodicity)
      yOut[0] = yIn[0];
      yOut[1] = yIn[1];
      yOut[2] = yIn[2] + h;
      yOut[3] = yIn[3];
      yOut[4] = yIn[4];
      yOut[5] = yIn[5];
    }
  
#ifdef BDSDEBUG
  G4ThreeVector inA  = G4ThreeVector(yIn[0],yIn[1],yIn[2]);
  G4ThreeVector inB  = G4ThreeVector(yIn[3],yIn[4],yIn[5]);
  G4ThreeVector outA = G4ThreeVector(yOut[0],yOut[1],yOut[2]);
  G4ThreeVector outB = G4ThreeVector(yOut[3],yOut[4],yOut[5]);
  std::ios_base::fmtflags ff = G4cout.flags(); // save cout flags
  G4cout.precision(10);
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << "h (step length) " << h   /CLHEP::m << G4endl;
  G4cout << "Input x,y,z     " << inA /CLHEP::m << G4endl;
  G4cout << "Input px,py,pz  " << inB /CLHEP::m << G4endl;
  G4cout << "Output x,y,z    " << outA/CLHEP::m << G4endl;
  G4cout << "Output px,py,pz " << outB/CLHEP::m << G4endl;
  G4cout.flags(ff); // reset cout flags
#endif
}

BDSTeleporterStepper::~BDSTeleporterStepper()
{}
