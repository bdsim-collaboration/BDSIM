#ifndef BDSBOPTRCHANGECROSSSECTION_H
#define BDSBOPTRCHANGECROSSSECTION_H

#include "globals.hh" // geant4 types / globals
#include "G4VBiasingOperator.hh"

class G4BOptnChangeCrossSection;
class G4ParticleDefinition;

#include <map>

/** 
 * @brief Operator that changes cross section of a process for a particle.
 *
 * Based on the GB01 example from Geant4.
 * 
 * @author Stewart Boogert
 */

class BDSBOptrChangeCrossSection: public G4VBiasingOperator
{
public:
  BDSBOptrChangeCrossSection(G4String particleToBias, G4String name = "ChangeXS");
  virtual ~BDSBOptrChangeCrossSection();
  
  virtual void StartRun();
  void SetBias(G4String processName, G4double dBias, G4int iPrimary);

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
  std::map<const G4BiasingProcessInterface*, G4double>                   fXSScale;
  std::map<const G4BiasingProcessInterface*, G4int>                      fPrimaryScale;
  G4bool                                                                 fSetup;
  G4String                                                               particleName;
  const G4ParticleDefinition*                                            fParticleToBias; 
};

#endif
