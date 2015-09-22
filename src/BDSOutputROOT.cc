#ifdef USE_ROOT
#include "BDSOutputROOT.hh"

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSSampler.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSTrajectory.hh"
#include "BDSUtilities.hh"
#include "BDSHistogram.hh"

#include <string>

BDSOutputROOT::BDSOutputROOT()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "output format ROOT"<<G4endl;
#endif
  theRootOutputFile = nullptr;
  PrecisionRegionEnergyLossTree = nullptr;
  EnergyLossTree                = nullptr;
  PrimaryHitsTree               = nullptr;
  PrimaryLossTree               = nullptr;
  Init(); // activate the output
}

BDSOutputROOT::~BDSOutputROOT()
{
  if (theRootOutputFile && theRootOutputFile->IsOpen())
    {theRootOutputFile->Write(0,TObject::kOverwrite);}
}

void BDSOutputROOT::BuildSamplerTree(G4String name)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  TTree* SamplerTree = new TTree(name, "Sampler output");
  
  SamplerTree->Branch("E0",  &E0,  "E0/F" ); // (GeV)
  SamplerTree->Branch("x0",  &x0,  "x0/F" ); // (m)
  SamplerTree->Branch("y0",  &y0,  "y0/F" ); // (m)
  SamplerTree->Branch("z0",  &z0,  "z0/F" ); // (m)
  SamplerTree->Branch("xp0", &xp0, "xp0/F"); // (rad)
  SamplerTree->Branch("yp0", &yp0, "yp0/F"); // (rad)
  SamplerTree->Branch("zp0", &zp0, "zp0/F"); // (rad)
  SamplerTree->Branch("t0",  &t0,  "t0/F" ); // (ns)

  SamplerTree->Branch("E_prod",  &E_prod,  "E_prod/F" ); // (GeV)
  SamplerTree->Branch("x_prod",  &x_prod,  "x_prod/F" ); // (m)
  SamplerTree->Branch("y_prod",  &y_prod,  "y_prod/F" ); // (m)
  SamplerTree->Branch("z_prod",  &z_prod,  "z_prod/F" ); // (m)
  SamplerTree->Branch("xp_prod", &xp_prod, "xp_prod/F"); // (rad)
  SamplerTree->Branch("yp_prod", &yp_prod, "yp_prod/F"); // (rad)
  SamplerTree->Branch("zp_prod", &zp_prod, "zp_prod/F"); // (rad)
  SamplerTree->Branch("t_prod",  &t_prod,  "t_prod/F" ); // (ns)

  SamplerTree->Branch("E_lastScat",  &E_lastScat,  "E_lastScat/F");  // (GeV)
  SamplerTree->Branch("x_lastScat",  &x_lastScat,  "x_lastScat/F");  // (m)
  SamplerTree->Branch("y_lastScat",  &y_lastScat,  "y_lastScat/F");  // (m)
  SamplerTree->Branch("z_lastScat",  &z_lastScat,  "z_lastScat/F");  // (m)
  SamplerTree->Branch("xp_lastScat", &xp_lastScat, "xp_lastScat/F"); // (rad)
  SamplerTree->Branch("yp_lastScat", &yp_lastScat, "yp_lastScat/F"); // (rad)
  SamplerTree->Branch("zp_lastScat", &zp_lastScat, "zp_lastScat/F"); // (rad)
  SamplerTree->Branch("t_lastScat",  &t_lastScat,  "t_lastScat/F");  // (ns)
  
  SamplerTree->Branch("E",  &E,  "E/F");  // (GeV)
  SamplerTree->Branch("x",  &x,  "x/F");  // (m)
  SamplerTree->Branch("y",  &y,  "y/F");  // (m)
  SamplerTree->Branch("z",  &z,  "z/F");  // (m)
  SamplerTree->Branch("xp", &xp, "xp/F"); // (rad)
  SamplerTree->Branch("yp", &yp, "yp/F"); // (rad)
  SamplerTree->Branch("zp", &zp, "zp/F"); // (rad)
  SamplerTree->Branch("t",  &t,  "t/F");  // (ns)
  
  SamplerTree->Branch("X",  &X,  "X/F");  // (m)
  SamplerTree->Branch("Y",  &Y,  "Y/F");  // (m)
  SamplerTree->Branch("Z",  &Z,  "Z/F");  // (m)
  SamplerTree->Branch("Xp", &Xp, "Xp/F"); // (rad)
  SamplerTree->Branch("Yp", &Yp, "Yp/F"); // (rad)
  SamplerTree->Branch("Zp", &Zp, "Zp/F"); // (rad)
 
  SamplerTree->Branch("S",  &S,   "S/F"); // (m)
  
  SamplerTree->Branch("weight",     &weight,     "weight/F");
  SamplerTree->Branch("partID",     &part,       "partID/I");
  SamplerTree->Branch("nEvent",     &eventno,    "nEvent/I");
  SamplerTree->Branch("parentID",   &pID,        "parentID/I");
  SamplerTree->Branch("trackID",    &track_id,   "trackID/I");
  SamplerTree->Branch("turnnumber", &turnnumber, "turnnumber/I");
  SamplerTree->Branch("process",    &process);
}

