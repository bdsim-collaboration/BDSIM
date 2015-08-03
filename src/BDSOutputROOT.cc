#ifdef USE_ROOT
#include "BDSOutputROOT.hh"

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSSampler.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSTrajectory.hh"
#include "BDSUtilities.hh"
#include "BDSHistogram.hh"


BDSOutputROOT::BDSOutputROOT():BDSOutputBase()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "output format ROOT"<<G4endl;
#endif
  theRootOutputFile = nullptr;
  PrecisionRegionEnergyLossTree = nullptr;
  EnergyLossTree    = nullptr;
  PrimaryHitsTree   = nullptr;
  PrimaryLossTree   = nullptr;
  Init(); // activate the output
}

BDSOutputROOT::~BDSOutputROOT()
{
  if (theRootOutputFile && theRootOutputFile->IsOpen())
    {theRootOutputFile->Write();}
}

void BDSOutputROOT::BuildSamplerTree(G4String name)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  TTree* SamplerTree = new TTree(name, "Sampler output");
  
  SamplerTree->Branch("E0",&E0,"E0/F"); // (GeV)
  SamplerTree->Branch("x0",&x0,"x0/F"); // (mum)
  SamplerTree->Branch("y0",&y0,"y0/F"); // (mum)
  SamplerTree->Branch("z0",&z0,"z0/F"); // (m)
  SamplerTree->Branch("xp0",&xp0,"xp0/F"); // (rad)
  SamplerTree->Branch("yp0",&yp0,"yp0/F"); // (rad)
  SamplerTree->Branch("zp0",&zp0,"zp0/F"); // (rad)
  SamplerTree->Branch("t0",&t0,"t0/F"); // (ns)

  SamplerTree->Branch("E_prod",&E_prod,"E_prod/F"); // (GeV)
  SamplerTree->Branch("x_prod",&x_prod,"x_prod/F"); // (mum)
  SamplerTree->Branch("y_prod",&y_prod,"y_prod/F"); // (mum)
  SamplerTree->Branch("z_prod",&z_prod,"z_prod/F"); // (m)
  SamplerTree->Branch("xp_prod",&xp_prod,"xp_prod/F"); // (rad)
  SamplerTree->Branch("yp_prod",&yp_prod,"yp_prod/F"); // (rad)
  SamplerTree->Branch("zp_prod",&zp_prod,"zp_prod/F"); // (rad)
  SamplerTree->Branch("t_prod",&t_prod,"t_prod/F"); // (ns)

  SamplerTree->Branch("E_lastScat",&E_lastScat,"E_lastScat/F"); // (GeV)
  SamplerTree->Branch("x_lastScat",&x_lastScat,"x_lastScat/F"); // (mum)
  SamplerTree->Branch("y_lastScat",&y_lastScat,"y_lastScat/F"); // (mum)
  SamplerTree->Branch("z_lastScat",&z_lastScat,"z_lastScat/F"); // (m)
  SamplerTree->Branch("xp_lastScat",&xp_lastScat,"xp_lastScat/F"); // (rad)
  SamplerTree->Branch("yp_lastScat",&yp_lastScat,"yp_lastScat/F"); // (rad)
  SamplerTree->Branch("zp_lastScat",&zp_lastScat,"zp_lastScat/F"); // (rad)
  SamplerTree->Branch("t_lastScat",&t_lastScat,"t_lastScat/F"); // (ns)
  
  SamplerTree->Branch("E",&E,"E/F"); // (GeV)
  SamplerTree->Branch("x",&x,"x/F"); // (mum)
  SamplerTree->Branch("y",&y,"y/F"); // (mum)
  SamplerTree->Branch("z",&z,"z/F"); // (m)
  SamplerTree->Branch("xp",&xp,"xp/F"); // (rad)
  SamplerTree->Branch("yp",&yp,"yp/F"); // (rad)
  SamplerTree->Branch("zp",&zp,"zp/F"); // (rad)
  SamplerTree->Branch("t",&t,"t/F"); // (ns)
  
  SamplerTree->Branch("X",&X,"X/F"); // (mum)
  SamplerTree->Branch("Y",&Y,"Y/F"); // (mum)
  SamplerTree->Branch("Z",&Z,"Z/F"); // (m)
  SamplerTree->Branch("Xp",&Xp,"Xp/F"); // (rad)
  SamplerTree->Branch("Yp",&Yp,"Yp/F"); // (rad)
  SamplerTree->Branch("Zp",&Zp,"Zp/F"); // (rad)
  
  SamplerTree->Branch("s",&s,"s/F"); // (m)
  
  SamplerTree->Branch("weight",&weight,"weight/F");
  SamplerTree->Branch("partID",&part,"partID/I");
  SamplerTree->Branch("nEvent",&nev,"nEvent/I");
  SamplerTree->Branch("parentID",&pID,"parentID/I");
  SamplerTree->Branch("trackID",&track_id,"trackID/I");
  SamplerTree->Branch("turnnumber",&turnnumber,"turnnumber/I");
}

