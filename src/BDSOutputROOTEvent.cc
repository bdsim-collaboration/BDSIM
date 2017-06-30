#include "BDSOutputROOTEvent.hh"

#include "BDSParser.hh"
#include "parser/options.h"
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
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
  const BDSGlobalConstants* g = BDSGlobalConstants::Instance();

  useScoringMap  = g->UseScoringMap();
  writePrimaries = g->WritePrimaries();

  G4bool storeLinks  = g->StoreELossLinks();
  G4bool storeLocal  = g->StoreELossLocal();
  G4bool storeGlobal = g->StoreELossGlobal();
  
  eLoss     = new BDSOutputROOTEventLoss(storeLinks, storeLocal, storeGlobal);
  pFirstHit = new BDSOutputROOTEventLoss(true      ,       true,       false);
  pLastHit  = new BDSOutputROOTEventLoss(true      ,       true,       false);
  tHit      = new BDSOutputROOTEventLoss(false     ,      false,       true);
  traj      = new BDSOutputROOTEventTrajectory();
  evtHistos = new BDSOutputROOTEventHistograms();
  evtInfo   = new BDSOutputROOTEventInfo();
  runHistos = new BDSOutputROOTEventHistograms();
  runInfo   = new BDSOutputROOTEventRunInfo();

  // build sampler structures
  if (writePrimaries)
    {
#ifndef __ROOTDOUBLE__
      primary = new BDSOutputROOTEventSampler<float>("Primary");
#else
      primary = new BDSOutputROOTEventSampler<double>("Primary");
#endif
      samplerTrees.push_back(primary);
      samplerNames.push_back("Primary");
    }
  
  for (auto const samplerName : BDSSamplerRegistry::Instance()->GetUniqueNames())
    {// create sampler structure
#ifndef __ROOTDOUBLE__
      BDSOutputROOTEventSampler<float> *res = new BDSOutputROOTEventSampler<float>(samplerName);
#else
      BDSOutputROOTEventSampler<double> *res = new BDSOutputROOTEventSampler<double>(samplerName);
#endif
      samplerTrees.push_back(res);
      samplerNames.push_back(samplerName);
    }
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
  // construct output histograms
  // calculate histogram dimensions
  CalculateHistogramParameters();
  const G4double smin   = 0.0;
  const G4double smax   = sMaxHistograms / CLHEP::m;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "histogram parameters calculated to be: " << G4endl;
  G4cout << "s minimum: " << smin     << " m" << G4endl;
  G4cout << "s maximum: " << smax     << " m" << G4endl;
  G4cout << "# of bins: " << nbins    << G4endl;
#endif
  // create the histograms
  Create1DHistogram("PhitsHisto","Primary Hits", nbins,smin,smax);
  Create1DHistogram("PlossHisto","Primary Loss", nbins,smin,smax);
  Create1DHistogram("ElossHisto","Energy Loss",  nbins,smin,smax);
  // prepare bin edges for a by-element histogram
  std::vector<G4double> binedges;
  BDSBeamline* flatBeamline = BDSAcceleratorModel::Instance()->GetFlatBeamline();
  if (flatBeamline) // can be nullptr in case of generate primaries only
    {binedges = flatBeamline->GetEdgeSPositions();}
  else
    {binedges = {0,1};}
  // create per element ("pe") bin width histograms
  Create1DHistogram("PhitsPEHisto","Primary Hits per Element", binedges);
  Create1DHistogram("PlossPEHisto","Primary Loss per Element", binedges);
  Create1DHistogram("ElossPEHisto","Energy Loss per Element" , binedges);
  
  if (useScoringMap)
    {
      const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
      evtHistos->Create3DHistogram("ScoringMap", "Energy Deposition",
				   g->NBinsX(), g->XMin()/CLHEP::m, g->XMax()/CLHEP::m,
				   g->NBinsY(), g->YMin()/CLHEP::m, g->YMax()/CLHEP::m,
				   g->NBinsZ(), g->ZMin()/CLHEP::m, g->ZMax()/CLHEP::m);
    }
}

