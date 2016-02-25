#include "BDSOutputROOTBase.hh"

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSHistogram.hh"
#include "BDSSampler.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSTrajectory.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant types / globals

#include <string>
#include <vector>

BDSOutputROOTBase::BDSOutputROOTBase()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "output format ROOT"<<G4endl;
#endif
  theRootOutputFile = nullptr;
  PrecisionRegionEnergyLossTree = nullptr;
  EnergyLossTree                = nullptr;
  PrimaryHitsTree               = nullptr;
  PrimaryLossTree               = nullptr;
  TunnelLossTree                = nullptr;
  tunnelHitsHisto               = nullptr;
}

BDSOutputROOTBase::~BDSOutputROOTBase()
{
  if (theRootOutputFile && theRootOutputFile->IsOpen())
    {theRootOutputFile->Write(0,TObject::kOverwrite);}
}

TTree* BDSOutputROOTBase::BuildSamplerTreeBasic(G4String name)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  TTree* SamplerTree = new TTree(name, "Sampler output");
  
  SamplerTree->Branch("E",          &E,          "E/F");  // (GeV)
  SamplerTree->Branch("x",          &x,          "x/F");  // (m)
  SamplerTree->Branch("y",          &y,          "y/F");  // (m)
  SamplerTree->Branch("z",          &z,          "z/F");  // (m)
  SamplerTree->Branch("xp",         &xp,         "xp/F"); // (rad)
  SamplerTree->Branch("yp",         &yp,         "yp/F"); // (rad)
  SamplerTree->Branch("zp",         &zp,         "zp/F"); // (rad)
  SamplerTree->Branch("t",          &t,          "t/F");  // (ns)
  SamplerTree->Branch("S",          &S,          "S/F");  // (m)
  SamplerTree->Branch("weight",     &weight,     "weight/F");
  SamplerTree->Branch("partID",     &part,       "partID/I");
  SamplerTree->Branch("nEvent",     &eventno,    "nEvent/I");
  SamplerTree->Branch("parentID",   &pID,        "parentID/I");
  SamplerTree->Branch("trackID",    &track_id,   "trackID/I");
  SamplerTree->Branch("turnnumber", &turnnumber, "turnnumber/I");
  SamplerTree->Branch("process",    &process);

  return SamplerTree;
}

