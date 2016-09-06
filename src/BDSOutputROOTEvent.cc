#include "BDSOutputROOTEvent.hh"

#include "BDSParser.hh"
#include "parser/options.h"
#include "BDSAnalysisManager.hh"
#include "BDSDebug.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSamplerHit.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSTrajectoryPoint.hh"
#include "BDSUtilities.hh"

#include "TFile.h"
#include "TObject.h"
#include "TTree.h"

#include <ctime>

BDSOutputROOTEvent::BDSOutputROOTEvent()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif

#ifndef __ROOTDOUBLE__
  primary = new BDSOutputROOTEventSampler<float>("Primary");
#else
  primary = new BDSOutputROOTEventSampler<double>("Primary");
#endif
  eLoss     = new BDSOutputROOTEventLoss(false,false);
  pFirstHit = new BDSOutputROOTEventLoss(true,false);
  pLastHit  = new BDSOutputROOTEventLoss(true,false);
  tHit      = new BDSOutputROOTEventLoss(false,true);
  traj      = new BDSOutputROOTEventTrajectory();
  evtHistos = new BDSOutputROOTEventHistograms();
  evtInfo   = new BDSOutputROOTEventInfo();

  runHistos = new BDSOutputROOTEventHistograms();
  runInfo   = new BDSOutputROOTEventRunInfo();
}

BDSOutputROOTEvent::~BDSOutputROOTEvent() 
{
  if (theRootOutputFile && theRootOutputFile->IsOpen())
    {theRootOutputFile->Write(0,TObject::kOverwrite);}

  // need to delete all objects allocated in constructor
  delete primary;
  delete eLoss;
  delete pFirstHit;
  delete pLastHit;
  delete tHit;
  delete traj;
  delete evtHistos;
  delete evtInfo;
  delete runHistos;
  delete runInfo;
}

void BDSOutputROOTEvent::CreateHistograms()
{
  // copy histograms from analysis manager
  BDSAnalysisManager* analysisManager = BDSAnalysisManager::Instance();
  for (int i=0; i<analysisManager->NumberOfHistograms(); i++)
    {
      BDSHistogram1D* hist = analysisManager->GetHistogram(i);
      G4String name = hist->GetName();
      G4String title = hist->GetTitle();
      G4bool equiDistant = hist->GetEquidistantBins();
      if (equiDistant)
	{
	  G4int nbins = hist->GetNBins();
	  G4double xmin = hist->GetXMin();
	  G4double xmax = hist->GetXMax();
	  evtHistos->Create1DHistogram(name,title,nbins,xmin,xmax);
	  runHistos->Create1DHistogram(name,title,nbins,xmin,xmax);
	}
      else
	{
	  // create bin edges
	  std::vector<double> binedges = hist->GetBinLowerEdges();
	  binedges.push_back(hist->GetXMax());
	  evtHistos->Create1DHistogram(name,title,binedges);
	  runHistos->Create1DHistogram(name,title,binedges);
	}
    }
}

