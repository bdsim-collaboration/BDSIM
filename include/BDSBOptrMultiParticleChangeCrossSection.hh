#ifndef BDSBOPTRMULTIPARTICLECHANGECROSSSECTION_H
#define BDSBOPTRMULTIPARTICLECHANGECROSSSECTION_H

// this class uses classes from Geant4 10.0
#include "G4Version.hh"
#if G4VERSION_NUMBER > 999

#include "G4VBiasingOperator.hh"
class BDSBOptrChangeCrossSection;
class G4ParticleDefinition; 

#include <map>

class BDSBOptrMultiParticleChangeCrossSection : public G4VBiasingOperator {
public:
  BDSBOptrMultiParticleChangeCrossSection();
  virtual ~BDSBOptrMultiParticleChangeCrossSection();
  
  void AddParticle(G4String particleName);
  void StartTracking( const G4Track* track );

private: 
  virtual G4VBiasingOperation* ProposeOccurenceBiasingOperation(const G4Track*                   track,
								const G4BiasingProcessInterface* callingProcess);
  // -- Methods not used:
  virtual G4VBiasingOperation* ProposeFinalStateBiasingOperation(const G4Track*, const G4BiasingProcessInterface*)
  {return 0;}
  virtual G4VBiasingOperation* ProposeNonPhysicsBiasingOperation(const G4Track*, const G4BiasingProcessInterface*)
  {return 0;}
  virtual void OperationApplied(const G4BiasingProcessInterface* callingProcess,
				G4BiasingAppliedCase             biasingCase,
 				G4VBiasingOperation*             occurenceOperationApplied,
				G4double                         weightForOccurenceInteraction,
				G4VBiasingOperation*             finalStateOperationApplied, 
				const G4VParticleChange*         particleChangeProduced);  
  std::map<const G4ParticleDefinition*, BDSBOptrChangeCrossSection*> fBOptrForParticle;
  std::vector<const G4ParticleDefinition*>                           fParticlesToBias;
  BDSBOptrChangeCrossSection*                                        fCurrentOperator;

  // -- count number of biased interations for current track:
  G4int fnInteractions;  
};

#endif

#endif