void BDSOutputROOT::Init()
{
  const BDSExecOptions*     execOptions     = BDSExecOptions::Instance();
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();
  // set up the root file
  G4String basefilename = execOptions->GetOutputFilename();
  // if more than one file add number (starting at 0)
  int evntsPerNtuple = globalConstants->GetNumberOfEventsPerNtuple();
  if (evntsPerNtuple>0 && globalConstants->GetNumberToGenerate()>evntsPerNtuple) {
    basefilename += "_" + std::to_string(outputFileNumber);
  }
  filename = basefilename + ".root";
  // policy: overwrite if output filename specifically set, otherwise increase number
  // always check in interactive mode
  if (!execOptions->GetOutputFilenameSet() || !execOptions->GetBatch()) {
    // check if file exists
    int nTimeAppended = 1;
    while (BDS::FileExists(filename)) {
      // if exists remove trailing .root
      filename = basefilename + "-" + std::to_string(nTimeAppended);
      filename += ".root";
      nTimeAppended +=1;
    }
  }
  
  G4cout<<"Setting up new file: "<<filename<<G4endl;
  theRootOutputFile=new TFile(filename,"RECREATE", "BDS output file");

  // Build sampler tree
  G4String primariesSamplerName="Primaries";
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " building sampler tree named: " << primariesSamplerName << G4endl;
#endif
  BuildSamplerTree(primariesSamplerName);
  for(G4int i=0;i<BDSSampler::GetNSamplers();i++)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " building sampler tree number: " << i << G4endl;
#endif
      G4String name=BDSSampler::outputNames[i];
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " named: " << name << G4endl;
#endif
      BuildSamplerTree(name);
    }
  for(G4int i=0;i<BDSSamplerCylinder::GetNSamplers();i++)
    {
      G4String name=BDSSamplerCylinder::outputNames[i];
      BuildSamplerTree(name);
    }

  if(globalConstants->GetStoreTrajectory() || globalConstants->GetStoreMuonTrajectories() || globalConstants->GetStoreNeutronTrajectories()) 
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
  PrecisionRegionEnergyLossTree->Branch("weight",     &weight,     "weight/F");
  PrecisionRegionEnergyLossTree->Branch("partID",     &part,       "partID/I");
  PrecisionRegionEnergyLossTree->Branch("volumeName", &volumeName, "volumeName/C");
  PrecisionRegionEnergyLossTree->Branch("turnnumber", &turnnumber, "turnnumber/I");
  PrecisionRegionEnergyLossTree->Branch("eventNo",    &eventno,    "eventNo/I");
}

