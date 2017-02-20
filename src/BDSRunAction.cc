#include "BDSAcceleratorModel.hh"
#include "BDSAnalysisManager.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSOutputBase.hh" 
#include "BDSRunAction.hh"

#include "globals.hh"               // geant4 globals / types
#include "G4Run.hh"

#include "CLHEP/Random/Random.h"

#include <sstream>
#include <string>

extern BDSOutputBase* bdsOutput;         // output interface

BDSRunAction::BDSRunAction()
{;}

BDSRunAction::~BDSRunAction()
{;}

void BDSRunAction::BeginOfRunAction(const G4Run* aRun)
{
  // save the random engine state
  std::stringstream ss;
  CLHEP::HepRandom::saveFullState(ss);
  seedStateAtStart = ss.str();
  
  // get the current time
  starttime = time(nullptr);

  // construct output histograms
  // calculate histogram dimensions
  const G4double smin   = 0.0;
  const G4double smax   = BDSGlobalConstants::Instance()->SMaxHistograms() / CLHEP::m;
  const G4int    nbins  = BDSGlobalConstants::Instance()->NBins();
  const G4String slabel = "s [m]";
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "histogram parameters calculated to be: " << G4endl;
  G4cout << "s minimum: " << smin     << " m" << G4endl;
  G4cout << "s maximum: " << smax     << " m" << G4endl;
  G4cout << "# of bins: " << nbins    << G4endl;
#endif
  // create the histograms
  BDSAnalysisManager* analMan = BDSAnalysisManager::Instance();
  analMan->Create1DHistogram("PhitsHisto","Primary Hits",
			     nbins,smin,smax,slabel,
			     "Number of Primaries"); //0
  analMan->Create1DHistogram("PlossHisto","Primary Loss",
			     nbins,smin,smax,slabel,
			     "Number of Primaries"); //1
  analMan->Create1DHistogram("ElossHisto","Energy Loss",
			     nbins,smin,smax,slabel,"GeV"); //2
  // prepare bin edges for a by-element histogram
  std::vector<G4double> binedges = BDSAcceleratorModel::Instance()->GetFlatBeamline()->GetEdgeSPositions();
  
  // create per element ("pe") bin width histograms
  analMan->Create1DHistogram("PhitsPEHisto","Primary Hits per Element",
			     binedges,slabel, "Number of Primaries / Element"); //3
  analMan->Create1DHistogram("PlossPEHisto","Primary Loss per Element",
			     binedges,slabel, "Number of Primaries / Element"); //4
  analMan->Create1DHistogram("ElossPEHisto","Energy Loss per Element" ,
			     binedges,slabel,"GeV"); //5
  
  // Output feedback
  G4cout << __METHOD_NAME__ << " Run " << aRun->GetRunID() << " start. Time is " << asctime(localtime(&starttime)) << G4endl;

  bdsOutput->Initialise(); // open file, create structures and histograms
}

void BDSRunAction::EndOfRunAction(const G4Run* aRun)
{
  // Get the current time
  stoptime = time(nullptr);
  // run duration
  G4float duration = difftime(stoptime,starttime);

  // Output feedback
  G4cout << G4endl << __METHOD_NAME__ << "Run " << aRun->GetRunID()
	 << " end. Time is " << asctime(localtime(&stoptime));
  
  // Write output
  // write histograms to output - do this before potentially closing / opening new files
  for (int i=0; i<BDSAnalysisManager::Instance()->NumberOfHistograms(); i++)
    {bdsOutput->WriteHistogram(BDSAnalysisManager::Instance()->GetHistogram(i));}

  bdsOutput->Write(starttime, stoptime, duration, seedStateAtStart); // write last file
  bdsOutput->Close();

  // delete analysis manager
  delete BDSAnalysisManager::Instance();
  
  // note difftime only calculates to the integer second
  G4cout << "Run Duration >> " << (int)duration << " s" << G4endl;
}
