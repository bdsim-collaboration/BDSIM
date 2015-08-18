// this class needs headers from Geant4 10.1

#include "G4Version.hh"

#if G4VERSION_NUMBER > 1009
#include "BDSBOptrChangeCrossSection.hh"
#include "BDSDebug.hh"

#include "G4BiasingProcessInterface.hh"
#include "G4BiasingProcessSharedData.hh"
#include "G4BOptnChangeCrossSection.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4VProcess.hh"

BDSBOptrChangeCrossSection::BDSBOptrChangeCrossSection(G4String particleNameIn,
						       G4String name)
  : G4VBiasingOperator(name), fSetup(true), particleName(particleNameIn)
{
  fParticleToBias = G4ParticleTable::GetParticleTable()->FindParticle(particleName);
  
  if ( fParticleToBias == 0 ) {
    G4ExceptionDescription ed;
    ed << "Particle `" << particleName << "' not found !" << G4endl;
    G4Exception("BDSBOptrChangeCrossSection(...)","BDSIM",JustWarning,ed);
  }
}

BDSBOptrChangeCrossSection::~BDSBOptrChangeCrossSection()
{
  for(std::map< const G4BiasingProcessInterface*, G4BOptnChangeCrossSection* >::iterator it = fChangeCrossSectionOperations.begin() ;
      it != fChangeCrossSectionOperations.end() ;
      it++ ) 
    delete (*it).second;
}


void BDSBOptrChangeCrossSection::StartRun()
{
  // --------------
  // -- Setup stage:
  // ---------------
  // -- Start by collecting processes under biasing, create needed biasing
  // -- operations and associate these operations to the processes:
  if(fSetup) {
    const G4ProcessManager*           processManager = fParticleToBias->GetProcessManager();
    const G4BiasingProcessSharedData* sharedData     = G4BiasingProcessInterface::GetSharedData(processManager);
    if (sharedData) {
      // -- sharedData tested, as is can happen a user attaches an operator to a
      // -- volume but without defined BiasingProcessInterface processes.
      for (size_t i = 0 ; i < (sharedData->GetPhysicsBiasingProcessInterfaces()).size(); i++) {
	const G4BiasingProcessInterface* wrapperProcess = (sharedData->GetPhysicsBiasingProcessInterfaces())[i];
	G4String operationName = "XSchange-"+wrapperProcess->GetWrappedProcess()->GetProcessName();
	fChangeCrossSectionOperations[wrapperProcess] = new G4BOptnChangeCrossSection(operationName);
	fXSScale[wrapperProcess]      = 1.0;
	fPrimaryScale[wrapperProcess] = 0;
      }
    }
    fSetup = false;
  }
}

void BDSBOptrChangeCrossSection::SetBias(G4String processName, G4double bias, G4int iPrimary) {
    const G4ProcessManager*           processManager = fParticleToBias->GetProcessManager();
    const G4BiasingProcessSharedData* sharedData     = G4BiasingProcessInterface::GetSharedData(processManager);
    for (size_t i = 0 ; i < (sharedData->GetPhysicsBiasingProcessInterfaces()).size(); i++) {
      const G4BiasingProcessInterface* wrapperProcess = (sharedData->GetPhysicsBiasingProcessInterfaces())[i];
      if(processName == wrapperProcess->GetWrappedProcess()->GetProcessName()) { 
	fXSScale[wrapperProcess]      = bias;
	fPrimaryScale[wrapperProcess] = iPrimary;
      }
    }
}

