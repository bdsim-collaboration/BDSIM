#ifdef USE_ROOT
#include "BDSOutputROOT.hh"

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSSampler.hh"
#include "BDSSamplerCylinder.hh"

BDSOutputROOT::BDSOutputROOT():BDSOutputBase()
{
  G4cout<<"output format ROOT"<<G4endl;
  theRootOutputFile = NULL;
  EnergyLossHisto = NULL;
  PrecisionRegionEnergyLossTree = NULL;
  EnergyLossTree = NULL;
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
  
  SamplerTree->Branch("E0",&E0,"E0 (GeV)/F");
  SamplerTree->Branch("x0",&x0,"x0 (mum)/F");
  SamplerTree->Branch("y0",&y0,"y0 (mum)/F");
  SamplerTree->Branch("z0",&z0,"z0 (m)/F");
  SamplerTree->Branch("xp0",&xp0,"xp0 (rad)/F");
  SamplerTree->Branch("yp0",&yp0,"yp0 (rad)/F");
  SamplerTree->Branch("zp0",&zp0,"zp0 (rad)/F");
  SamplerTree->Branch("t0",&t0,"t0 (ns)/F");
  
  SamplerTree->Branch("E",&E,"E (GeV)/F");
  SamplerTree->Branch("x",&x,"x (mum)/F");
  SamplerTree->Branch("y",&y,"y (mum)/F");
  SamplerTree->Branch("z",&z,"z (m)/F");
  SamplerTree->Branch("xp",&xp,"xp (rad)/F");
  SamplerTree->Branch("yp",&yp,"yp (rad)/F");
  SamplerTree->Branch("zp",&zp,"zp (rad)/F");
  SamplerTree->Branch("t",&t,"t (ns)/F");
  
  SamplerTree->Branch("X",&X,"X (mum)/F");
  SamplerTree->Branch("Y",&Y,"Y (mum)/F");
  SamplerTree->Branch("Z",&Z,"Z (m)/F");
  SamplerTree->Branch("Xp",&Xp,"Xp (rad)/F");
  SamplerTree->Branch("Yp",&Yp,"Yp (rad)/F");
  SamplerTree->Branch("Zp",&Zp,"Zp (rad)/F");
  
  SamplerTree->Branch("s",&s,"s (m)/F");
  
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
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << " building sampler tree named: " << primariesSamplerName << G4endl;
#endif
  BuildSamplerTree(primariesSamplerName);
  for(G4int i=0;i<BDSSampler::GetNSamplers();i++)
    {
#ifdef DEBUG
      G4cout << __METHOD_NAME__ << " building sampler tree number: " << i << G4endl;
#endif
      G4String name=BDSSampler::outputNames[i];
#ifdef DEBUG
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
      TrajTree->Branch("x",&x,"x (mum)/F");
      TrajTree->Branch("y",&y,"y (mum)/F");
      TrajTree->Branch("z",&z,"z (m)/F");
      TrajTree->Branch("part",&part,"part/I");
    }

  // build energy loss histogram
  G4int nBins = G4int(BDSGlobalConstants::Instance()->GetZMax()/(BDSGlobalConstants::Instance()->GetElossHistoBinWidth()*CLHEP::m));
  G4double wx=(BDSGlobalConstants::Instance()->GetTunnelRadius()+BDSGlobalConstants::Instance()->GetTunnelOffsetX())*2;
  G4double wy=(BDSGlobalConstants::Instance()->GetTunnelRadius()+BDSGlobalConstants::Instance()->GetTunnelOffsetY())*2;
  G4double bs=BDSGlobalConstants::Instance()->GetComponentBoxSize();
  G4double wmax=std::max(wx,wy);
  wmax=std::max(wmax,bs);

  EnergyLossHisto = new TH1F("ElossHisto", "Energy Loss",nBins,0.,BDSGlobalConstants::Instance()->GetZMax()/CLHEP::m);
  EnergyLossTree= new TTree("ElossTree", "Energy Loss");
  EnergyLossTree->Branch("z",&z_el,"z (m)/F");
  EnergyLossTree->Branch("E",&E_el,"E (GeV)/F");

  PrecisionRegionEnergyLossTree= new TTree("PrecisionRegionElossTree", "Energy Loss");//"x:y:z:E:partID:parentID:weight:volumeName");
  PrecisionRegionEnergyLossTree->Branch("x",&x_el_p,"x (m)/F");
  PrecisionRegionEnergyLossTree->Branch("y",&y_el_p,"y (m)/F");
  PrecisionRegionEnergyLossTree->Branch("z",&z_el_p,"z (m)/F");
  PrecisionRegionEnergyLossTree->Branch("E",&E_el_p,"E (GeV)/F");
  PrecisionRegionEnergyLossTree->Branch("weight",&weight_el_p,"weight/F");
  PrecisionRegionEnergyLossTree->Branch("partID",&part_el_p,"partID/I");
  PrecisionRegionEnergyLossTree->Branch("volumeName",&volumeName_el_p,"volumeName/C");
  PrecisionRegionEnergyLossTree->Branch("turnnumber",&turnnumber,"turnnumber/I");
}

