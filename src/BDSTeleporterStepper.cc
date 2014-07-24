#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 

#include "BDSDriftStepper.hh"
#include "G4ThreeVector.hh"
#include "BDSTeleporterStepper.hh"
#include "BDSDebug.hh"

extern G4int event_number;

BDSTeleporterStepper::BDSTeleporterStepper(G4Mag_EqRhs *EqRhs)
   : G4MagIntegratorStepper(EqRhs,6)  // integrate over 6 variables only !!
                                       // position & velocity
{
  verboseStep        = BDSExecOptions::Instance()->GetVerboseStep();
  verboseEventNumber = BDSExecOptions::Instance()->GetVerboseEventNumber();
  nvar = 6;
}


void BDSTeleporterStepper::AdvanceHelix( const G4double  yIn[],
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
  if(verboseStep || verboseEventNumber == event_number)
    {
      int G4precision = G4cout.precision();
      G4cout.precision(10);
      G4cout << __METHOD_NAME__ << G4endl;
      G4cout << std::setw(12);
      G4cout << "h" << "xIn" << "yIn" << "zIn" << "v0" << "xOut" << "yOut" << "zOut" << G4endl;
      G4cout << h/CLHEP::m << yIn[0]/CLHEP::m << yIn[1]/CLHEP::m << yIn[2]/CLHEP::m;
      G4cout << v0 << yDrift[0]/CLHEP::m  << yDrift[1]/CLHEP::m  << yDrift[2]/CLHEP::m;
      G4cout << G4endl;
      // set precision back
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
