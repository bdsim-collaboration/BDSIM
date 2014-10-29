#ifdef USE_ROOT
#include "BDSOutputROOT.hh"

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSSampler.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSTrajectory.hh"


BDSOutputROOT::BDSOutputROOT():BDSOutputBase()
{
  G4cout<<"output format ROOT"<<G4endl;
  theRootOutputFile = NULL;
  EnergyLossHisto   = NULL;
  PrimaryLossHisto  = NULL;
  PrecisionRegionEnergyLossTree = NULL;
  EnergyLossTree    = NULL;
  PrimaryLossTree   = NULL;
  Init(); // activate the output
}

BDSOutputROOT::~BDSOutputROOT()
{
  if (theRootOutputFile && theRootOutputFile->IsOpen()) {
    theRootOutputFile->Write();
    //      theRootOutputFile->Close();
    //      delete theRootOutputFile;
  }
}

void BDSOutputROOT::BuildSamplerTree(G4String name){
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
  // set up the root file
  filename = BDSExecOptions::Instance()->GetOutputFilename() + "_" 
    + BDSGlobalConstants::Instance()->StringFromInt(outputFileNumber) + ".root";
  
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
      //G4String name="samp"+BDSGlobalConstants::Instance()->StringFromInt(i+1);
      G4String name=BDSSamplerCylinder::outputNames[i];
      BuildSamplerTree(name);
    }

  if(BDSGlobalConstants::Instance()->GetStoreTrajectory() || BDSGlobalConstants::Instance()->GetStoreMuonTrajectories() || BDSGlobalConstants::Instance()->GetStoreNeutronTrajectories()) 
    // create a tree with trajectories
    {
      TTree* TrajTree = new TTree("Trajectories", "Trajectories");
      TrajTree->Branch("x",&x,"x/F"); // (mum)
      TrajTree->Branch("y",&y,"y/F"); // (mum)
      TrajTree->Branch("z",&z,"z/F"); // (m)
      TrajTree->Branch("part",&part,"part/I");
    }

  // build energy loss histogram
  G4int nBins = G4int(BDSGlobalConstants::Instance()->GetSMax()/(BDSGlobalConstants::Instance()->GetElossHistoBinWidth()*CLHEP::m));
  G4double wx=(BDSGlobalConstants::Instance()->GetTunnelRadius()+BDSGlobalConstants::Instance()->GetTunnelOffsetX())*2;
  G4double wy=(BDSGlobalConstants::Instance()->GetTunnelRadius()+BDSGlobalConstants::Instance()->GetTunnelOffsetY())*2;
  G4double bs=BDSGlobalConstants::Instance()->GetComponentBoxSize();
  G4double wmax=std::max(wx,wy);
  wmax=std::max(wmax,bs);

  EnergyLossHisto = new TH1F("ElossHisto", "Energy Loss",nBins,0.,BDSGlobalConstants::Instance()->GetSMax()/CLHEP::m);
  EnergyLossTree= new TTree("ElossTree", "Energy Loss");
  EnergyLossTree->Branch("s",&s_el,"s/F"); // (m)
  EnergyLossTree->Branch("E",&E_el,"E/F"); // (GeV)

  PrimaryLossHisto = new TH1F("PlossHisto", "Primary Losses", nBins, 0., BDSGlobalConstants::Instance()->GetSMax()/CLHEP::m);
  PrimaryLossTree = new TTree("PlossTree", "Primary Losses");
  PrimaryLossTree->Branch("s",&s_pl,"s/F"); // (m)
  PrimaryLossTree->Branch("E",&E_pl,"s/F"); // (GeV)

  PrecisionRegionEnergyLossTree= new TTree("PrecisionRegionElossTree", "Energy Loss");//"x:y:z:s:E:partID:parentID:weight:volumeName");
  PrecisionRegionEnergyLossTree->Branch("x",&x_el_p,"x/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("y",&y_el_p,"y/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("z",&z_el_p,"z/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("s",&s_el_p,"s/F"); // (m)
  PrecisionRegionEnergyLossTree->Branch("E",&E_el_p,"E/F"); // (GeV)
  PrecisionRegionEnergyLossTree->Branch("weight",&weight_el_p,"weight/F");
  PrecisionRegionEnergyLossTree->Branch("partID",&part_el_p,"partID/I");
  PrecisionRegionEnergyLossTree->Branch("volumeName",&volumeName_el_p,"volumeName/C");
  PrecisionRegionEnergyLossTree->Branch("turnnumber",&turnnumber,"turnnumber/I");
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
				 G4int    TurnsTaken){

  TTree* sTree=(TTree*)gDirectory->Get(Name);
  if(!sTree) G4Exception("BDSOutputROOT: ROOT Sampler not found!", "-1", FatalException, "");
  E0=InitMom / CLHEP::GeV;
  x0=InitX / CLHEP::micrometer;
  y0=InitY / CLHEP::micrometer;
  z0=InitZ / CLHEP::m;
  xp0=InitXPrime / CLHEP::radian;
  yp0=InitYPrime / CLHEP::radian;
  zp0=InitZPrime / CLHEP::radian;
  t0=InitT / CLHEP::ns;
  E_prod=ProdMom / CLHEP::GeV;
  x_prod=ProdX / CLHEP::micrometer;
  y_prod=ProdY / CLHEP::micrometer;
  z_prod=ProdZ / CLHEP::m;
  xp_prod=ProdXPrime / CLHEP::radian;
  yp_prod=ProdYPrime / CLHEP::radian;
  zp_prod=ProdZPrime / CLHEP::radian;
  t_prod=ProdT / CLHEP::ns;
  E_lastScat=LastScatMom / CLHEP::GeV;
  x_lastScat=LastScatX / CLHEP::micrometer;
  y_lastScat=LastScatY / CLHEP::micrometer;
  z_lastScat=LastScatZ / CLHEP::m;
  xp_lastScat=LastScatXPrime / CLHEP::radian;
  yp_lastScat=LastScatYPrime / CLHEP::radian;
  zp_lastScat=LastScatZPrime / CLHEP::radian;
  t_lastScat=LastScatT / CLHEP::ns;

  E=Mom / CLHEP::GeV;
  //Edep=Edep / CLHEP::GeV;
  x=X / CLHEP::micrometer;
  y=Y / CLHEP::micrometer;
  z=Z / CLHEP::m;
  xp=XPrime / CLHEP::radian;
  yp=YPrime / CLHEP::radian;
  zp=ZPrime / CLHEP::radian;
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
				 G4int    TurnsTaken){
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
void BDSOutputROOT::WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec){
  //  G4int tID;
  G4TrajectoryPoint* TrajPoint;
  G4ThreeVector TrajPos;
  
  TTree* TrajTree;
  
  G4String name = "Trajectories";
  
  TrajTree=(TTree*)gDirectory->Get(name);
  
  if(TrajTree == NULL) { G4cerr<<"TrajTree=NULL"<<G4endl; return;}
  
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
	    if(!((std::abs(part==13))&&(BDSGlobalConstants::Instance()->GetStoreMuonTrajectories()))){ 
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

// make energy loss histo

void BDSOutputROOT::WriteEnergyLoss(BDSEnergyCounterHitsCollection* hc)
{
  G4int n_hit = hc->entries();
  for (G4int i=0;i<n_hit;i++)
    {
      //all regions fill the energy loss tree....
      E_el=(*hc)[i]->GetEnergy()/CLHEP::GeV;
      s_el=(*hc)[i]->GetEnergyWeightedS()*10*(1e-6)/(CLHEP::cm*E_el);
      EnergyLossHisto->Fill(s_el,E_el);
      EnergyLossTree->Fill();
      
      if((*hc)[i]->GetPrecisionRegion()){ //Only the precision region fills this tree, preserving every hit, its position and weight, instead of summing weighted energy in each beam line component.
	weight_el_p=(G4int)(*hc)[i]->GetWeight();
	E_el_p=((*hc)[i]->GetEnergy()/CLHEP::GeV)/weight_el_p;
	x_el_p=((*hc)[i]->GetEnergyWeightedX()/(CLHEP::cm*1e5*E_el_p))/weight_el_p;
	y_el_p=((*hc)[i]->GetEnergyWeightedY()*10/(CLHEP::cm*E_el_p))/weight_el_p;
	z_el_p=((*hc)[i]->GetEnergyWeightedZ()*10*(1e-6)/(CLHEP::cm*E_el_p))/weight_el_p;
	s_el_p=((*hc)[i]->GetEnergyWeightedS()*10*(1e-6)/(CLHEP::cm*E_el_p))/weight_el_p;
	part_el_p=(*hc)[i]->GetPartID();
	turnnumber=(*hc)[i]->GetTurnsTaken();
	G4String temp = (*hc)[i]->GetName()+'\0';
	strncpy(volumeName_el_p,temp.c_str(),sizeof(volumeName_el_p)-1);
	PrecisionRegionEnergyLossTree->Fill();
      }
    }
}

void BDSOutputROOT::WritePrimaryLoss(BDSEnergyCounterHit* hit)
{
  //all regions fill the energy loss tree....
  s_pl=hit->GetS()/CLHEP::m;
  E_pl=hit->GetEnergy()/CLHEP::GeV;

  PrimaryLossHisto->Fill(s_pl,E_pl);
  PrimaryLossTree->Fill();
}

void BDSOutputROOT::Commit()
{
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
      theRootOutputFile=NULL;
    }
  G4cout << __METHOD_NAME__ << " ...finished." << G4endl;
}
#endif
