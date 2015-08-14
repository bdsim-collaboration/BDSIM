#ifndef BDSBOPTRCHANGECROSSSECTION_H
#define BDSBOPTRCHANGECROSSSECTION_H

#include "G4VBiasingOperator.hh"
class G4BOptnChangeCrossSection;
class G4ParticleDefinition;

#include <map>

class BDSBOptrChangeCrossSection : public G4VBiasingOperator {
public:
  BDSBOptrChangeCrossSection(G4String particleToBias, G4String name = "ChangeXS");
  virtual ~BDSBOptrChangeCrossSection();
  virtual void StartRun();

private: 
  virtual G4VBiasingOperation* ProposeOccurenceBiasingOperation(const G4Track* track,
								const G4BiasingProcessInterface* callingProcess);
  virtual G4VBiasingOperation* ProposeFinalStateBiasingOperation(const G4Track*, const G4BiasingProcessInterface*) 
  {return 0;}
  virtual G4VBiasingOperation* ProposeNonPhysicsBiasingOperation(const G4Track*, const G4BiasingProcessInterface*)
  {return 0;}

  using G4VBiasingOperator::OperationApplied;

  virtual void OperationApplied( const G4BiasingProcessInterface* callingProcess,
                                 G4BiasingAppliedCase             biasingCase,
                                 G4VBiasingOperation*             occurenceOperationApplied,
                                 G4double                         weightForOccurenceInteraction,
                                 G4VBiasingOperation*             finalStateOperationApplied, 
                                 const G4VParticleChange*         particleChangeProduced );
  std::map<const G4BiasingProcessInterface*, G4BOptnChangeCrossSection*> fChangeCrossSectionOperations;
  G4bool                                                                 fSetup;
  const G4ParticleDefinition*                                            fParticleToBias; 
  
};

#endif