G4VBiasingOperation* BDSBOptrChangeCrossSection::ProposeOccurenceBiasingOperation(const G4Track*                   track, 
										  const G4BiasingProcessInterface* callingProcess) {
  // -----------------------------------------------------
  // -- Check if current particle type is the one to bias:
  //  -----------------------------------------------------
  if ( track->GetDefinition() != fParticleToBias ) return 0;
    
  // ---------------------------------------------------------------------
  // -- select and setup the biasing operation for current callingProcess:
  // ---------------------------------------------------------------------
  // -- Check if the analog cross-section well defined : for example, the conversion
  // -- process for a gamma below e+e- creation threshold has an DBL_MAX interaction
  // -- length. Nothing is done in this case (ie, let analog process to deal with the case)


  G4double analogInteractionLength =  callingProcess->GetWrappedProcess()->GetCurrentInteractionLength();
  if (analogInteractionLength > DBL_MAX/10.) return 0;

  if(analogInteractionLength < 0) {
    return 0;
  }

  G4double analogXS = 0;
  if(analogInteractionLength > 0) {
    analogXS = 1./analogInteractionLength;
  }

  //  G4cout << __METHOD_NAME__ <<   callingProcess->GetWrappedProcess()->GetProcessName() << " analogInteractionLength " 
  //	 << analogInteractionLength << " analogXS " << analogXS << G4endl;

  // -- Analog cross-section is well-defined:
  // -- Choose a constant cross-section bias. But at this level, this factor can be made
  // -- direction dependent, like in the exponential transform MCNP case, or it
  // -- can be chosen differently, depending on the process, etc.
  G4double XStransformation = 1.0;

  
  // -- fetch the operation associated to this callingProcess:
  G4BOptnChangeCrossSection*   operation = fChangeCrossSectionOperations[callingProcess];
  // -- get the operation that was proposed to the process in the previous step:
  G4VBiasingOperation* previousOperation = callingProcess->GetPreviousOccurenceBiasingOperation();

  // -- check for only scaling primary
  if ( fPrimaryScale[callingProcess] == 1 && track->GetParentID() != 0 ) return 0;
  XStransformation = fXSScale[callingProcess];

#if 0
  if(callingProcess->GetWrappedProcess()->GetProcessName() == "AnnihiToMuPair" || 
     //     callingProcess->GetWrappedProcess()->GetProcessName() == "ee2hadr" || 
     //     callingProcess->GetWrappedProcess()->GetProcessName() == "annihil" || 
     callingProcess->GetWrappedProcess()->GetProcessName() == "GammaToMuPair" ) {
    //    G4cout << callingProcess->GetWrappedProcess()->GetProcessName() << G4endl;
    XStransformation = 1e5;
  }
#endif

  // STB Just return the operation before the multiple sampling check
  operation->SetBiasedCrossSection( XStransformation * analogXS );
  operation->Sample();
  return operation;
    
  // -- now setup the operation to be returned to the process: this
  // -- consists in setting the biased cross-section, and in asking
  // -- the operation to sample its exponential interaction law.
  // -- To do this, to first order, the two lines:
  //        operation->SetBiasedCrossSection( XStransformation * analogXS );
  //        operation->Sample();
  // -- are correct and sufficient.
  // -- But, to avoid having to shoot a random number each time, we sample
  // -- only on the first time the operation is proposed, or if the interaction
  // -- occured. If the interaction did not occur for the process in the previous,
  // -- we update the number of interaction length instead of resampling.

  if(previousOperation == 0) {
    //    G4cout << __METHOD_NAME__ << " previousOperation==0 " << XStransformation * analogXS << G4endl;
    operation->SetBiasedCrossSection( XStransformation * analogXS );
    operation->Sample();
  }
  else {
    //    G4cout << __METHOD_NAME__ << " previousOperation!=0 " << XStransformation * analogXS << G4endl;
    if(previousOperation != operation) {
      // -- should not happen !
      G4ExceptionDescription ed;
      ed << "Logic problem in operation handling !" << G4endl;
      G4Exception("GB01BOptrChangeCrossSection::ProposeOccurenceBiasingOperation(...)",
		  "exGB01.02",JustWarning,ed);
      return 0;
    }
    if(operation->GetInteractionOccured()) {
      operation->SetBiasedCrossSection( XStransformation * analogXS );
      operation->Sample();
    }
    else {
      // -- update the 'interaction length' and underneath 'number of interaction lengths'
      // -- for past step  (this takes into accout the previous step cross-section value)
      operation->UpdateForStep(callingProcess->GetPreviousStepSize());
      // -- update the cross-section value:
      operation->SetBiasedCrossSection(XStransformation * analogXS);
      // -- forces recomputation of the 'interaction length' taking into account above
      // -- new cross-section value [tricky : to be improved]
      operation->UpdateForStep(0.0);
    }
  }
  return operation;  
}

void BDSBOptrChangeCrossSection::OperationApplied(const G4BiasingProcessInterface*           callingProcess, 
						  G4BiasingAppliedCase,
						  G4VBiasingOperation*             occurenceOperationApplied,
						  G4double,
						  G4VBiasingOperation*,    
						  const G4VParticleChange*                                  ) {
  G4BOptnChangeCrossSection* operation = fChangeCrossSectionOperations[callingProcess];
  if (operation == occurenceOperationApplied) operation->SetInteractionOccured();
}

#endif
