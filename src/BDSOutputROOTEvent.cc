#include "BDSOutputROOTEvent.hh"

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSUtilities.hh"
#include "BDSSamplerBase.hh"

BDSOutputROOTEvent::BDSOutputROOTEvent() 
{  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  this->Init(); 
}

BDSOutputROOTEvent::~BDSOutputROOTEvent() 
{
  if (theRootOutputFile && theRootOutputFile->IsOpen())
    {theRootOutputFile->Write(0,TObject::kOverwrite);}
}

void BDSOutputROOTEvent::Init() 
{
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
  BDSOutputROOTEventOptions *theOptionsOutput = new BDSOutputROOTEventOptions();
  theOptionsOutputTree->Branch("Options.","BDSOutputROOTEventOptions",theOptionsOutput,32000,1);
  theOptionsOutput->Fill();
  theOptionsOutputTree->Fill();

  //
  // build model and write structure
  //
  BDSOutputROOTEventModel *theModelOutput = new BDSOutputROOTEventModel();
  theModelOutputTree->Branch("Model.","BDSOutputROOTEventModel",theModelOutput,32000,1);
  theModelOutput->Fill();
  theModelOutputTree->Fill();
   
 
  //
  // build primary structures
  //
  primary = new BDSOutputROOTEventSampler("Primary");
  theRootOutputTree->Branch("Primary.","BDSOutputROOTEventSampler",primary,32000,1); 
  samplerMap["Primary"] = primary;

  //
  // build sampler structures 
  //
  for(G4int i=0;i<BDSSamplerBase::GetNSamplers();i++) 
    { 
      G4String name=BDSSamplerBase::outputNames[i];

      // remove sampler number
      G4String stripName = name.substr(0,name.find_last_of("_"));
      stripName = stripName.substr(8,100); // remove Sampler_
      
      // create sampler structure
      samplerMap[name] = new BDSOutputROOTEventSampler(name);

      // set tree branches 
      theRootOutputTree->Branch((stripName+".").c_str(),"BDSOutputROOTEventSampler",samplerMap[name],32000,1);

      G4cout << name << G4endl;      
    }

  //
  // Build loss and hit structures
  // 
  eloss = new BDSOutputROOTEventLoss();
  ploss = new BDSOutputROOTEventLoss();


  //
  // Build process structures
  //
  
  //
  // Build trajectory structures
  // 

}
  
/// write sampler hit collection
void BDSOutputROOTEvent::WriteHits(BDSSamplerHitsCollection* hc) 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << __METHOD_NAME__ << hc->entries() << std::endl;
#endif

  for(int i=0;i<hc->entries();i++) {
    //    G4cout << (*hc)[i]->GetName() << G4endl;
    samplerMap[(*hc)[i]->GetName()]->Fill((*hc)[i]);
  }  
}

/// write energy deposition hits
void BDSOutputROOTEvent::WriteEnergyLoss(BDSEnergyCounterHitsCollection*) 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
}

/// write where primaries stop being primaries
void BDSOutputROOTEvent::WritePrimaryLoss(BDSEnergyCounterHit* // ploss
					  ) 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif


}

/// write where primaries impact
void BDSOutputROOTEvent::WritePrimaryHit(BDSEnergyCounterHit* // phits
					 ) 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
}

/// write tunnel hits
void BDSOutputROOTEvent::WriteTunnelHits(BDSTunnelHitsCollection* // tunnelHits
					 ) 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
}

/// write a trajectory 
void BDSOutputROOTEvent::WriteTrajectory(std::vector<BDSTrajectory*> &// TrajVec
					 ) 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
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
  primary->Fill(E,x0,y0,z0,xp,yp,zp,t,weight,PDGType,nEvent,TurnsTaken);
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

/// write and close and open new file
void BDSOutputROOTEvent::Commit() 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
}

/// write and close the file
void BDSOutputROOTEvent::Write() 
{
  theRootOutputFile->Write();
  theRootOutputFile->Close();
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
}

void BDSOutputROOTEvent::Flush()
{
  // loop over sampler map and clear vectors
  for(auto i= samplerMap.begin() ; i != samplerMap.end() ;++i) {
    i->second->Flush();
  }  
  
}
