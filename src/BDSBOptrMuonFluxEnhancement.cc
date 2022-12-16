#include "BDSBOptrMuonFluxEnhancement.hh"
#include "BDSBOptnSplitAtCreation.hh"

#include "G4BiasingProcessInterface.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4VProcess.hh"

#include "Randomize.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "G4HadronicProcessStore.hh"
#include "G4InteractionLawPhysical.hh"
#include "G4ProcessManager.hh"
#include "G4UnitsTable.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4NavigationHistory.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHandle.hh"
#include "G4VSolid.hh"

#include "TrackInformation.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/// \class BDSBOptrMuonFluxEnhancement
/// \Brief
/// Implementetion of G4VBiasingOperator to change cross section of a process.\n
/// Also biases the final state of the inelastic process.
/// \EndBrief
///
/// \Detailed
/// The occurance of the Kaon+ inelastic process is modified (biased)\n
/// by updating its cross-section at each step by a factor depending on\n
/// the distance to back of GTK3 non-void volumes.\n
/// It also forwards a call to BOptnForceFinalState to bias also the\n
/// final state of this
/// for details see:
/// http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForToolkitDeveloper/html/GuideToExtendFunctionality/EventBiasing/eventBiasing.html#physics-process-occurrence-biasing
///
/// \EndDetailed