void BDSOutputROOT::Init()
{
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();
  // set up the root file
  filename = BDSExecOptions::Instance()->GetOutputFilename();
  // if more than one file add number (starting at 0)
  int evntsPerNtuple = globalConstants->GetNumberOfEventsPerNtuple();
  if (evntsPerNtuple>0 && globalConstants->GetNumberToGenerate()>evntsPerNtuple) {
    filename += "_" + BDS::StringFromInt(outputFileNumber);
  }
  filename += ".root";
  
  G4cout<<"Setting up new file: "<<filename<<G4endl;
  theRootOutputFile=new TFile(filename,"RECREATE", "BDS output file");

  //build sampler tree
  G4String primariesSamplerName="primaries";
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
      TrajTree->Branch("x",&x,"x/F"); // (mum)
      TrajTree->Branch("y",&y,"y/F"); // (mum)
      TrajTree->Branch("z",&z,"z/F"); // (m)
      TrajTree->Branch("part",&part,"part/I");
    }

  //Energy loss tree setup
  EnergyLossTree= new TTree("ElossTree", "Energy Loss");
  EnergyLossTree->Branch("s",&S_el,"s/F"); // (m)
  EnergyLossTree->Branch("E",&E_el,"E/F"); // (GeV)
  EnergyLossTree->Branch("weight", &weight_el, "weight/F"); // weight

  //Primary loss tree setup
  PrimaryLossTree  = new TTree("PlossTree", "Primary Losses");
  PrimaryLossTree->Branch("X",          &X_pl,          "X/F"); // (m)
  PrimaryLossTree->Branch("Y",          &Y_pl,          "Y/F"); // (m)
  PrimaryLossTree->Branch("Z",          &Z_pl,          "Z/F"); // (m)
  PrimaryLossTree->Branch("S",          &S_pl,          "S/F"); // (m)
  PrimaryLossTree->Branch("x",          &x_pl,          "x/F"); // (m)
  PrimaryLossTree->Branch("y",          &y_pl,          "y/F"); // (m)
  PrimaryLossTree->Branch("z",          &z_pl,          "z/F"); // (m)
  PrimaryLossTree->Branch("E",          &E_pl,          "s/F"); // (GeV)
  PrimaryLossTree->Branch("weight",     &weight_pl,     "weight/F");
  PrimaryLossTree->Branch("partID",     &part_pl,       "partID/I");
  PrimaryLossTree->Branch("turnnumber", &turnnumber_pl, "turnnumber/I");
  PrimaryLossTree->Branch("eventNo",    &eventno_pl,    "eventNo/I");

  //Primary hits tree setup
  PrimaryHitsTree  = new TTree("PhitsTree", "Primary Hits");
  PrimaryHitsTree->Branch("X",          &X_ph,          "X/F"); // (m)
  PrimaryHitsTree->Branch("Y",          &Y_ph,          "Y/F"); // (m)
  PrimaryHitsTree->Branch("Z",          &Z_ph,          "Z/F"); // (m)
  PrimaryHitsTree->Branch("S",          &S_ph,          "S/F"); // (m)
  PrimaryHitsTree->Branch("x",          &x_ph,          "x/F"); // (m)
  PrimaryHitsTree->Branch("y",          &y_ph,          "y/F"); // (m)
  PrimaryHitsTree->Branch("z",          &z_ph,          "z/F"); // (m)
  PrimaryHitsTree->Branch("E",          &E_ph,          "E/F"); // (GeV)
  PrimaryHitsTree->Branch("weight",     &weight_ph,     "weight/F");
  PrimaryHitsTree->Branch("partID",     &part_ph,       "partID/I");
  PrimaryHitsTree->Branch("turnnumber", &turnnumber_ph, "turnnumber/I");
  PrimaryHitsTree->Branch("eventNo",    &eventno_ph,    "eventNo/I");
  
  //Precision region energy loss tree setup
  PrecisionRegionEnergyLossTree= new TTree("PrecisionRegionElossTree", "Energy Loss");
  PrecisionRegionEnergyLossTree->Branch("X",          &X_el_p,          "X/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("Y",          &Y_el_p,          "Y/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("Z",          &Z_el_p,          "Z/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("S",          &S_el_p,          "S/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("x",          &x_el_p,          "x/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("y",          &y_el_p,          "y/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("z",          &z_el_p,          "z/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("E",          &E_el_p,          "E/F"); // (GeV)
  PrecisionRegionEnergyLossTree->Branch("weight",     &weight_el_p,     "weight/F");
  PrecisionRegionEnergyLossTree->Branch("partID",     &part_el_p,       "partID/I");
  PrecisionRegionEnergyLossTree->Branch("volumeName", &volumeName_el_p, "volumeName/C");
  PrecisionRegionEnergyLossTree->Branch("turnnumber", &turnnumber_el_p, "turnnumber/I");
  PrecisionRegionEnergyLossTree->Branch("eventNo",    &eventno_el_p,    "eventNo/I");
}

void BDSOutputROOT::WriteRootHit(G4String Name, 
				 G4double InitMom, 
				 G4double InitX, 
				 G4double InitY, 
				 G4double InitZ, 
				 G4double InitXPrime, 
				 G4double InitYPrime, 
				 G4double InitZPrime, 
				 G4double InitT, 
				 G4double ProdMom, 
				 G4double ProdX, 
				 G4double ProdY, 
				 G4double ProdZ, 
				 G4double ProdXPrime, 
				 G4double ProdYPrime, 
				 G4double ProdZPrime, 
				 G4double ProdT, 
				 G4double LastScatMom, 
				 G4double LastScatX, 
				 G4double LastScatY, 
				 G4double LastScatZ, 
				 G4double LastScatXPrime, 
				 G4double LastScatYPrime, 
				 G4double LastScatZPrime, 
				 G4double LastScatT, 
				 G4double Mom, 
				 G4double X, 
				 G4double Y, 
				 G4double Z, 
				 G4double XPrime, 
				 G4double YPrime, 
				 G4double ZPrime, 
				 G4double T, 
				 G4double GlobalX, 
				 G4double GlobalY, 
				 G4double GlobalZ, 
				 G4double GlobalXPrime, 
				 G4double GlobalYPrime, 
				 G4double GlobalZPrime, 
				 G4double S, 
				 G4double Weight, 
				 G4int    PDGtype, 
				 G4int    EventNo, 
				 G4int    ParentID,
				 G4int    TrackID, 
				 G4int    TurnsTaken)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  TTree* sTree=(TTree*)gDirectory->Get(Name);
  if(!sTree) G4Exception("BDSOutputROOT: ROOT Sampler not found!", "-1", FatalException, "");
  E0          = InitMom     / CLHEP::GeV;
  x0          = InitX       / CLHEP::micrometer;
  y0          = InitY       / CLHEP::micrometer;
  z0          = InitZ       / CLHEP::m;
  xp0         = InitXPrime  / CLHEP::radian;
  yp0         = InitYPrime  / CLHEP::radian;
  zp0         = InitZPrime  / CLHEP::radian;
  t0          = InitT       / CLHEP::ns;
  E_prod      = ProdMom     / CLHEP::GeV;
  x_prod      = ProdX       / CLHEP::micrometer;
  y_prod      = ProdY       / CLHEP::micrometer;
  z_prod      = ProdZ       / CLHEP::m;
  xp_prod     = ProdXPrime  / CLHEP::radian;
  yp_prod     = ProdYPrime  / CLHEP::radian;
  zp_prod     = ProdZPrime  / CLHEP::radian;
  t_prod      = ProdT       / CLHEP::ns;
  E_lastScat  = LastScatMom / CLHEP::GeV;
  x_lastScat  = LastScatX   / CLHEP::micrometer;
  y_lastScat  = LastScatY   / CLHEP::micrometer;
  z_lastScat  = LastScatZ   / CLHEP::m;
  xp_lastScat = LastScatXPrime / CLHEP::radian;
  yp_lastScat = LastScatYPrime / CLHEP::radian;
  zp_lastScat = LastScatZPrime / CLHEP::radian;
  t_lastScat  = LastScatT   / CLHEP::ns;

  E           =Mom / CLHEP::GeV;
  //Edep=Edep / CLHEP::GeV;
  x           = X / CLHEP::micrometer;
  y           = Y / CLHEP::micrometer;
  z           = Z / CLHEP::m;
  xp          = XPrime / CLHEP::radian;
  yp          = YPrime / CLHEP::radian;
  zp          = ZPrime / CLHEP::radian;
  t=T / CLHEP::ns;
  X=GlobalX / CLHEP::m;
  Y=GlobalY / CLHEP::m;
  Z=GlobalZ / CLHEP::m;
  Xp=GlobalXPrime / CLHEP::radian;
  Yp=GlobalYPrime / CLHEP::radian;
  Zp=GlobalZPrime / CLHEP::radian;
  s=S / CLHEP::m;
  weight=Weight;
  part=PDGtype; 
  nev=EventNo; 
  pID=ParentID; 
  track_id=TrackID;
  turnnumber=TurnsTaken;
  sTree->Fill();
}

void BDSOutputROOT::WritePrimary(G4String samplerName, 
				 G4double E,
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
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  WriteRootHit(samplerName, 
	       E, 
	       x0, y0, z0, 
	       xp, yp, zp, 
	       t, 
	       E, 
	       x0, y0, z0, 
	       xp, yp, zp, 
	       t, 
	       E, 
	       x0, y0, z0, 
	       xp, yp, zp, 
	       t, 
	       E, 
	       x0, y0, z0, 
	       xp, yp, zp, 
	       t, 
	       0,0,0,0,0,0,0,
	       weight, 
	       PDGType, 
	       nEvent, 
	       0, 
	       1, 
	       TurnsTaken);
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
		   (*hc)[i]->GetInitMom(),
		   (*hc)[i]->GetInitX(),
		   (*hc)[i]->GetInitY(),
		   (*hc)[i]->GetInitZ(),
		   (*hc)[i]->GetInitXPrime(),
		   (*hc)[i]->GetInitYPrime(),
		   (*hc)[i]->GetInitZPrime(),
		   (*hc)[i]->GetInitT(),
		   (*hc)[i]->GetProdMom(),
		   (*hc)[i]->GetProdX(),
		   (*hc)[i]->GetProdY(),
		   (*hc)[i]->GetProdZ(),
		   (*hc)[i]->GetProdXPrime(),
		   (*hc)[i]->GetProdYPrime(),
		   (*hc)[i]->GetProdZPrime(),
		   (*hc)[i]->GetProdT(),
		   (*hc)[i]->GetLastScatMom(),
		   (*hc)[i]->GetLastScatX(),
		   (*hc)[i]->GetLastScatY(),
		   (*hc)[i]->GetLastScatZ(),
		   (*hc)[i]->GetLastScatXPrime(),
		   (*hc)[i]->GetLastScatYPrime(),
		   (*hc)[i]->GetLastScatZPrime(),
		   (*hc)[i]->GetLastScatT(),
		   (*hc)[i]->GetMom(),
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
		   (*hc)[i]->GetTurnsTaken()
		   );
    }
}