void BDSOutputROOTBase::Init()
{
  const BDSExecOptions*     execOptions     = BDSExecOptions::Instance();
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();
  // set up the root file
  G4String basefilename = execOptions->GetOutputFilename();
  // if more than one file add number (starting at 0)
  int evntsPerNtuple = globalConstants->GetNumberOfEventsPerNtuple();
  if (evntsPerNtuple>0 && globalConstants->GetNumberToGenerate()>evntsPerNtuple)
    {basefilename += "_" + std::to_string(outputFileNumber);}
  filename = basefilename + ".root";
  // policy: overwrite if output filename specifically set, otherwise increase number
  // always check in interactive mode
  if (!execOptions->GetOutputFilenameSet() || !execOptions->GetBatch())
    {
      // check if file exists
      int nTimeAppended = 1;
      while (BDS::FileExists(filename))
	{
	  // if exists remove trailing .root
	  filename = basefilename + "-" + std::to_string(nTimeAppended);
	  filename += ".root";
	  nTimeAppended +=1;
	}
    }
  
  G4cout << __METHOD_NAME__ << "Setting up new file: "<<filename<<G4endl;
  theRootOutputFile = new TFile(filename,"RECREATE", "BDS output file");

  // Build sampler trees and store in samplerTrees
  // clear (for the case of multiple output files)
  samplerTrees.clear();
  samplerTrees.reserve(BDSSamplerRegistry::Instance()->size());
  
  G4String primariesSamplerName="Primaries";
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "building sampler tree named: " << primariesSamplerName << G4endl;
#endif
  TTree* sampler = BuildSamplerTree(primariesSamplerName);
  // primaries is the first
  samplerTrees.push_back(sampler);

  for (auto const samplerName : BDSSamplerRegistry::Instance()->GetNames())
    {
      G4String name = samplerName;
      // Check if a tree by this name already exists (name has to be unique)
      TTree* tree = (TTree*)gDirectory->Get(name);
      // If it exists, add number and increase, start counting at 2
      if(tree)
	{
	  int count = 2;
	  G4String uniqueName;
	  while (tree)
	    {
	      uniqueName = name + "_" + std::to_string(count);
	      tree = (TTree*)gDirectory->Get(uniqueName);
	      count++;
	    }
	  name = uniqueName;
	}

#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "named: " << name << G4endl;
#endif
      sampler = BuildSamplerTree(name);
      samplerTrees.push_back(sampler);
    }

  if(globalConstants->GetStoreTrajectory() ||
     globalConstants->GetStoreMuonTrajectories() ||
     globalConstants->GetStoreNeutronTrajectories()) 
    // create a tree with trajectories
    {
      TTree* TrajTree = new TTree("Trajectories", "Trajectories");
      TrajTree->Branch("x",    &x,    "x/F");  // (m)
      TrajTree->Branch("y",    &y,    "y/F");  // (m)
      TrajTree->Branch("z",    &z,    "z/F");  // (m)
      TrajTree->Branch("part", &part, "part/I");
    }

  // Energy loss tree setup
  EnergyLossTree= new TTree("ElossTree", "Energy Loss");
  EnergyLossTree->Branch("S",      &S,      "S/F"); // (m)
  EnergyLossTree->Branch("E",      &E,      "E/F"); // (GeV)
  EnergyLossTree->Branch("weight", &weight, "weight/F"); // weight

  // Primary loss tree setup
  PrimaryLossTree  = new TTree("PlossTree", "Primary Losses");
  PrimaryLossTree->Branch("X",          &X,          "X/F"); // (m)
  PrimaryLossTree->Branch("Y",          &Y,          "Y/F"); // (m)
  PrimaryLossTree->Branch("Z",          &Z,          "Z/F"); // (m)
  PrimaryLossTree->Branch("S",          &S,          "S/F"); // (m)
  PrimaryLossTree->Branch("x",          &x,          "x/F"); // (m)
  PrimaryLossTree->Branch("y",          &y,          "y/F"); // (m)
  PrimaryLossTree->Branch("z",          &z,          "z/F"); // (m)
  PrimaryLossTree->Branch("E",          &E,          "s/F"); // (GeV)
  PrimaryLossTree->Branch("weight",     &weight,     "weight/F");
  PrimaryLossTree->Branch("partID",     &part,       "partID/I");
  PrimaryLossTree->Branch("turnnumber", &turnnumber, "turnnumber/I");
  PrimaryLossTree->Branch("eventNo",    &eventno,    "eventNo/I");

  // Primary hits tree setup
  PrimaryHitsTree  = new TTree("PhitsTree", "Primary Hits");
  PrimaryHitsTree->Branch("X",          &X,          "X/F"); // (m)
  PrimaryHitsTree->Branch("Y",          &Y,          "Y/F"); // (m)
  PrimaryHitsTree->Branch("Z",          &Z,          "Z/F"); // (m)
  PrimaryHitsTree->Branch("S",          &S,          "S/F"); // (m)
  PrimaryHitsTree->Branch("x",          &x,          "x/F"); // (m)
  PrimaryHitsTree->Branch("y",          &y,          "y/F"); // (m)
  PrimaryHitsTree->Branch("z",          &z,          "z/F"); // (m)
  PrimaryHitsTree->Branch("E",          &E,          "E/F"); // (GeV)
  PrimaryHitsTree->Branch("weight",     &weight,     "weight/F");
  PrimaryHitsTree->Branch("partID",     &part,       "partID/I");
  PrimaryHitsTree->Branch("turnnumber", &turnnumber, "turnnumber/I");
  PrimaryHitsTree->Branch("eventNo",    &eventno,    "eventNo/I");

  // Tunnel hits tree setup
  TunnelLossTree = new TTree("TunnelHitsTree", "Tunnel Hits");
  TunnelLossTree->Branch("E",           &E_tun,      "E/F");     // (GeV)
  TunnelLossTree->Branch("S",           &S_tun,      "S/F");     // (m)
  TunnelLossTree->Branch("r",           &r_tun,      "r/F");     // (m)
  TunnelLossTree->Branch("theta",       &angle_tun,  "theta/F"); // (rad)

  // Tunnel hits histogram
  G4double smin     = 0.0;
  G4double smax     = BDSGlobalConstants::Instance()->GetSMax() / CLHEP::m;
  G4double binwidth = BDSGlobalConstants::Instance()->GetElossHistoBinWidth();
  G4int    nbins    = (int) ceil((smax-smin)/binwidth); // rounding up so last bin definitely covers smax
  smax              = smin + (nbins*binwidth);          // redefine smax
  // x then y -> x is angle, y is s position
  tunnelHitsHisto = new TH2F("tunnel_hits_histo","Tunnel Enegy Deposition",50,-CLHEP::pi-0.01,CLHEP::pi+0.01,nbins,0,smax);
  
  // Precision region energy loss tree setup
  PrecisionRegionEnergyLossTree= new TTree("PrecisionRegionElossTree", "Energy Loss");
  PrecisionRegionEnergyLossTree->Branch("X",          &X,          "X/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("Y",          &Y,          "Y/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("Z",          &Z,          "Z/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("S",          &S,          "S/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("x",          &x,          "x/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("y",          &y,          "y/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("z",          &z,          "z/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("E",          &E,          "E/F"); // (GeV)
  PrecisionRegionEnergyLossTree->Branch("stepLength", &stepLength, "stepLength/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("weight",     &weight,     "weight/F");
  PrecisionRegionEnergyLossTree->Branch("partID",     &part,       "partID/I");
  PrecisionRegionEnergyLossTree->Branch("volumeName", &volumeName, "volumeName/C");
  PrecisionRegionEnergyLossTree->Branch("turnnumber", &turnnumber, "turnnumber/I");
  PrecisionRegionEnergyLossTree->Branch("eventNo",    &eventno,    "eventNo/I");
}

void BDSOutputROOTBase::WriteRootHit(TTree*   Tree,
				     G4double totalEnergy,
				     G4double xIn,
				     G4double yIn,
				     G4double zIn,
				     G4double xPrime,
				     G4double yPrime,
				     G4double zPrime,
				     G4double tIn,
				     G4double sIn,
				     G4double weightIn,
				     G4int    PDGtype,
				     G4int    eventNoIn,
				     G4int    parentID,
				     G4int    trackIDIn,
				     G4int    turnsTakenIn,
				     G4String processIn,
				     G4bool   fillTree)
{
  E           = totalEnergy / CLHEP::GeV;
  x           = xIn         / CLHEP::m;
  y           = yIn         / CLHEP::m;
  z           = zIn         / CLHEP::m;
  xp          = xPrime      / CLHEP::radian;
  yp          = yPrime      / CLHEP::radian;
  zp          = zPrime      / CLHEP::radian;
  T           = tIn         / CLHEP::ns;
  S           = sIn         / CLHEP::m;
  weight      = weightIn;
  part        = PDGtype; 
  eventno     = eventNoIn; 
  pID         = parentID; 
  track_id    = trackIDIn;
  turnnumber  = turnsTakenIn;
  process     = processIn;

  if (fillTree)
    {Tree->Fill();}
}

void BDSOutputROOTBase::WriteRootHit(TTree*         tree,
				     BDSSamplerHit* hit,
				     G4bool         fillTree)
{
  E           = hit->GetTotalEnergy() / CLHEP::GeV;
  x           = hit->GetX()           / CLHEP::m;
  y           = hit->GetY()           / CLHEP::m;
  z           = hit->GetZ()           / CLHEP::m;
  xp          = hit->GetXPrime()      / CLHEP::radian;
  yp          = hit->GetYPrime()      / CLHEP::radian;
  zp          = hit->GetZPrime()      / CLHEP::radian;
  T           = hit->GetT()           / CLHEP::ns;
  S           = hit->GetS()           / CLHEP::m;
  weight      = hit->GetWeight();
  part        = hit->GetPDGtype();
  eventno     = hit->GetEventNo();
  pID         = hit->GetParentID();
  track_id    = hit->GetTrackID();
  turnnumber  = hit->GetTurnsTaken();
  process     = hit->GetProcess();

  if (fillTree)
    {tree->Fill();}
}

void BDSOutputROOTBase::WritePrimary(G4double totalEnergy,
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
				     G4int    turnsTaken)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  WriteRootHit(samplerTrees[0], // Primaries is the first Sampler
	       totalEnergy, 
	       x0, y0, z0, 
	       xp, yp, zp, 
	       t,/*s=*/0.0,
	       weight, 
	       PDGType, 
	       nEvent, 
	       0, 
	       1, 
	       turnsTaken,
	       "");
}

void BDSOutputROOTBase::WriteHits(BDSSamplerHitsCollection *hc)
{
  G4String name;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Number of hits " << hc->entries() << G4endl;
#endif
  for (G4int i=0; i<hc->entries(); i++)
    {
      G4int  samplerID = (*hc)[i]->GetSamplerID();
      TTree* tree      = samplerTrees[samplerID+1]; // +1 to account for extra primaries sampler in output
      WriteRootHit(tree, (*hc)[i]);
    }
}

void BDSOutputROOTBase::WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4String name = "Trajectories";
  
  TTree* TrajTree=(TTree*)gDirectory->Get(name);
  
  if(TrajTree == nullptr) { G4cerr<<"TrajTree=nullptr"<<G4endl; return;}
  
  for(auto iT = TrajVec.begin(); iT<TrajVec.end(); iT++)
    {
      G4Trajectory* Traj=(G4Trajectory*)(*iT);
	  
      G4int parentID=Traj->GetParentID();
      part = Traj->GetPDGEncoding();
	  
      G4bool saveTrajectory=false;

      // store primaries
      if((parentID==0)&&(BDSGlobalConstants::Instance()->GetStoreTrajectory()))
	{saveTrajectory = true;}
      // store muons
      else if((std::abs(part)==13)&&(BDSGlobalConstants::Instance()->GetStoreMuonTrajectories()))
	{saveTrajectory = true;}
      // store neutrons
      else if((part==2112)&&(BDSGlobalConstants::Instance()->GetStoreNeutronTrajectories()))
	{saveTrajectory = true;}
	  
      if(!saveTrajectory) continue;
      
      for(G4int j=0; j<Traj->GetPointEntries(); j++)
	{
	  G4TrajectoryPoint* TrajPoint=(G4TrajectoryPoint*)Traj->GetPoint(j);
	  G4ThreeVector TrajPos=TrajPoint->GetPosition();
	  
	  x = TrajPos.x() / CLHEP::m;
	  y = TrajPos.y() / CLHEP::m;
	  z = TrajPos.z() / CLHEP::m;
	  
	  TrajTree->Fill();
	}
    }
}

void BDSOutputROOTBase::FillHit(BDSEnergyCounterHit* hit)
{
  //copy variables from hit to root variables
  X          = hit->GetX()/CLHEP::m;
  Y          = hit->GetY()/CLHEP::m;
  Z          = hit->GetZ()/CLHEP::m;
  S          = hit->GetS()/CLHEP::m;
  x          = hit->Getx()/CLHEP::m;
  y          = hit->Gety()/CLHEP::m;
  z          = hit->Getz()/CLHEP::m;
  E          = hit->GetEnergy()/CLHEP::GeV;
  stepLength = hit->GetStepLength()/CLHEP::m;
  weight     = hit->GetWeight();
  part       = hit->GetPartID();
  turnnumber = hit->GetTurnsTaken();
  eventno    = hit->GetEventNo();
}

void BDSOutputROOTBase::WriteEnergyLoss(BDSEnergyCounterHitsCollection* hc)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4int n_hit = hc->entries();
  for (G4int i=0;i<n_hit;i++)
    {
      //all regions fill the energy loss tree....
      BDSEnergyCounterHit* hit = (*hc)[i];
      FillHit(hit);
      EnergyLossTree->Fill();
      
      if(hit->GetPrecisionRegion())
	{
	  //Only the precision region fills this tree, preserving every hit, its position and weight,
	  //instead of summing weighted energy in each beam line component.
	  //name - convert to char array for root
	  G4String temp = hit->GetName();
	  strncpy(volumeName,temp.c_str(),sizeof(volumeName)-1);
	  PrecisionRegionEnergyLossTree->Fill();
	}
    }
}

void BDSOutputROOTBase::WritePrimaryLoss(BDSEnergyCounterHit* hit)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  //copy variables from hit to root variables
  FillHit(hit);
  //write to file
  PrimaryLossTree->Fill();
}

void BDSOutputROOTBase::WritePrimaryHit(BDSEnergyCounterHit* hit)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  //copy variables from hit to root variables
  FillHit(hit);
  //write to file
  PrimaryHitsTree->Fill();
}

  
void BDSOutputROOTBase::WriteTunnelHits(BDSTunnelHitsCollection* tunnelHits)
{
  for (G4int i = 0; i < tunnelHits->entries(); i++)
    {
      BDSTunnelHit* hit = (*tunnelHits)[i];
      E_tun     = hit->GetEnergy()/CLHEP::GeV;
      S_tun     = hit->GetS()/CLHEP::m;
      r_tun     = hit->Getr()/CLHEP::m;
      angle_tun = hit->Gettheta();
      TunnelLossTree->Fill();

      // x,y,w -> angle,s,energy
      tunnelHitsHisto->Fill(angle_tun, S_tun, E_tun);
    }
}


