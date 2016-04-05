#include "BDSSteppingAction.hh"
#include "BDSGlobalConstants.hh"

#include "G4AffineTransform.hh"
#include "G4NavigationHistory.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

BDSSteppingAction::BDSSteppingAction():_step(nullptr)
{;}

BDSSteppingAction::~BDSSteppingAction()
{;}

void BDSSteppingAction::UserSteppingAction(const G4Step* ThisStep)
{
  _step = ThisStep;
  if(BDSGlobalConstants::Instance()->VerboseStep())
    {VerboseSteppingAction();}
}

void BDSSteppingAction::ThresholdCutSteppingAction()
{;}
  
void BDSSteppingAction::VerboseSteppingAction()
{ 
  //output in case of verbose step
  int ID=_step->GetTrack()->GetTrackID();
  int G4precision = G4cout.precision();
  G4cout.precision(10);
  G4cout<<"This volume="<< _step->GetTrack()->GetVolume()->GetName()<<G4endl;
	
  G4LogicalVolume* LogVol=_step->GetTrack()->GetVolume()->GetLogicalVolume();
  G4cout<<"This log volume="<<LogVol->GetName() <<G4endl;
	
  G4cout<<"ID="<<ID<<" part="<<
    _step->GetTrack()->GetDefinition()->GetParticleName()<<
    "Energy="<<_step->GetTrack()->GetTotalEnergy()/CLHEP::GeV<<
    " mom Px="
	<<_step->GetTrack()->GetMomentum()[0]/CLHEP::GeV<<
    " Py="<<_step->GetTrack()->GetMomentum()[1]/CLHEP::GeV<<
    " Pz="<<_step->GetTrack()->GetMomentum()[2]/CLHEP::GeV<<" vol="<<
    _step->GetTrack()->GetVolume()->GetName()<<G4endl;
	
  G4cout<<" Global Position="<<_step->GetTrack()->GetPosition()<<G4endl;
  G4AffineTransform tf(_step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform());
  G4cout<<" Local Position="<< tf.TransformPoint(_step->GetTrack()->GetPosition()) <<G4endl;

  if(_step->GetTrack()->GetMaterial()->GetName() !="LCVacuum")
    G4cout<<"material="<<_step->GetTrack()->GetMaterial()->GetName()<<G4endl;

  G4VProcess* proc=(G4VProcess*)(_step->GetPostStepPoint()->
				 GetProcessDefinedStep() );

  if(proc)G4cout<<" post-step process="<<proc->GetProcessName()<<G4endl<<G4endl;


  proc=(G4VProcess*)(_step->GetPreStepPoint()->
		     GetProcessDefinedStep() );

  if(proc)G4cout<<" pre-step process="<<proc->GetProcessName()<<G4endl<<G4endl;

  // set precision back
  G4cout.precision(G4precision);
}