/// write a trajectory to file
void BDSOutputROOT::WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  //  G4int tID;
  G4TrajectoryPoint* TrajPoint;
  G4ThreeVector TrajPos;
  
  TTree* TrajTree;
  
  G4String name = "Trajectories";
  
  TrajTree=(TTree*)gDirectory->Get(name);
  
  if(TrajTree == nullptr) { G4cerr<<"TrajTree=nullptr"<<G4endl; return;}
  
  if(TrajVec.size())
    {
      std::vector<BDSTrajectory*>::iterator iT;
      
      for(iT=TrajVec.begin();iT<TrajVec.end();iT++)
	{
	  G4Trajectory* Traj=(G4Trajectory*)(*iT);
	  
	  //	  tID=Traj->GetTrackID();	      
	  
	  G4int parentID=Traj->GetParentID();
	  part = Traj->GetPDGEncoding();
	  
	  G4bool saveTrajectory=true;
	  
	  if(!((parentID==0)&&(BDSGlobalConstants::Instance()->GetStoreTrajectory()))){ 
	    if(!((std::abs(part)==13)&&(BDSGlobalConstants::Instance()->GetStoreMuonTrajectories()))){ 
	      if(!((part==2112)&&(BDSGlobalConstants::Instance()->GetStoreNeutronTrajectories()))){ 
		saveTrajectory=false;
	      }
	    }
	  }
	  
	  if(saveTrajectory){
	    for(G4int j=0; j<Traj->GetPointEntries(); j++)
	      {
		TrajPoint=(G4TrajectoryPoint*)Traj->GetPoint(j);
		TrajPos=TrajPoint->GetPosition();
		
		x = TrajPos.x() / CLHEP::m;
		y = TrajPos.y() / CLHEP::m;
		z = TrajPos.z() / CLHEP::m;
		
		TrajTree->Fill();
	      }
	  }
	}
    }
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
      E_el      = hit->GetEnergy()/CLHEP::GeV;
      S_el      = hit->GetS()/CLHEP::m;
      weight_el = hit->GetWeight();
      EnergyLossTree->Fill();
      
      if((*hc)[i]->GetPrecisionRegion()){ //Only the precision region fills this tree, preserving every hit, its position and weight, instead of summing weighted energy in each beam line component.
	weight_el_p     = (*hc)[i]->GetWeight();
	E_el_p          = (*hc)[i]->GetEnergy()/CLHEP::GeV;
	X_el_p          = (*hc)[i]->GetX()/CLHEP::m;
	Y_el_p          = (*hc)[i]->GetY()/CLHEP::m;
	Z_el_p          = (*hc)[i]->GetZ()/CLHEP::m;
	S_el_p          = (*hc)[i]->GetS()/CLHEP::m;
	x_el_p          = (*hc)[i]->Getx()/CLHEP::m;
	y_el_p          = (*hc)[i]->Gety()/CLHEP::m;
	z_el_p          = (*hc)[i]->Getz()/CLHEP::m;
	part_el_p       = (*hc)[i]->GetPartID();
	turnnumber_el_p = (*hc)[i]->GetTurnsTaken();
	eventno_el_p    = (*hc)[i]->GetEventNo();
	//name - convert to char array for root
	G4String temp = (*hc)[i]->GetName()+'\0';
	strncpy(volumeName_el_p,temp.c_str(),sizeof(volumeName_el_p)-1);
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
  X_pl          = hit->GetX()/CLHEP::m;
  Y_pl          = hit->GetY()/CLHEP::m;
  Z_pl          = hit->GetZ()/CLHEP::m;
  S_pl          = hit->GetS()/CLHEP::m;
  x_pl          = hit->Getx()/CLHEP::m;
  y_pl          = hit->Gety()/CLHEP::m;
  z_pl          = hit->Getz()/CLHEP::m;
  E_pl          = hit->GetEnergy()/CLHEP::GeV;
  weight_pl     = hit->GetWeight();
  part_pl       = hit->GetPartID();
  turnnumber_pl = hit->GetTurnsTaken();
  eventno_pl    = hit->GetEventNo();
  
  //write to file
  PrimaryLossTree->Fill();
}

void BDSOutputROOT::WritePrimaryHit(BDSEnergyCounterHit* hit)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  //copy variables from hit to root variables
  X_ph          = hit->GetX()/CLHEP::m;
  Y_ph          = hit->GetY()/CLHEP::m;
  Z_ph          = hit->GetZ()/CLHEP::m;
  S_ph          = hit->GetS()/CLHEP::m;
  x_ph          = hit->Getx()/CLHEP::m;
  y_ph          = hit->Gety()/CLHEP::m;
  z_ph          = hit->Getz()/CLHEP::m;
  E_ph          = hit->GetEnergy()/CLHEP::GeV;
  weight_ph     = hit->GetWeight();
  part_ph       = hit->GetPartID();
  turnnumber_ph = hit->GetTurnsTaken();
  eventno_ph    = hit->GetEventNo();
  
  //write to file
  PrimaryHitsTree->Fill();
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
      theRootOutputFile->Write();
      theRootOutputFile->Close();
      delete theRootOutputFile;
      theRootOutputFile=nullptr;
    }
  G4cout << __METHOD_NAME__ << " ...finished." << G4endl;
}
#endif
