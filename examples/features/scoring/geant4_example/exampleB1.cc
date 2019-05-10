#include "B1DetectorConstruction.hh"
#include "B1ActionInitialization.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "QBBC.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"
#include "G4ScoringManager.hh"

#include "B1UserScoreWriter.hh"

#include "G4ScoringBox.hh"
#include "G4PSDoseDeposit3D.hh"
#include "G4PSNofStep3D.hh"
#include "G4SDParticleFilter.hh"
#include "PSAmbientDose3D.hh"
#include "PSRadiationQuantity3D.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{

    // Choose the Random engine
    G4Random::setTheEngine(new CLHEP::RanecuEngine);

    // Construct the default run manager
    //
#ifdef G4MULTITHREADED
    G4MTRunManager* runManager = new G4MTRunManager;
#else
    G4RunManager* runManager = new G4RunManager;
#endif


    /// Prepare the scoring manager before the run initialize
    G4ScoringManager * scManager = G4ScoringManager::GetScoringManager();
    scManager->SetVerboseLevel(1);

    // Set mandatory initialization classes
    //
    // Detector construction
    runManager->SetUserInitialization(new B1DetectorConstruction());

    // Physics list
    G4VModularPhysicsList* physicsList = new QBBC;
    physicsList->SetVerboseLevel(1);
    runManager->SetUserInitialization(physicsList);

    // User action initialization
    runManager->SetUserInitialization(new B1ActionInitialization());

    runManager->Initialize();


    G4bool visualization = 1;
    if(visualization == 0)
    {

        /// Create a scorer box
        G4ScoringBox* B1Scorer_box = new G4ScoringBox("scoring_mesh");

        /// size of the scoring mesh
        G4double vsize[3];
        vsize[0] = 0.5*20.*CLHEP::cm;
        vsize[1] = 0.5*20.*CLHEP::cm;
        vsize[2] = 0.5*20.*CLHEP::cm;
        B1Scorer_box->SetSize(vsize);

        /// divisions of the scoring mesh
        G4int nSegment[3];
        nSegment[0] = 2;
        nSegment[1] = 5;
        nSegment[2] = 10;

        B1Scorer_box->SetNumberOfSegments(nSegment);

        /// Position of the scoring mesh
        G4double centerPosition[3];
        centerPosition[0] = 0.*CLHEP::cm;
        centerPosition[1] = 0.*CLHEP::cm;
        centerPosition[2] = 0.*CLHEP::cm;
        B1Scorer_box->SetCenterPosition(centerPosition);

        /// Dose scorer

        G4PSDoseDeposit3D* ps =new G4PSDoseDeposit3D("Dose");
        B1Scorer_box->SetPrimitiveScorer(ps);

        /// Ambient dose scorer
        PSAmbientDose3D* ambientdose_ps = new PSAmbientDose3D("Ambient_dose_mesh");
        B1Scorer_box->SetPrimitiveScorer(ambientdose_ps);
        //B1Scorer_box->SetFilter(new G4SDParticleFilter("pfilter","proton"));

        /// Radioprotection scorer
        PSRadiationQuantity3D* ambientdose_scorer_proton = new PSRadiationQuantity3D("ambient_dose_proton","h10_coeffs/h10protons.txt");
        B1Scorer_box->SetPrimitiveScorer(ambientdose_scorer_proton);
        B1Scorer_box->SetFilter(new G4SDParticleFilter("pfilter","proton"));

        PSRadiationQuantity3D* ambientdose_scorer_neutron = new PSRadiationQuantity3D("ambient_dose_neutron","h10_coeffs/h10neutrons.txt");
        B1Scorer_box->SetPrimitiveScorer(ambientdose_scorer_neutron);
        B1Scorer_box->SetFilter(new G4SDParticleFilter("nfilter","neutron"));

        /// register the mesh in the scoring manager
        scManager->RegisterScoringMesh(B1Scorer_box);
        scManager->SetScoreWriter(new B1UserScoreWriter()); // #OWN WRITER, TO DO WITH ROOT
        scManager->SetCurrentMesh(B1Scorer_box);

        G4int N_protons = 10000;
        runManager->BeamOn(N_protons);
    }

    else
    {

        // Activate UI-command base scorer

        //====================================================================
        // Un-comment this line for user defined score writer
        // scManager->SetScoreWriter(new B1UserScoreWriter()); // #FOR WRITING -> DO IT WITH ROOT
        //====================================================================

        G4VisManager* visManager = new G4VisExecutive;
        G4UIExecutive* ui = 0;
        G4UImanager* UImanager = G4UImanager::GetUIpointer();
        visManager->Initialize();
        ui = new G4UIExecutive(argc, argv);
        UImanager->ApplyCommand("/control/execute init_vis.mac");
        ui->SessionStart();
        delete ui;
    }

    scManager->List();

    scManager->DumpQuantityToFile("scoring_mesh", // Name of the mesh
                                  "ambient_dose_proton", // name of the primitive scorer
                                  "Ambient_dose_proton.txt", // name of the output file
                                  "sequence"
    );

    scManager->DumpQuantityToFile("scoring_mesh", // Name of the mesh
                                  "ambient_dose_neutron", // name of the primitive scorer
                                  "Ambient_dose_neutron.txt", // name of the output file
                                  "sequence"
    );

    scManager->CloseCurrentMesh();

    delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
