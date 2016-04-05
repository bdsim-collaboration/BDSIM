#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSIntegratorTeleporter.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

BDSIntegratorTeleporter::BDSIntegratorTeleporter(G4Mag_EqRhs* eqRHSIn,
						 G4ThreeVector teleporterDeltaIn):
  BDSIntegratorBase(eqRHSIn, 6),
  teleporterDelta(teleporterDeltaIn)
{;}

void BDSIntegratorTeleporter::Stepper(const G4double yIn[],
				      const G4double /*dxdy*/[],
				      const G4double h,
				      G4double yOut[],
				      G4double yErr[])
{
  for(G4int i = 0; i < nVariables; i++)
    {yErr[i] = 0;}

  G4int turnstaken = BDSGlobalConstants::Instance()->TurnsTaken();
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "turnstaken: " << turnstaken << G4endl;
#endif
  // has to have completed at least 1 turn and be going forwards
  // must test for this to avoid backwards going particles getting stuck
  if (turnstaken > 0 && yIn[5] > 0)
    {
      G4ThreeVector localPosition = ConvertToLocal(yIn);
      G4ThreeVector localPositionAfter;
      localPositionAfter[0] = localPosition.x() - teleporterDelta.x();
      localPositionAfter[1] = localPosition.y() - teleporterDelta.y();
      localPositionAfter[2] = localPosition.z() + h;

      G4ThreeVector globalPosition = ConvertToGlobal(localPositionAfter);
      yOut[0] = globalPosition[0];
      yOut[1] = globalPosition[1];
      yOut[2] = globalPosition[2];
      yOut[3] = yIn[3];
      yOut[4] = yIn[4];
      yOut[5] = yIn[5];
    }
  else
    {
      // move particle to other side without affecting anything
      yOut[0] = yIn[0];
      yOut[1] = yIn[1];
      if (yIn[5] > 0) // going forwards
	{yOut[2] = yIn[2] + h;}
      else // going backwards
	{yOut[2] = yIn[2] - h;}
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