void BDSOutputROOT::WriteRootHit(G4String Name, 
				 G4double InitTotalEnergy, 
				 G4double InitX, 
				 G4double InitY, 
				 G4double InitZ, 
				 G4double InitXPrime, 
				 G4double InitYPrime, 
				 G4double InitZPrime, 
				 G4double InitT, 
				 G4double ProdTotalEnergy, 
				 G4double ProdX, 
				 G4double ProdY, 
				 G4double ProdZ, 
				 G4double ProdXPrime, 
				 G4double ProdYPrime, 
				 G4double ProdZPrime, 
				 G4double ProdT, 
				 G4double LastScatTotalEnergy, 
				 G4double LastScatX, 
				 G4double LastScatY, 
				 G4double LastScatZ, 
				 G4double LastScatXPrime, 
				 G4double LastScatYPrime, 
				 G4double LastScatZPrime, 
				 G4double LastScatT, 
				 G4double TotalEnergy, 
				 G4double LocalX, 
				 G4double LocalY, 
				 G4double LocalZ, 
				 G4double LocalXPrime, 
				 G4double LocalYPrime, 
				 G4double LocalZPrime, 
				 G4double GlobalT, 
				 G4double GlobalX, 
				 G4double GlobalY, 
				 G4double GlobalZ, 
				 G4double GlobalXPrime, 
				 G4double GlobalYPrime, 
				 G4double GlobalZPrime, 
				 G4double GlobalS, 
				 G4double Weight, 
				 G4int    PDGtype, 
				 G4int    EventNo, 
				 G4int    ParentID,
				 G4int    TrackID, 
				 G4int    TurnsTaken,
				 G4String Process)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  TTree* sTree=(TTree*)gDirectory->Get(Name);
  if(!sTree) {
    G4String errorString = "BDSOutputROOT: ROOT Sampler " + Name + " not found!";
    G4Exception(errorString.c_str(), "-1", FatalException, "");
  }
  E0          = InitTotalEnergy/ CLHEP::GeV;
  x0          = InitX          / CLHEP::m;
  y0          = InitY          / CLHEP::m;
  z0          = InitZ          / CLHEP::m;
  xp0         = InitXPrime     / CLHEP::radian;
  yp0         = InitYPrime     / CLHEP::radian;
  zp0         = InitZPrime     / CLHEP::radian;
  t0          = InitT          / CLHEP::ns;
  E_prod      = ProdTotalEnergy/ CLHEP::GeV;
  x_prod      = ProdX          / CLHEP::m;
  y_prod      = ProdY          / CLHEP::m;
  z_prod      = ProdZ          / CLHEP::m;
  xp_prod     = ProdXPrime     / CLHEP::radian;
  yp_prod     = ProdYPrime     / CLHEP::radian;
  zp_prod     = ProdZPrime     / CLHEP::radian;
  t_prod      = ProdT          / CLHEP::ns;
  E_lastScat  = LastScatTotalEnergy/ CLHEP::GeV;
  x_lastScat  = LastScatX      / CLHEP::m;
  y_lastScat  = LastScatY      / CLHEP::m;
  z_lastScat  = LastScatZ      / CLHEP::m;
  xp_lastScat = LastScatXPrime / CLHEP::radian;
  yp_lastScat = LastScatYPrime / CLHEP::radian;
  zp_lastScat = LastScatZPrime / CLHEP::radian;
  t_lastScat  = LastScatT      / CLHEP::ns;
  E           = TotalEnergy    / CLHEP::GeV;
  x           = LocalX         / CLHEP::m;
  y           = LocalY         / CLHEP::m;
  z           = LocalZ         / CLHEP::m;
  xp          = LocalXPrime    / CLHEP::radian;
  yp          = LocalYPrime    / CLHEP::radian;
  zp          = LocalZPrime    / CLHEP::radian;
  t           = GlobalT        / CLHEP::ns;
  X           = GlobalX        / CLHEP::m;
  Y           = GlobalY        / CLHEP::m;
  Z           = GlobalZ        / CLHEP::m;
  Xp          = GlobalXPrime   / CLHEP::radian;
  Yp          = GlobalYPrime   / CLHEP::radian;
  Zp          = GlobalZPrime   / CLHEP::radian;
  S           = GlobalS        / CLHEP::m;
  weight      = Weight;
  part        = PDGtype; 
  eventno     = EventNo; 
  pID         = ParentID; 
  track_id    = TrackID;
  turnnumber  = TurnsTaken;
  process     = Process;
  sTree->Fill();
}

void BDSOutputROOT::WritePrimary(G4String samplerName, 
				 G4double totalEnergy,
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
  WriteRootHit(samplerName, 
	       totalEnergy, 
	       x0, y0, z0, 
	       xp, yp, zp, 
	       t, 
	       totalEnergy, 
	       x0, y0, z0, 
	       xp, yp, zp, 
	       t, 
	       totalEnergy, 
	       x0, y0, z0, 
	       xp, yp, zp, 
	       t, 
	       totalEnergy, 
	       x0, y0, z0, 
	       xp, yp, zp, 
	       t, 
	       0,0,0,0,0,0,0,
	       weight, 
	       PDGType, 
	       nEvent, 
	       0, 
	       1, 
	       turnsTaken,
	       "");
}