void BDSOutputROOTEvent::Create1DHistogram(G4String name, G4String title,
                                           G4int nbins, G4double xmin, G4double xmax)
{
  evtHistos->Create1DHistogram(name, title, nbins, xmin, xmax);
  runHistos->Create1DHistogram(name, title, nbins, xmin, xmax);
}

void BDSOutputROOTEvent::Create1DHistogram(G4String name, G4String title,
                                           std::vector<double>& edges)
{
  evtHistos->Create1DHistogram(name,title,edges);
  runHistos->Create1DHistogram(name,title,edges);
}

void BDSOutputROOTEvent::CalculateHistogramParameters()
{
  // rounding up so last bin definitely covers smax
  // (max - min) / bin width -> min = 0 here.
  const BDSGlobalConstants* gc = BDSGlobalConstants::Instance();
  const G4double binWidth = gc->ElossHistoBinWidth();
  nbins = (int) ceil(gc->SMax() / binWidth); // round up to integer # of bins
  if (nbins == 0) nbins = 1; // can happen for generate primaries only
  sMaxHistograms = nbins * binWidth;
}

void BDSOutputROOTEvent::Initialise() 
{
  outputFileNumber++;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();
  
  CreateHistograms();
  
  // Base root file name 
  G4String basefilename = globalConstants->OutputFileName();
  basefilename = basefilename+std::string("_event");

  // if more than one file add number (starting at 0)
  if (numberEventPerFile>0 && globalConstants->NGenerate()>numberEventPerFile)
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
  theEventOutputTree     = new TTree("Event","BDSIM event");
  
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
  theEventOutputTree->Branch("Info.",           "BDSOutputROOTEventInfo",evtInfo,32000,1);

  // Build primary structures
  if (writePrimaries)
    {theEventOutputTree->Branch("Primary.",        "BDSOutputROOTEventSampler",primary,32000,1);}

  // Build loss and hit structures
  theEventOutputTree->Branch("Eloss.",          "BDSOutputROOTEventLoss",eLoss,4000,1);
  theEventOutputTree->Branch("PrimaryFirstHit.","BDSOutputROOTEventLoss",pFirstHit,4000,2);
  theEventOutputTree->Branch("PrimaryLastHit.", "BDSOutputROOTEventLoss",pLastHit, 4000,2);
  theEventOutputTree->Branch("TunnelHit.",      "BDSOutputROOTEventLoss",tHit, 4000,2);

  // Build trajectory structures
  theEventOutputTree->Branch("Trajectory.",     "BDSOutputROOTEventTrajectory",traj,4000,2);

  // Build event histograms
  theEventOutputTree->Branch("Histos.",         "BDSOutputROOTEventHistograms",evtHistos,32000,1);

  // build sampler structures
  for (G4int i = 0; i < (G4int)samplerTrees.size(); ++i)
    {
      auto samplerTreeLocal = samplerTrees.at(i);
      auto samplerName      = samplerNames.at(i);
      // set tree branches
      theEventOutputTree->Branch((samplerName+".").c_str(),
                                 "BDSOutputROOTEventSampler",
                                 samplerTreeLocal,32000,1);
    }
}
  
/// write sampler hit collection
void BDSOutputROOTEvent::WriteHits(BDSSamplerHitsCollection* hc) 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << __METHOD_NAME__ << hc->entries() << std::endl;
#endif
  for (int i=0; i<hc->entries(); i++)
    {
      G4int samplerID = (*hc)[i]->GetSamplerID();
      if (writePrimaries)
        {samplerID += 1;} // offset index by one
      samplerTrees[samplerID]->Fill((*hc)[i]);
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
    if (useScoringMap)
    {
      G4double x = hit->Getx()/CLHEP::m;
      G4double y = hit->Gety()/CLHEP::m;
      evtHistos->Fill3DHistogram(0, x, y, sHit, eW);
    }
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
  if (writePrimaries)
    {primary->Fill(E,x0,y0,z0,xp,yp,zp,t,weight,PDGType,nEvent,TurnsTaken,0 /* always first element */);}
}

void BDSOutputROOTEvent::FillEvent() 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  theRootOutputFile->cd();
  theEventOutputTree->Fill();
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
  if (theRootOutputFile)
    {theRootOutputFile->cd();}
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
