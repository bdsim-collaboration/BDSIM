#include "G4Version.hh"
#if G4VERSION_NUMBER > 999

#include "BDSBOptrMultiParticleChangeCrossSection.hh"
#include "G4BiasingProcessInterface.hh"

#include "BDSBOptrChangeCrossSection.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "BDSDebug.hh"

BDSBOptrMultiParticleChangeCrossSection::BDSBOptrMultiParticleChangeCrossSection()
  : G4VBiasingOperator("NotSureWhatToCallThis")
{
#ifdef BDSDEBUG
  debug = true;
#else
  debug = false;
#endif
}

BDSBOptrMultiParticleChangeCrossSection::~BDSBOptrMultiParticleChangeCrossSection() 
{}

void BDSBOptrMultiParticleChangeCrossSection::AddParticle(G4String particleName)
{
  const G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle(particleName);
  
  if(particle == nullptr) 
    {
      G4ExceptionDescription ed;
      ed << "Particle `" << particleName << "' not found !" << G4endl;
      G4Exception("BDSBOptrMultiParticleChangeCrossSection::AddParticle(...)",
		  "BDSIM",JustWarning,ed);
      return;
    }
  
  BDSBOptrChangeCrossSection* optr = new BDSBOptrChangeCrossSection(particleName,particleName);
  fParticlesToBias.push_back(particle);
  fBOptrForParticle[particle] = optr;
}

void BDSBOptrMultiParticleChangeCrossSection::SetBias(G4String particleName, G4String process, G4double dBias, G4int iPrimary) 
{
  if(debug) 
    G4cout << __METHOD_NAME__ << particleName << " " << process << " " << dBias << " " << iPrimary << G4endl;
  const G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle(particleName);
  if(particle == nullptr) {
    G4ExceptionDescription ed;
    ed << "Particle `" << particleName << "' not found !" << G4endl;
    G4Exception("BDSBOptrMultiParticleChangeCrossSection::SetBias(...)",
		"BDSIM",JustWarning,ed);
    return;
  }
  fBOptrForParticle[particle]->SetBias(process,dBias,iPrimary);
}


G4VBiasingOperation* BDSBOptrMultiParticleChangeCrossSection::ProposeOccurenceBiasingOperation(const G4Track* track,
											       const G4BiasingProcessInterface* callingProcess) {
  // -- examples of limitations imposed to apply the biasing:
  // -- limit application of biasing to primary particles only:
  //  if ( track->GetParentID() != 0 ) return 0;
  // -- limit to at most 5 biased interactions:
  //  if ( fnInteractions > 4 )        return 0;
  // -- and limit to a weight of at least 0.05:
  //  if ( track->GetWeight() < 0.05 ) return 0;
  
  if ( fCurrentOperator ) return fCurrentOperator->GetProposedOccurenceBiasingOperation(track, callingProcess);
  else                    return nullptr;
}


void BDSBOptrMultiParticleChangeCrossSection::StartTracking(const G4Track* track) {
  // -- fetch the underneath biasing operator, if any, for the current particle type:
  const G4ParticleDefinition* definition = track->GetParticleDefinition();
  std::map <const G4ParticleDefinition*,BDSBOptrChangeCrossSection*>::iterator it = fBOptrForParticle.find(definition);
  fCurrentOperator = 0;
  if(it != fBOptrForParticle.end()) fCurrentOperator = (*it).second;

  // -- reset count for number of biased interactions:
  fnInteractions = 0;
}

void  BDSBOptrMultiParticleChangeCrossSection::
OperationApplied(const G4BiasingProcessInterface*               callingProcess, 
		 G4BiasingAppliedCase                              biasingCase,
		 G4VBiasingOperation*                occurenceOperationApplied, 
		 G4double                        weightForOccurenceInteraction,
		 G4VBiasingOperation*               finalStateOperationApplied, 
		 const G4VParticleChange*               particleChangeProduced ) {
  // -- count number of biased interactions:
  fnInteractions++;
  
  // -- inform the underneath biasing operator that a biased interaction occured:
  if (fCurrentOperator) fCurrentOperator->ReportOperationApplied(callingProcess,
								 biasingCase,
								 occurenceOperationApplied,
								 weightForOccurenceInteraction,
								 finalStateOperationApplied,
								 particleChangeProduced);
}

#endif
