
#ifndef BDSSynchrotronRadiation_h
#define BDSSynchrotronRadiation_h 1

#include "BDSGlobalConstants.hh"

#include "G4ios.hh" 
#include "globals.hh"
#include "Randomize.hh" 
#include "G4VDiscreteProcess.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4OrderedTable.hh" 
#include "G4PhysicsTable.hh"
#include "G4PhysicsLogVector.hh"
#include "BDSComptonEngine.hh"
#include "BDSMaterials.hh"
#include "Randomize.hh"

#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4FieldTrack.hh"

#include "G4MagIntegratorDriver.hh"

#include "G4Navigator.hh"
#include "G4AffineTransform.hh"

extern G4double BDSLocalRadiusOfCurvature;
 
class BDSSynchrotronRadiation : public G4VDiscreteProcess 
{ 
public:
 
  BDSSynchrotronRadiation(const G4String& processName = "BDSSynchRad");
 
  ~BDSSynchrotronRadiation();

  G4bool IsApplicable(const G4ParticleDefinition&);
     
  G4double GetMeanFreePath(const G4Track& track,
			   G4double previousStepSize,
			   G4ForceCondition* condition );
 
  G4VParticleChange *PostStepDoIt(const G4Track& track,         
				  const G4Step&  step);                 

  G4double SynGenC(G4double xmin);
  G4double SynRadC(G4double x);

protected:

private:

  // assignment and copy constructor not implemented nor used
  BDSSynchrotronRadiation & operator=(const BDSSynchrotronRadiation &right);
  BDSSynchrotronRadiation(const BDSSynchrotronRadiation&);

  G4double nExpConst;
  G4double CritEngFac;
  G4int MeanFreePathCounter;

private:
};

inline G4bool 
BDSSynchrotronRadiation::IsApplicable(const G4ParticleDefinition& particle)
{
  return(  (&particle == G4Electron::Electron())
	   ||(&particle == G4Positron::Positron()) );
}

inline G4double 
BDSSynchrotronRadiation::GetMeanFreePath(const G4Track& track,
					 G4double /*PreviousStepSize*/,
					 G4ForceCondition* ForceCondition)
{  
  *ForceCondition = NotForced ;

  //   static G4FieldManager* lastFieldManager;

  G4double MeanFreePath;
  G4FieldManager* TheFieldManager=
    track.GetVolume()->GetLogicalVolume()->GetFieldManager();

  if(track.GetTotalEnergy()<BDSGlobalConstants::Instance()->GetThresholdCutCharged())
    return DBL_MAX;
  /*
  G4double SynchOnZPos = (7.184+4.0) * CLHEP::m;
  if(track.GetPosition().z() + BDSGlobalConstants::Instance()->GetWorldSizeZ() < SynchOnZPos)
    return DBL_MAX;
  */
  if(TheFieldManager)
    {
      const G4Field* pField = TheFieldManager->GetDetectorField() ;
      G4ThreeVector  globPosition = track.GetPosition() ;
      G4double  globPosVec[3], FieldValueVec[3]={0.} ;
      globPosVec[0] = globPosition.x() ;
      globPosVec[1] = globPosition.y() ;
      globPosVec[2] = globPosition.z() ;
      
      if(pField)
	pField->GetFieldValue( globPosVec, FieldValueVec ) ; 
	
      G4double Blocal= FieldValueVec[1];
      if ( FieldValueVec[0]!=0.)
	Blocal=sqrt(Blocal*Blocal+FieldValueVec[0]*FieldValueVec[0]);

      
 
      if(track.GetMaterial()==BDSMaterials::Instance()->GetMaterial("Vacuum")&&Blocal !=0 )
	{
	  G4ThreeVector InitMag=track.GetMomentum();

	  // transform to the local coordinate frame

	  G4AffineTransform tf(track.GetTouchable()->GetHistory()->GetTopTransform().Inverse());
	  const G4RotationMatrix Rot=tf.NetRotation();
	  const G4ThreeVector Trans=-tf.NetTranslation();
	  InitMag=Rot*InitMag; 


	  G4double Rlocal=(InitMag.z()/GeV)/(0.299792458 * Blocal/tesla) *m;
	  
	  MeanFreePath=
	    fabs(Rlocal)/(track.GetTotalEnergy()*nExpConst);

	  MeanFreePath /= BDSGlobalConstants::Instance()->GetSynchMeanFreeFactor();

	  if(MeanFreePathCounter==BDSGlobalConstants::Instance()->GetSynchMeanFreeFactor())
	    MeanFreePathCounter=0;

#ifdef DEBUG
          G4cout<<"*****************SR*************************"<<G4endl;
          G4cout<<"Track momentum: "<<InitMag<<G4endl;
          G4cout<<"Blocal="<<Blocal/CLHEP::tesla<<"  Rlocal="<<RlocalCLHEP::/m<<G4endl;
          G4cout<<track.GetVolume()->GetName()<<" mfp="<<MeanFreePath/CLHEP::m<<G4endl;
          G4cout<<"********************************************"<<G4endl;
#endif

	  return MeanFreePath;
	}
      else
	return DBL_MAX;
    }
  else
    return DBL_MAX;
  
}
  


#endif
