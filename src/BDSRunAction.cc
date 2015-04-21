#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSOutputBase.hh" 
#include "BDSRunAction.hh"
#include "BDSDebug.hh"
#include "BDSAnalysisManager.hh"
#include "BDSBeamline.hh"
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
  starttime = time(NULL);

  // construct output histograms
  // calculate histogram dimensions
  G4double smin, smax, binwidth;
  G4int nbins;
  smin     = 0.0;
  smax     = BDSGlobalConstants::Instance()->GetSMax()/CLHEP::m;
  binwidth = BDSGlobalConstants::Instance()->GetElossHistoBinWidth(); // CHECK UNITS
  nbins    = (int) ceil((smax-smin)/binwidth); // rounding up so last bin definitely covers smax
  smax     = smin + (nbins*binwidth);          // redefine smax
  // create the histograms
  phitsindex = BDSAnalysisManager::Instance()->Create1DHistogram("PhitsHisto","Primary Hits",nbins,smin,smax); //0
  plossindex = BDSAnalysisManager::Instance()->Create1DHistogram("PlossHisto","Primary Loss",nbins,smin,smax); //1
  elossindex = BDSAnalysisManager::Instance()->Create1DHistogram("ElossHisto","Energy Loss", nbins,smin,smax); //2
  // prepare bin edges for a by-element histogram
  std::vector<double> binedges;
  binedges.push_back(0.0);
  G4double s_end = 0.0; // s position at the end of the element
  for (BDSBeamline::Instance()->first(); !BDSBeamline::Instance()->isDone(); BDSBeamline::Instance()->next())
    {
      BDSAcceleratorComponent* item = BDSBeamline::Instance()->currentItem();
      s_end += item->GetArcLength()/CLHEP::m; // use arc length as hits binned in S
      binedges.push_back(s_end);
    }
  // create per element ("pe") bin width histograms
  phitspeindex = BDSAnalysisManager::Instance()->Create1DHistogram("PhitsPEHisto","Primary Hits per Element",binedges); //3
  plosspeindex = BDSAnalysisManager::Instance()->Create1DHistogram("PlossPEHisto","Primary Loss per Element",binedges); //4
  elosspeindex = BDSAnalysisManager::Instance()->Create1DHistogram("ElossPEHisto","Energy Loss per Element" ,binedges); //5
  
  //Output feedback
  G4cout << __METHOD_NAME__ << " Run " << aRun->GetRunID() << " start. Time is " << asctime(localtime(&starttime)) << G4endl;

}

void BDSRunAction::EndOfRunAction(const G4Run* aRun)
{
  //Get the current time
  stoptime = time(NULL);

  //Output feedback
  G4cout << __METHOD_NAME__ << "Run " << aRun->GetRunID() << " end. Time is " << asctime(localtime(&stoptime)) << G4endl;
  
  // Write output
  // write histograms to output - do this before potentially closing / opening new files
  bdsOutput->WriteHistogram(BDSAnalysisManager::Instance()->GetHistogram(0));
  bdsOutput->WriteHistogram(BDSAnalysisManager::Instance()->GetHistogram(1));
  bdsOutput->WriteHistogram(BDSAnalysisManager::Instance()->GetHistogram(2));
  bdsOutput->WriteHistogram(BDSAnalysisManager::Instance()->GetHistogram(3));
  bdsOutput->WriteHistogram(BDSAnalysisManager::Instance()->GetHistogram(4));
  bdsOutput->WriteHistogram(BDSAnalysisManager::Instance()->GetHistogram(5));
  if(BDSExecOptions::Instance()->GetBatch()) {  // Non-interactive mode
    bdsOutput->Write(); // write last file
  } else {
    bdsOutput->Commit(); // write and open new file
  }
    
  // note difftime only calculates to the integer second
  G4cout << "Run Duration >> " << (int)difftime(stoptime,starttime) << " s" << G4endl;
}
