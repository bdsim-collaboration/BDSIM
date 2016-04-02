#include "BDSOutputROOTEvent.hh"

#include "BDSParser.hh"
#include "parser/options.h"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSampler.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSUtilities.hh"

BDSOutputROOTEvent::BDSOutputROOTEvent()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
}

BDSOutputROOTEvent::~BDSOutputROOTEvent() 
{
  if (theRootOutputFile && theRootOutputFile->IsOpen())
    {theRootOutputFile->Write(0,TObject::kOverwrite);}
}

void BDSOutputROOTEvent::Initialise() 
{
  outputFileNumber++;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  const BDSExecOptions*     execOptions     = BDSExecOptions::Instance();
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();

  // Base root file name 
  G4String basefilename = execOptions->GetOutputFilename();
  basefilename = basefilename+std::string("_event");

  // if more than one file add number (starting at 0)
  int evntsPerNtuple = globalConstants->GetNumberOfEventsPerNtuple();
  if (evntsPerNtuple>0 && globalConstants->GetNumberToGenerate()>evntsPerNtuple)
    {basefilename += "_" + std::to_string(outputFileNumber);}
  filename = basefilename + std::string(".root");

  // policy: overwrite if output filename specifically set, otherwise increase number
  // always check in interactive mode
  if (!execOptions->GetOutputFilenameSet() || !execOptions->GetBatch()) {
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
  // root file
  theRootOutputFile    = new TFile(filename,"RECREATE", "BDS output file");
  // options data tree
  theOptionsOutputTree = new TTree("Options","BDSIM options");
  // model data tree
  theModelOutputTree   = new TTree("Model","BDSIM model");
  // event data tree
  theRootOutputTree    = new TTree("Event","BDSIM event");


  //
  // build options and write structure
  //
  // get options
  const GMAD::Options o = BDSParser::Instance()->GetOptions();
  const GMAD::OptionsBase *ob = dynamic_cast<const GMAD::OptionsBase*>(&o);
  // get exec options
  const BDSExecOptions *eo = BDSExecOptions::Instance();
  BDSOutputROOTEventOptions *theOptionsOutput = new BDSOutputROOTEventOptions(ob,eo);
  theOptionsOutputTree->Branch("Options.","BDSOutputROOTEventOptions",theOptionsOutput,32000,2);
  theOptionsOutput->Fill();
  theOptionsOutputTree->Fill();

  //
  // build model and write structure
  //
  BDSOutputROOTEventModel *theModelOutput = new BDSOutputROOTEventModel();
  theModelOutputTree->Branch("Model.","BDSOutputROOTEventModel",theModelOutput,32000,2);
  theModelOutput->Fill();
  theModelOutputTree->Fill();
   
 
  //
  // build primary structures
  //
  primary = new BDSOutputROOTEventSampler("Primary");
  theRootOutputTree->Branch("Primary.","BDSOutputROOTEventSampler",primary,32000,1); 
  samplerMap["Primary"] = primary;
  samplerTrees.push_back(primary);

  //
  // build sampler structures 
  for(auto const samplerName : BDSSamplerRegistry::Instance()->GetNames())
    {
      // create sampler structure
      BDSOutputROOTEventSampler *res = new BDSOutputROOTEventSampler(samplerName);
      //samplerMap[samplerName] = res;
      samplerTrees.push_back(res);

      // set tree branches 
      theRootOutputTree->Branch((samplerName+".").c_str(),
				"BDSOutputROOTEventSampler",
				res,
				4000,1);
    }

  //
  // Build loss and hit structures
  // 
  eLoss     = new BDSOutputROOTEventLoss();
  pFirstHit = new BDSOutputROOTEventHit();
  pLastHit  = new BDSOutputROOTEventHit();
  tHit      = new BDSOutputROOTEventHit();
  theRootOutputTree->Branch("Eloss.","BDSOutputROOTEventLoss",eLoss,4000,1);
  theRootOutputTree->Branch("PrimaryFirstHit.","BDSOutputROOTEventHit",pFirstHit,4000,2);
  theRootOutputTree->Branch("PrimaryLastHit.", "BDSOutputROOTEventHit",pLastHit, 4000,2);
  theRootOutputTree->Branch("TunnelHit.","BDSOutputROOTEventHit",tHit, 4000,2);

  //
  // Build process/track structures
  //
  
  //
  // Build trajectory structures
  // 
  traj = new BDSOutputROOTEventTrajectory();
  theRootOutputTree->Branch("Trajectory.","BDSOutputROOTEventTrajectory",traj,4000,2);
}
  
/// write sampler hit collection
void BDSOutputROOTEvent::WriteHits(BDSSamplerHitsCollection* hc) 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << __METHOD_NAME__ << hc->entries() << std::endl;
#endif

  for(int i=0;i<hc->entries();i++) {
    G4String samplerName = (*hc)[i]->GetName();
    G4int    samplerId   = (*hc)[i]->GetSamplerID();
    //samplerMap[samplerName]->Fill((*hc)[i]);
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
  for(G4int i=0;i<n_hit;i++){
    BDSEnergyCounterHit *hit = (*hc)[i];
    eLoss->Fill(hit);
  }
}

/// write where primaries impact
void BDSOutputROOTEvent::WritePrimaryHit(BDSEnergyCounterHit* phit) // TODO WritePrimaryFirstHit
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
    pFirstHit->Fill(phit);
}

/// write where primaries stop being primaries
void BDSOutputROOTEvent::WritePrimaryLoss(BDSEnergyCounterHit* ploss) // TODO WritePrimaryLastHit)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  pLastHit->Fill(ploss);
}

/// write tunnel hits
void BDSOutputROOTEvent::WriteTunnelHits(BDSTunnelHitsCollection *hc)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  G4int n_hit = hc->entries();
  for(G4int i=0;i<n_hit;i++){
    BDSTunnelHit *hit = (*hc)[i];
    tHit->Fill(hit);
  }
}

/// write a trajectory 
void BDSOutputROOTEvent::WriteTrajectory(std::vector<BDSTrajectory*> &trajVec)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
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
  theRootOutputTree->Fill();
  this->Flush();
  
}

void BDSOutputROOTEvent::Write() 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif

  if(theRootOutputFile && theRootOutputFile->IsOpen())
    {
      theRootOutputFile->Write(nullptr,TObject::kOverwrite);
    }
}

void BDSOutputROOTEvent::Close()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  if(theRootOutputFile && theRootOutputFile->IsOpen())
    {
      theRootOutputFile->Close();
      delete theRootOutputFile;
      theRootOutputFile=nullptr;
    }
}

void BDSOutputROOTEvent::Flush()
{
  // loop over sampler map and clear vectors
  for(auto i= samplerTrees.begin() ; i != samplerTrees.end() ;++i) {
    (*i)->Flush();
  }  
  eLoss->Flush();
  pFirstHit->Flush();
  pLastHit->Flush();
  tHit->Flush();
  traj->Flush();
}
