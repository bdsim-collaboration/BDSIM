#include "BDSBOptrMultiParticleMuonFluxEnhancement.hh"
#include "G4BiasingProcessInterface.hh"

#include "BDSBOptrMuonFluxEnhancement.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

BDSBOptrMultiParticleMuonFluxEnhancement::BDSBOptrMultiParticleMuonFluxEnhancement():
G4VBiasingOperator("BDSBOptrMultiParticleMuonFluxEnhancement")
{}

BDSBOptrMultiParticleMuonFluxEnhancement::~BDSBOptrMultiParticleMuonFluxEnhancement()
{
  for (auto op : fBOptrForParticle)
  {delete op.second;}
}

void BDSBOptrMultiParticleMuonFluxEnhancement::StartTracking(const G4Track *track) {
  const G4ParticleDefinition *definition = track->GetParticleDefinition();
  std::map<const G4ParticleDefinition *, BDSBOptrMuonFluxEnhancement *>::iterator it =
    fBOptrForParticle.find(definition);
  fCurrentOperator = 0;
  if (it != fBOptrForParticle.end()) {
    fCurrentOperator = (*it).second;
  }
}

void BDSBOptrMultiParticleMuonFluxEnhancement::AddParticle(G4String particleName){
  const G4ParticleDefinition *particleDef = 
    G4ParticleTable::GetParticleTable()->FindParticle(particleName);

  if (!particleDef){
    G4ExceptionDescription ed;
    ed << "Particle `" << particleName << "' not found !" << G4endl;
    G4Exception("BDSBOptrMultiParticleMuonFluxEnhancement::AddParticle(...)", "exGB01.02", JustWarning, ed);
    return;
  }

  BDSBOptrMuonFluxEnhancement *optr = new BDSBOptrMuonFluxEnhancement(particleName);
  fParticlesToBias.push_back(particleDef);
  fBOptrForParticle[particleDef] = optr;
}

G4VBiasingOperation *BDSBOptrMultiParticleMuonFluxEnhancement::ProposeOccurenceBiasingOperation(
  const G4Track *track, const G4BiasingProcessInterface *callingProcess)
{
  if (fCurrentOperator)
  {return fCurrentOperator->GetProposedOccurenceBiasingOperation(track, callingProcess);}
  else
  {return nullptr;}
}

void BDSBOptrMultiParticleMuonFluxEnhancement::OperationApplied(
  const G4BiasingProcessInterface *callingProcess, G4BiasingAppliedCase biasingCase,
  G4VBiasingOperation *occurenceOperationApplied, G4double weightForOccurenceInteraction,
  G4VBiasingOperation *finalStateOperationApplied,
  const G4VParticleChange *particleChangeProduced) {
  if (fCurrentOperator)
    fCurrentOperator->ReportOperationApplied(callingProcess, biasingCase, occurenceOperationApplied,
                                             weightForOccurenceInteraction,
                                             finalStateOperationApplied, particleChangeProduced);
}

G4VBiasingOperation *BDSBOptrMultiParticleMuonFluxEnhancement::ProposeFinalStateBiasingOperation(
  const G4Track *track, const G4BiasingProcessInterface *callingProcess) {
  if (fCurrentOperator)
    return fCurrentOperator->GetProposedFinalStateBiasingOperation(track, callingProcess);
  return 0;
}

G4VBiasingOperation *BDSBOptrMultiParticleMuonFluxEnhancement::ProposeNonPhysicsBiasingOperation(
  const G4Track *track, const G4BiasingProcessInterface *callingProcess) {
  if (fCurrentOperator)
    return fCurrentOperator->GetProposedNonPhysicsBiasingOperation(track, callingProcess);
  return 0;
}