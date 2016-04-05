#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSAnalysisManager.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSOutputBase.hh" 
#include "BDSRunAction.hh"

#include "G4Run.hh"

#include "globals.hh"               // geant4 globals / types

extern BDSOutputBase* bdsOutput;         // output interface

BDSRunAction::BDSRunAction()
{}

BDSRunAction::~BDSRunAction()
{}

void BDSRunAction::BeginOfRunAction(const G4Run* aRun)
{
  //Get the current time
  starttime = time(nullptr);

  // construct output histograms
  // calculate histogram dimensions
  G4double smin     = 0.0;
  G4double smax     = BDSGlobalConstants::Instance()->SMax() / CLHEP::m;
  G4double binwidth = BDSGlobalConstants::Instance()->ElossHistoBinWidth();
  G4int    nbins    = (int) ceil((smax-smin)/binwidth); // rounding up so last bin definitely covers smax
  smax              = smin + (nbins*binwidth);          // redefine smax
  G4String slabel   = "s [m]";
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "histogram parameters calculated to be: " << G4endl;
  G4cout << "s minimum: " << smin     << " m" << G4endl;
  G4cout << "s maximum: " << smax     << " m" << G4endl;
  G4cout << "bin width: " << binwidth << " m" << G4endl;
  G4cout << "# of bins: " << nbins    << G4endl;
#endif
  // create the histograms
  phitsindex = BDSAnalysisManager::Instance()->Create1DHistogram("PhitsHisto","Primary Hits",nbins,smin,smax,slabel); //0
  plossindex = BDSAnalysisManager::Instance()->Create1DHistogram("PlossHisto","Primary Loss",nbins,smin,smax,slabel); //1
  elossindex = BDSAnalysisManager::Instance()->Create1DHistogram("ElossHisto","Energy Loss", nbins,smin,smax,slabel,"GeV"); //2

  // prepare bin edges for a by-element histogram
  std::vector<G4double> binedges = BDSAcceleratorModel::Instance()->GetFlatBeamline()->GetSPositionEndOfEach();
  
  // create per element ("pe") bin width histograms
  phitspeindex = BDSAnalysisManager::Instance()->Create1DHistogram("PhitsPEHisto","Primary Hits per Element",binedges,slabel); //3
  plosspeindex = BDSAnalysisManager::Instance()->Create1DHistogram("PlossPEHisto","Primary Loss per Element",binedges,slabel); //4
  elosspeindex = BDSAnalysisManager::Instance()->Create1DHistogram("ElossPEHisto","Energy Loss per Element" ,binedges,slabel,"GeV"); //5
  
  // Output feedback
  G4cout << __METHOD_NAME__ << " Run " << aRun->GetRunID() << " start. Time is " << asctime(localtime(&starttime)) << G4endl;

  bdsOutput->Initialise(); // open file
}

void BDSRunAction::EndOfRunAction(const G4Run* aRun)
{
  // Get the current time
  stoptime = time(nullptr);

  // Output feedback
  G4cout << __METHOD_NAME__ << "Run " << aRun->GetRunID() << " end. Time is " << asctime(localtime(&stoptime)) << G4endl;
  
  // Write output
  // write histograms to output - do this before potentially closing / opening new files
  for (int i=0; i<BDSAnalysisManager::Instance()->NumberOfHistograms(); i++)
    {bdsOutput->WriteHistogram(BDSAnalysisManager::Instance()->GetHistogram(i));}

  bdsOutput->Write(); // write last file
  bdsOutput->Close();

  // delete analysis manager
  delete BDSAnalysisManager::Instance();
  
  // note difftime only calculates to the integer second
  G4cout << "Run Duration >> " << (int)difftime(stoptime,starttime) << " s" << G4endl;
}