void BDSOutputROOT::WriteRootHit(G4String Name, G4double InitMom, G4double InitX, G4double InitY, G4double InitZ, G4double InitXPrime, G4double InitYPrime, G4double InitZPrime, G4double  InitT, G4double  Mom, G4double X, G4double Y, G4double Z, G4double XPrime, G4double YPrime, G4double ZPrime, G4double T, G4double GlobalX, G4double GlobalY, G4double GlobalZ, G4double GlobalXPrime, G4double GlobalYPrime, G4double GlobalZPrime, G4double S, G4double Weight, G4int  PDGtype, G4int  EventNo, G4int ParentID,G4int TrackID, G4int TurnsTaken){

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

void BDSOutputROOT::WritePrimary(G4String samplerName, G4double E,G4double x0,G4double y0,G4double z0,G4double xp,G4double yp,G4double zp,G4double t,G4double weight,G4int PDGType, G4int nEvent, G4int TurnsTaken){
  WriteRootHit(samplerName, E, x0, y0, z0, xp, yp, zp, t, E, x0, y0, z0, xp, yp, zp, t, x, y, z, xp, yp, zp, z, weight, PDGType, nEvent, 0, 1, TurnsTaken);
}

void BDSOutputROOT::WriteHits(BDSSamplerHitsCollection *hc)
{
  G4String name;
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << " hc->endtries() = " << hc->entries() << G4endl;
#endif
  for (G4int i=0; i<hc->entries(); i++)
    {
      G4String name = (*hc)[i]->GetName();
#ifdef DEBUG
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
void BDSOutputROOT::WriteTrajectory(std::vector<G4VTrajectory*> &TrajVec){
  //  G4int tID;
  G4TrajectoryPoint* TrajPoint;
  G4ThreeVector TrajPos;
  
  TTree* TrajTree;
    
  G4String name = "Trajectories";
  
  TrajTree=(TTree*)gDirectory->Get(name);

  if(TrajTree == NULL) { G4cerr<<"TrajTree=NULL"<<G4endl; return;}
  
  if(TrajVec.size())
    {
      std::vector<G4VTrajectory*>::iterator iT;
      
      for(iT=TrajVec.begin();iT<TrajVec.end();iT++)
	{
	  G4Trajectory* Traj=(G4Trajectory*)(*iT);
	  
	  //	  tID=Traj->GetTrackID();	      
	  part = Traj->GetPDGEncoding();
	  
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

// make energy loss histo

void BDSOutputROOT::WriteEnergyLoss(BDSEnergyCounterHitsCollection* hc)
{
  G4int n_hit = hc->entries();
  for (G4int i=0;i<n_hit;i++)
    {
      //all regions fill the energy loss tree....
      E_el=(*hc)[i]->GetEnergy()/CLHEP::GeV;
      z_el=(*hc)[i]->GetEnergyWeightedZ()*10*(1e-6)/(CLHEP::cm*E_el);
      EnergyLossHisto->Fill(z_el,E_el);
      EnergyLossTree->Fill();
      
      if((*hc)[i]->GetPrecisionRegion()){ //Only the precision region fills this tree, preserving every hit, its position and weight, instead of summing weighted energy in each beam line component.
	weight_el_p=(G4int)(*hc)[i]->GetWeight();
	E_el_p=((*hc)[i]->GetEnergy()/CLHEP::GeV)/weight_el_p;
	x_el_p=((*hc)[i]->GetEnergyWeightedX()/(CLHEP::cm*1e5*E_el_p))/weight_el_p;
	y_el_p=((*hc)[i]->GetEnergyWeightedY()*10/(CLHEP::cm*E_el_p))/weight_el_p;
	z_el_p=((*hc)[i]->GetEnergyWeightedZ()*10*(1e-6)/(CLHEP::cm*E_el_p))/weight_el_p;
	part_el_p=(*hc)[i]->GetPartID();
	turnnumber=(*hc)[i]->GetTurnsTaken();
	G4String temp = (*hc)[i]->GetName()+'\0';
	strncpy(volumeName_el_p,temp.c_str(),sizeof(volumeName_el_p)-1);
	PrecisionRegionEnergyLossTree->Fill();
      }
    }
}

void BDSOutputROOT::Commit()
{
  Write();
  outputFileNumber++;
  Init();
}

void BDSOutputROOT::Write()
{
  if(theRootOutputFile && theRootOutputFile->IsOpen())
    {
#ifdef DEBUG
      G4cout << __METHOD_NAME__ << " writing to root file..." << G4endl;
#endif
      //Dump all other quantities to file...
      theRootOutputFile->Write();
      theRootOutputFile->Close();
      delete theRootOutputFile;
      theRootOutputFile=NULL;
    }
}
#endif