BDSBOptrMuonFluxEnhancement::BDSBOptrMuonFluxEnhancement(G4String particleName, G4String name)
  : G4VBiasingOperator(name), fSetup(true), fChangeCrossSectionOperations(),
    fFinalStateOperation() {
  /// \MemberDescr
  /// \param particleName: G4String containing the name of the particle to bias.
  /// \param name: G4String containing the name to be assigned to this operator
  ///
  /// Constructor.\n
  /// Retireves the particle definition of particle with name particleName\n
  /// and creates a final state biasing operation for this particle type\n
  /// If the definition is not found, a warning is issued and no biasing occurs.
  /// \EndMemberDescr

  fParticleToBias = G4ParticleTable::GetParticleTable()->FindParticle(particleName);
  if (fParticleToBias == 0) {
    G4ExceptionDescription ed;
    ed << "Particle `" << particleName << "' not found !" << G4endl;
    G4Exception("BOptrChangeInelastic(...)", "exGB01.01", JustWarning, ed);
  }
  fSplitAtCreationOperation = new BDSBOptnSplitAtCreation("splitAtCreation");
  G4cout << "Created split-at-creation operation for " << fParticleToBias->GetParticleName().data()
         << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BDSBOptrMuonFluxEnhancement::~BDSBOptrMuonFluxEnhancement() {
  /// \MemberDescr
  /// Destructor\n
  /// Deletes all of the biasing operations created
  /// \EndMemberDescr
  for (auto op : fFinalStateOperation) {
    delete op.second;
  }
  for (auto op : fChangeCrossSectionOperations) {
    delete op.second;
  }

  delete fSplitAtCreationOperation;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BDSBOptrMuonFluxEnhancement::StartRun() {
  /// \MemberDescr
  ///
  /// Setup stage.\n
  /// Start by collecting processes under biasing, create needed biasing
  /// operations and associate these operations to the processes
  /// \EndMemberDescr

  if (fSetup) {

    const G4ProcessManager *processManager = fParticleToBias->GetProcessManager();
    const G4BiasingProcessSharedData *sharedData =
      G4BiasingProcessInterface::GetSharedData(processManager);
    if (sharedData) // -- sharedData tested, as is can happen a user attaches an operator to a
                    // -- volume but without defined BiasingProcessInterface processes.
    {
      for (size_t i = 0; i < (sharedData->GetPhysicsBiasingProcessInterfaces()).size(); i++) {
        const G4BiasingProcessInterface *wrapperProcess =
          (sharedData->GetPhysicsBiasingProcessInterfaces())[i];

        G4String operationName;
        operationName = "XSchange-"+wrapperProcess->GetWrappedProcess()->GetProcessName();
        fChangeCrossSectionOperations[wrapperProcess] = new G4BOptnChangeCrossSection(operationName);
        G4cout << "Created cross section operation for " << wrapperProcess->GetProcessName().data() << G4endl;

        /*
        switch (BiasingUtility::GetInstance()->GetBiasMethod()) {
        case 2:
          operationName = "XSchange-" + wrapperProcess->GetWrappedProcess()->GetProcessName();
          fChangeCrossSectionOperations[wrapperProcess] =
            new G4BOptnChangeCrossSection(operationName);
          G4cout << "Created crossSection operator for " << wrapperProcess->GetProcessName().data()
                 << ", of particle " << fParticleToBias->GetParticleName().data() << G4endl;

          operationName =
            "ChangeFinalState-" + wrapperProcess->GetWrappedProcess()->GetProcessName();

          // fFinalStateOperation[wrapperProcess] = new BOptnChangeInelastic(operationName);
          // G4cout << "Created final state operation for " <<
          // wrapperProcess->GetProcessName().data()
          //        << ", of particle " << fParticleToBias->GetParticleName().data() << G4endl;
          break;

        case 3:
          operationName = "XSchange-" + wrapperProcess->GetWrappedProcess()->GetProcessName();
          fChangeCrossSectionOperations[wrapperProcess] =
            new G4BOptnChangeCrossSection(operationName);
          G4cout << "Created crossSection operator for " << wrapperProcess->GetProcessName().data()
                 << ", of particle " << fParticleToBias->GetParticleName().data() << G4endl;

        default:
          break;
        }
        */
      }
    }
    fSetup = false;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VBiasingOperation *BDSBOptrMuonFluxEnhancement::ProposeOccurenceBiasingOperation(
  const G4Track *track, const G4BiasingProcessInterface *callingProcess) {
  /// \MemberDescr
  /// \param track: pointer to G4Track object under biasing
  /// \param callingProcess: pointer to G4BiasingProcessInterface, the biased process.
  ///
  /// Retrieve the analog (non-biased) interaction cross-section for the current step,\n
  /// then compute the distance between the track's position to the exit of the volume\n
  /// and rescale the cross section as described in the user manual (link in class description).\n
  ///
  /// Perform some preliminary sanity checks first:\n
  /// 1) The interaction should not have occured\n
  /// 2) The position of the track should be between ends of\n
  /// non-void GTK3 material
  /// 3) The analog cross-section should not be 0. (happens sometimes)
  /// \EndMemberDescr

  G4int trackPDGid = track->GetParticleDefinition()->GetPDGEncoding();
  TrackInformation *trackInfo = (TrackInformation *)track->GetUserInformation();

  // No biasing in case either:
  // --track is analog (i.e. not a clone)
  // --track is allowed to do inelastic interactions
  if (trackInfo->GetTrackType() == 0 ||
      trackInfo->GetIsAllowedInelastic()){
    return nullptr;
  }


  // No biasing for processes that do not kill the particle
  G4ProcessType processType= callingProcess->GetWrappedProcess()->GetProcessType();
  if (procprocessType!= fHadronic && processType != fDecay && trackPDGid != 22){
    return nullptr;
  }

  // Photons treated separately because we don't want to bias in vacuum
  if (trackPDGid == 22){
    G4VPhysicalVolume *volume = track->GetVolume();
    if (volume){
      // Check if the volume is made of void. Better option needed.
      if (volume->GetLogicalVolume()->GetMaterial()->GetName().contains("G4_Galactic")){
        return nullptr
      }
    }
  }

  G4BOptnChangeCrossSection *operation = fChangeCrossSectionOperations[callingProcess];
  if (!operation){
    return nullptr;
  }

  G4double analogInteractionLength = 
    callingProcess->GetWrappedProcess()->GetCurrentInteractionLength() / CLHEP::mm;

  if (analogInteractionLength > DBL_MAX/10.){
    return nullptr;
  }
  if (analogInteractionLength < 0.){
    return nullptr;
  }

  G4double analogXS = 1./analogInteractionLength;
  G4double XStransformation = 1.0;

  // to update to new G4
  G4String processName = callingProcess->GetWrappedProcess()->GetProcessName();
  if (processType == fHadronic || processName.contains("conv")){
    // clones should not undergo this kind of process
    XStransformation = 0.0;
  }
  else{
    // exponential increase of the xs for processes leading to muons
    G4StepPoint *pointPre = track->GetStep()->GetPreStepPoint();

    G4ThreeVector worldPosition = pointPre->GetPosition();
    G4ThreeVector worldMomDir = pointPre->GetMomentumDirection();
    G4double dz = BiasingUtility::GetInstance()->GetDecayZMax() / CLHEP::mm -
                  worldPosition.getZ() / CLHEP::mm;
    G4double dx = worldMomDir.getX() / worldMomDir.getZ() * dz;
    G4double dy = worldMomDir.getY() / worldMomDir.getZ() * dz;

    G4double distanceLeft = sqrt(dx * dx + dy * dy + dz * dz);
    XStransformation = 1. / (1.0 - std::exp(-analogXS * distanceLeft));
  }

  operation->SetBiasedCrossSection(XStransformation*analogXS);
  operation->Sample();
  return operation;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BDSBOptrMuonFluxEnhancement::OperationApplied(const G4BiasingProcessInterface * /*callingProcess*/,
                                            G4BiasingAppliedCase,
                                            G4VBiasingOperation * /*occurenceOperationApplied*/,
                                            G4double, G4VBiasingOperation *,
                                            const G4VParticleChange *) {
  /// \MemberDescr
  /// \param callingProcess: pointer to G4BiasingProcessInterface, the biased process
  /// \param occurenceOperationApplied: pointer to G4VBiasingOperation, the change of cross-section
  //
  /// Optionnal base class method implementation.\n
  /// This method is called to inform the operator that a proposed operation has been applied.\n
  /// In the present case, it means that a physical interaction occured (interaction at
  /// PostStepDoIt level):
  /// \EndMemberDescr

  // G4BOptnChangeInelastic *operation = fChangeInelasticOperations[callingProcess];
  // if (operation == occurenceOperationApplied){
  //   operation->SetInteractionOccured();
  //   BiasingUtility::GetInstance()->SetGTKInelasticOccured();
  // }
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VBiasingOperation *BDSBOptrMuonFluxEnhancement::ProposeFinalStateBiasingOperation(
  const G4Track * /*track*/, const G4BiasingProcessInterface *callingProcess) {
  /// \MemberDescr
  /// Mandatory base class method implementation.\n
  /// This method is called if the process ocurred and asks for
  /// the operation acting on its final state of it.\n
  /// Call here the final state biasing operations, which\n
  /// in turn, calls the analog process's PostStepDoIt method.
  /// \EndMemberDescr
  // return 0;
  return nullptr;
}

G4VBiasingOperation *BDSBOptrMuonFluxEnhancement::ProposeNonPhysicsBiasingOperation(
  const G4Track *, const G4BiasingProcessInterface *) {
  BOptnSplitAtCreation *operation = fSplitAtCreationOperation;
  if (operation) {
    return operation;
  }
  return nullptr;
}
