#ifndef BDSBOptrMultiParticleMuonFluxEnhancement__hh
#define BDSBOptrMultiParticleMuonFluxEnhancement__hh 1

#include "G4VBiasingOperator.hh"

class G4ParticleDefinition;
class BDSBOptrMuonFluxEnhancement;

class BDSBOptrMultiParticleMuonFluxEnhancement : public G4VBiasingOperator
{
public:
  BDSBOptrMultiParticleMuonFluxEnhancement();
  ~BDSBOptrMultiParticleMuonFluxEnhancement();

  void AddParticle(G4String particleName);

private:
  // -----------------------------
  // -- Mandatory from base class:
  // -----------------------------
  // -- This method returns a biasing operation that will bias the physics process occurence:
  virtual G4VBiasingOperation*
  ProposeOccurenceBiasingOperation(const G4Track*                            track,
                                   const G4BiasingProcessInterface* callingProcess);
  virtual G4VBiasingOperation*
  ProposeFinalStateBiasingOperation(const G4Track*, const G4BiasingProcessInterface*);
  
  // -- Methods not used:
  virtual G4VBiasingOperation*
  ProposeNonPhysicsBiasingOperation(const G4Track*, const G4BiasingProcessInterface*);

private:
  // -- ("using" is to avoid compiler complaining against (false) method shadowing.)
  using G4VBiasingOperator::OperationApplied;

  // -- Optionnal base class method implementation.
  // -- This method is called to inform the operator that a proposed operation has been applied.
  // -- In the present case, it means that a physical interaction occured (interaction at
  // -- PostStepDoIt level):
  virtual void OperationApplied( const G4BiasingProcessInterface*                callingProcess,
                                 G4BiasingAppliedCase                               biasingCase,
                                 G4VBiasingOperation*                 occurenceOperationApplied,
                                 G4double                         weightForOccurenceInteraction,
                                 G4VBiasingOperation*                finalStateOperationApplied, 
                                 const G4VParticleChange*                particleChangeProduced );

public:
  // -- Optionnal base class method. It is called at the time a tracking of a particle starts:
  void StartTracking( const G4Track* track );
  
private:
  std::map < const G4ParticleDefinition*, BDSBOptrMuonFluxEnhancement* > fBOptrForParticle; ///< biasing Particle-Operator associations list.

  std::vector < const G4ParticleDefinition* > fParticlesToBias; ///< container of particle types to bias
  BDSBOptrMuonFluxEnhancement* fCurrentOperator;///< underneath operator

};


#endif