void BDSOutputROOTEvent::Initialise() 
{
  outputFileNumber++;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif

  CreateHistograms();
  
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();

  // Base root file name 
  G4String basefilename = globalConstants->OutputFileName();
  basefilename = basefilename+std::string("_event");

  // if more than one file add number (starting at 0)
  int evntsPerNtuple = globalConstants->NumberOfEventsPerNtuple();
  if (evntsPerNtuple>0 && globalConstants->NGenerate()>evntsPerNtuple)
    {basefilename += "_" + std::to_string(outputFileNumber);}
  filename = basefilename + std::string(".root");

  // policy: overwrite if output filename specifically set, otherwise increase number
  // always check in interactive mode
  if (!globalConstants->OutputFileNameSet() || !globalConstants->Batch()) {
    // check if file exists
    int nTimeAppended = 1;
    while (BDS::FileExists(filename)) {
      // if exists remove trailing .root
      filename = basefilename + std::string("-") + std::to_string(nTimeAppended);
      filename += ".root";
      nTimeAppended +=1;
    }
  }

  G4cout << __METHOD_NAME__ << "Setting up new file: "<<filename<<G4endl;
  // root file - note this sets the current 'directory' to this file!
  
  theRootOutputFile      = new TFile(filename,"RECREATE", "BDS output file");
  theRootOutputFile->cd();
  // options data tree
  theOptionsOutputTree   = new TTree("Options","BDSIM options");
  // model data tree
  theModelOutputTree     = new TTree("Model","BDSIM model");
  // run histogram tree
  theRunOutputTree       = new TTree("Run","BDSIM run histograms/information");
  // event data tree
  theRootOutputTree      = new TTree("Event","BDSIM event");
  
  // Build options and write structure
  // Get options
  const GMAD::Options o = BDSParser::Instance()->GetOptions();
  const GMAD::OptionsBase *ob = dynamic_cast<const GMAD::OptionsBase*>(&o);
  // Get exec options
  BDSOutputROOTEventOptions *theOptionsOutput = new BDSOutputROOTEventOptions(ob);
  theOptionsOutputTree->Branch("Options.",     "BDSOutputROOTEventOptions",theOptionsOutput,32000,2);
  theOptionsOutput->Fill();
  theOptionsOutputTree->Fill();
  
  // Build model and write structure
  BDSOutputROOTEventModel *theModelOutput = new BDSOutputROOTEventModel();
  theModelOutputTree->Branch("Model.",         "BDSOutputROOTEventModel",theModelOutput,32000);
  theModelOutput->Fill();
  theModelOutputTree->Fill();

  // Build run data tree
  theRunOutputTree->Branch("Histos.",          "BDSOutputROOTEventHistograms",runHistos,32000,1);
  theRunOutputTree->Branch("Info.",            "BDSOutputROOTEventRunInfo",runInfo,32000,1);

  // Event info output
  theRootOutputTree->Branch("Info.",           "BDSOutputROOTEventInfo",evtInfo,32000,1);

  // Build primary structures
  theRootOutputTree->Branch("Primary.",        "BDSOutputROOTEventSampler",primary,32000,1);
  //  samplerMap["Primary"] = primary;
  samplerTrees.push_back(primary);

  // Build loss and hit structures
  theRootOutputTree->Branch("Eloss.",          "BDSOutputROOTEventLoss",eLoss,4000,1);
  theRootOutputTree->Branch("PrimaryFirstHit.","BDSOutputROOTEventLoss",pFirstHit,4000,2);
  theRootOutputTree->Branch("PrimaryLastHit.", "BDSOutputROOTEventLoss",pLastHit, 4000,2);
  theRootOutputTree->Branch("TunnelHit.",      "BDSOutputROOTEventLoss",tHit, 4000,2);

  // Build trajectory structures
  theRootOutputTree->Branch("Trajectory.",     "BDSOutputROOTEventTrajectory",traj,4000,2);


  // Build event histograms
  theRootOutputTree->Branch("Histos.",         "BDSOutputROOTEventHistograms",evtHistos,32000,1);


  // build sampler structures 
  for(auto const samplerName : BDSSamplerRegistry::Instance()->GetUniqueNames())
    {
      // create sampler structure
#ifndef __ROOTDOUBLE__
      BDSOutputROOTEventSampler<float> *res = new BDSOutputROOTEventSampler<float>(samplerName);
#else 
      BDSOutputROOTEventSampler<double> *res = new BDSOutputROOTEventSampler<double>(samplerName);
#endif
      //samplerMap[samplerName] = res;
      samplerTrees.push_back(res);
      // set tree branches
      theRootOutputTree->Branch((G4String("Sampler_")+samplerName+".").c_str(),
				"BDSOutputROOTEventSampler",
				res,32000,1);
    }
}
  
/// write sampler hit collection
void BDSOutputROOTEvent::WriteHits(BDSSamplerHitsCollection* hc) 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << __METHOD_NAME__ << hc->entries() << std::endl;
#endif
  for(int i=0;i<hc->entries();i++)
    {
      G4int samplerId = (*hc)[i]->GetSamplerID();
      samplerTrees[samplerId+1]->Fill((*hc)[i]);
    }
}

/// write energy deposition hits
void BDSOutputROOTEvent::WriteEnergyLoss(BDSEnergyCounterHitsCollection* hc)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  G4int n_hit = hc->entries();
  for(G4int i=0;i<n_hit;i++)
  {
    BDSEnergyCounterHit* hit = (*hc)[i];
    eLoss->Fill(hit);

    G4double sHit = hit->GetSHit()/CLHEP::m;
    G4double eW   = hit->GetEnergyWeighted()/CLHEP::GeV;
    runHistos->Fill1DHistogram(2, sHit, eW);
    evtHistos->Fill1DHistogram(2, sHit, eW);
    runHistos->Fill1DHistogram(5, sHit, eW);
    evtHistos->Fill1DHistogram(5, sHit, eW);

  }
}