void BDSOutputROOTBase::WriteHistogram(BDSHistogram1D* hIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4String hname = hIn->GetName();
  hname = BDS::PrepareSafeName(hname);

  std::vector<G4double> binLowerEdges = hIn->GetBinLowerEdges();
  if (binLowerEdges.size() < 1)
    {return;} //no bins - don't write the histogram
  binLowerEdges.push_back(hIn->GetLastBin().GetUpperEdge()); //root requires nbins+1

  //always construct the histogram by bin edges - works both with constant
  //and variable bin width histograms
  // &vector[0] gives an array to the contents of the vector - ensured as
  // standard is that the vector's contents are contiguous
  TH1D* h = new TH1D(hname, hIn->GetTitle(), hIn->GetNBins(), &binLowerEdges[0]);
  // set label titles
  h->GetXaxis()->SetTitle(hIn->GetXLabel());
  h->GetYaxis()->SetTitle(hIn->GetYLabel());
  h->GetXaxis()->CenterTitle();
  h->GetYaxis()->CenterTitle();
  G4int i = 1;
  for (auto const& bin : *hIn)
    {
      h->SetBinContent(i, bin.GetValue());
      h->SetBinError(i, bin.GetError());
      i++;
    }
  //over / underflow manually set
  h->SetBinContent(0,hIn->GetUnderflowBin().GetValue()); //underflow
  h->SetBinContent(0,hIn->GetUnderflowBin().GetError());
  h->SetBinContent(h->GetNbinsX()+1,hIn->GetOverflowBin().GetValue()); //overflow
  h->SetBinContent(h->GetNbinsX()+1,hIn->GetOverflowBin().GetError());

  h->SetEntries(hIn->GetNEntries());
  
  h->Write(); // as commit actually closes a file as does write..
  delete h;
}

void BDSOutputROOTBase::Commit()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  Write();
  outputFileNumber++;
  Init();
}

void BDSOutputROOTBase::Write()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  if(theRootOutputFile && theRootOutputFile->IsOpen())
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " - ROOT file found and open, writing." << G4endl;
#endif
      //Dump all other quantities to file...
      theRootOutputFile->Write(0,TObject::kOverwrite);
      theRootOutputFile->Close();
      delete theRootOutputFile;
      theRootOutputFile=nullptr;
    }
  G4cout << __METHOD_NAME__ << " ...finished." << G4endl;
}