void BDSOutputROOT::WriteHits(BDSSamplerHitsCollection *hc)
{
  G4String name;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " hc->entries() = " << hc->entries() << G4endl;
#endif
  for (G4int i=0; i<hc->entries(); i++)
    {
      G4String name = (*hc)[i]->GetName();
#ifdef BDSDEBUG
      G4cout << "Writing hit to sampler " << name << G4endl;
#endif
      WriteRootHit(name,
		   (*hc)[i]->GetInitTotalEnergy(),
		   (*hc)[i]->GetInitX(),
		   (*hc)[i]->GetInitY(),
		   (*hc)[i]->GetInitZ(),
		   (*hc)[i]->GetInitXPrime(),
		   (*hc)[i]->GetInitYPrime(),
		   (*hc)[i]->GetInitZPrime(),
		   (*hc)[i]->GetInitT(),
		   (*hc)[i]->GetProdTotalEnergy(),
		   (*hc)[i]->GetProdX(),
		   (*hc)[i]->GetProdY(),
		   (*hc)[i]->GetProdZ(),
		   (*hc)[i]->GetProdXPrime(),
		   (*hc)[i]->GetProdYPrime(),
		   (*hc)[i]->GetProdZPrime(),
		   (*hc)[i]->GetProdT(),
		   (*hc)[i]->GetLastScatTotalEnergy(),
		   (*hc)[i]->GetLastScatX(),
		   (*hc)[i]->GetLastScatY(),
		   (*hc)[i]->GetLastScatZ(),
		   (*hc)[i]->GetLastScatXPrime(),
		   (*hc)[i]->GetLastScatYPrime(),
		   (*hc)[i]->GetLastScatZPrime(),
		   (*hc)[i]->GetLastScatT(),
		   (*hc)[i]->GetTotalEnergy(),
		   (*hc)[i]->GetX(),
		   (*hc)[i]->GetY(),
		   (*hc)[i]->GetZ(),
		   (*hc)[i]->GetXPrime(),
		   (*hc)[i]->GetYPrime(),
		   (*hc)[i]->GetZPrime(),
		   (*hc)[i]->GetT(),
		   (*hc)[i]->GetGlobalX(),
		   (*hc)[i]->GetGlobalY(),
		   (*hc)[i]->GetGlobalZ(),
		   (*hc)[i]->GetGlobalXPrime(),
		   (*hc)[i]->GetGlobalYPrime(),
		   (*hc)[i]->GetGlobalZPrime(),
		   (*hc)[i]->GetS(),
		   (*hc)[i]->GetWeight(),
		   (*hc)[i]->GetPDGtype(), 
		   (*hc)[i]->GetEventNo(), 
		   (*hc)[i]->GetParentID(), 
		   (*hc)[i]->GetTrackID(),
		   (*hc)[i]->GetTurnsTaken(),
		   (*hc)[i]->GetProcess());
    }
}

/// write a trajectory to file
void BDSOutputROOT::WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4String name = "Trajectories";
  
  TTree* TrajTree=(TTree*)gDirectory->Get(name);
  
  if(TrajTree == nullptr) { G4cerr<<"TrajTree=nullptr"<<G4endl; return;}
  
  for(auto iT=TrajVec.begin();iT<TrajVec.end();iT++)
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

void BDSOutputROOT::FillHit(BDSEnergyCounterHit* hit)
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
  weight     = hit->GetWeight();
  part       = hit->GetPartID();
  turnnumber = hit->GetTurnsTaken();
  eventno    = hit->GetEventNo();
}

void BDSOutputROOT::WriteEnergyLoss(BDSEnergyCounterHitsCollection* hc)
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
      
      if(hit->GetPrecisionRegion()){ //Only the precision region fills this tree, preserving every hit, its position and weight, instead of summing weighted energy in each beam line component.
	//name - convert to char array for root
	G4String temp = hit->GetName();
	strncpy(volumeName,temp.c_str(),sizeof(volumeName)-1);
	PrecisionRegionEnergyLossTree->Fill();
      }
    }
}

void BDSOutputROOT::WritePrimaryLoss(BDSEnergyCounterHit* hit)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  //copy variables from hit to root variables
  FillHit(hit);
  //write to file
  PrimaryLossTree->Fill();
}

void BDSOutputROOT::WritePrimaryHit(BDSEnergyCounterHit* hit)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  //copy variables from hit to root variables
  FillHit(hit);
  //write to file
  PrimaryHitsTree->Fill();
}

  
void BDSOutputROOT::WriteTunnelHits(BDSTunnelHitsCollection* tunnelHits)
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


void BDSOutputROOT::WriteHistogram(BDSHistogram1D* hIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4String hname = hIn->GetName();
  hname = BDS::PrepareSafeName(hname);

  std::vector<G4double> binLowerEdges = hIn->GetBinLowerEdges();
  if (binLowerEdges.size() < 1)
    {return;} //no bins - don't write the histogram
  binLowerEdges.push_back(hIn->GetLastBin()->GetUpperEdge()); //root requires nbins+1

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
  G4int i;
  for(hIn->first(),i = 1;!hIn->isDone();hIn->next(), i++)
    {
      BDSBin* currentBin = hIn->currentBin();
      h->SetBinContent(i,currentBin->GetValue());
      h->SetBinError(i,  currentBin->GetError());
    }
  //over / underflow manually set
  h->SetBinContent(0,hIn->GetUnderflowBin()->GetValue()); //underflow
  h->SetBinContent(0,hIn->GetUnderflowBin()->GetError());
  h->SetBinContent(h->GetNbinsX()+1,hIn->GetOverflowBin()->GetValue()); //overflow
  h->SetBinContent(h->GetNbinsX()+1,hIn->GetOverflowBin()->GetError());

  h->SetEntries(hIn->GetNEntries());
  
  h->Write(); // as commit actually closes a file as does write..
  delete h;
}

void BDSOutputROOT::Commit()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  Write();
  outputFileNumber++;
  Init();
}

void BDSOutputROOT::Write()
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
#endif