/// write where primaries impact
void BDSOutputROOTEvent::WritePrimaryHit(BDSTrajectoryPoint* phit)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  pFirstHit->Fill(phit);
  const G4double preStepSPosition = phit->GetPreS() / CLHEP::m;
  runHistos->Fill1DHistogram(0, preStepSPosition);
  evtHistos->Fill1DHistogram(0, preStepSPosition);
  runHistos->Fill1DHistogram(3, preStepSPosition);
  evtHistos->Fill1DHistogram(3, preStepSPosition);
}

/// write where primaries stop being primaries
void BDSOutputROOTEvent::WritePrimaryLoss(BDSTrajectoryPoint* ploss)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  pLastHit->Fill(ploss);
  const G4double postStepSPosition = ploss->GetPostS() / CLHEP::m;
  runHistos->Fill1DHistogram(1, postStepSPosition);
  evtHistos->Fill1DHistogram(1, postStepSPosition);
  runHistos->Fill1DHistogram(4, postStepSPosition);
  evtHistos->Fill1DHistogram(4, postStepSPosition);
}

/// write tunnel hits
void BDSOutputROOTEvent::WriteTunnelHits(BDSEnergyCounterHitsCollection* hc)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  G4int n_hit = hc->entries();
  for(G4int i=0;i<n_hit;i++)
    {
      BDSEnergyCounterHit *hit = (*hc)[i];
      tHit->Fill(hit);
    }
}

/// write a trajectory 
void BDSOutputROOTEvent::WriteTrajectory(std::vector<BDSTrajectory*> &trajVec)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " ntrajectory=" << trajVec.size() << G4endl;
#endif
  traj->Fill(trajVec);
}

/// write primary hit
void BDSOutputROOTEvent::WritePrimary(G4double E,
				      G4double x0,
				      G4double y0,
				      G4double z0,
				      G4double xp,
				      G4double yp,
				      G4double zp,
				      G4double t,
				      G4double weight,
				      G4int    PDGType,
				      G4int    nEvent,
				      G4int    TurnsTaken) 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  primary->Fill(E,x0,y0,z0,xp,yp,zp,t,weight,PDGType,nEvent,TurnsTaken,0 /* always first element */);
}

/// write a histgoram
void BDSOutputROOTEvent::WriteHistogram(BDSHistogram1D* /*histogramIn*/) 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
}

void BDSOutputROOTEvent::FillEvent() 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  theRootOutputFile->cd();
  theRootOutputTree->Fill();
  Flush();
}
void BDSOutputROOTEvent::WriteEventInfo(const time_t&  startTime,
					const time_t&  stopTime,
					const G4float& duration,
                                        const std::string& seedStateAtStart)
{
  evtInfo->startTime        = startTime;
  evtInfo->stopTime         = stopTime;
  evtInfo->duration         = duration;
  evtInfo->seedStateAtStart = seedStateAtStart;
}

void BDSOutputROOTEvent::WriteEventInfo(const BDSOutputROOTEventInfo* info)
{
  *evtInfo = *info;
}

void BDSOutputROOTEvent::Write(const time_t&  startTime,
			       const time_t&  stopTime,
			       const G4float& duration,
                               const std::string& seedStateAtStart)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  theRootOutputFile->cd();
  runInfo->startTime        = startTime;
  runInfo->stopTime         = stopTime;
  runInfo->duration         = duration;
  runInfo->seedStateAtStart = seedStateAtStart;
  theRunOutputTree->Fill();

  if(theRootOutputFile && theRootOutputFile->IsOpen())
    {theRootOutputFile->Write(nullptr,TObject::kOverwrite);}
}

void BDSOutputROOTEvent::Close()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  if(theRootOutputFile && theRootOutputFile->IsOpen())
    {
      theRootOutputFile->cd();
      theRootOutputFile->Write(0,TObject::kOverwrite);
      theRootOutputFile->Close();
      delete theRootOutputFile;
      theRootOutputFile = nullptr;
    }
}

void BDSOutputROOTEvent::Flush()
{
  theRootOutputFile->cd();
  // loop over sampler map and clear vectors
  for(auto i= samplerTrees.begin() ; i != samplerTrees.end() ;++i)
    {(*i)->Flush();}  
  eLoss->Flush();
  pFirstHit->Flush();
  pLastHit->Flush();
  tHit->Flush();
  traj->Flush();
  evtHistos->Flush();
}
