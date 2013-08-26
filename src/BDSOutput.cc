#include "BDSExecOptions.hh"
#include "BDSOutput.hh"
#include "BDSSamplerSD.hh"
#include <ctime>

// extern G4String outputFilename;

BDSOutput::BDSOutput():outputFileNumber(1)
{
  format = _ASCII; // default - write an ascii file
#ifdef USE_ROOT
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << " - USE_ROOT is defined." << G4endl;
#endif
  theRootOutputFile = NULL;
  EnergyLossHisto = NULL;
  PrecisionRegionEnergyLossTree = NULL;
  EnergyLossTree = NULL;
#endif
}

BDSOutput::BDSOutput(BDSOutputFormat fmt):format(fmt),outputFileNumber(1)
{
#ifdef USE_ROOT
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << " - USE_ROOT is defined." << G4endl;
#endif
  theRootOutputFile = NULL;
  EnergyLossHisto = NULL;
  PrecisionRegionEnergyLossTree = NULL;
  EnergyLossTree = NULL;
#endif
}

BDSOutput::~BDSOutput()
{
  if(format==_ASCII){
    of.close();
    ofEloss.close();
  }
#ifdef USE_ROOT
  if(format==_ROOT){
    if (theRootOutputFile && theRootOutputFile->IsOpen()) {
      theRootOutputFile->Write();
      //      theRootOutputFile->Close();
      //      delete theRootOutputFile;
    }
  }
#endif
}

void BDSOutput::SetFormat(BDSOutputFormat val)
{
  format = val;
  time_t tm = time(NULL);

  if( format == _ASCII)
    {
      G4String filename = BDSExecOptions::Instance()->GetOutputFilename()+".txt";
      G4cout << __METHOD_NAME__ << "Output format ASCII, filename: "<<filename<<G4endl;
      of.open(filename);
      of<<"### BDSIM output created "<<ctime(&tm)<<" ####"<<G4endl;
      of<<"# PT E[GeV] X[mum] Y[mum] Z[m] Xp[rad] Yp[rad]  NEvent Weight ParentID TrackID"<<G4endl;
      G4String filenameEloss = BDSExecOptions::Instance()->GetOutputFilename()+".eloss.txt";
      G4cout << __METHOD_NAME__ << "Eloss output format ASCII, filename: "<<filenameEloss<<G4endl;
      ofEloss.open(filenameEloss);
      ofEloss<<"### BDSIM eloss output created "<<ctime(&tm)<<" ####"<<G4endl;
      ofEloss<<"#Energy loss: Z[m] E[GeV] partID weight"<<G4endl;

    }
  if( format == _ROOT)
    {
      G4cout<<"output format ROOT"<<G4endl;
    }
}

void BDSOutput::BuildSamplerTree(G4String name){
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
}

// output initialization (ROOT only)
void BDSOutput::Init(G4int FileNum)
{
#ifdef USE_ROOT
  if(format != _ROOT) return;

  // set up the root file
  G4String _filename = BDSExecOptions::Instance()->GetOutputFilename() + "_" 
    + BDSGlobalConstants::Instance()->StringFromInt(FileNum) + ".root";
  
  G4cout<<"Setting up new file: "<<_filename<<G4endl;
  theRootOutputFile=new TFile(_filename,"RECREATE", "BDS output file");

  //build sampler tree
  G4String primariesSamplerName="primaries";
  G4cout << __METHOD_NAME__ << " building sampler tree named: " << primariesSamplerName << G4endl;
  BuildSamplerTree(primariesSamplerName);
  for(G4int i=0;i<BDSSampler::GetNSamplers();i++)
    {
      G4cout << __METHOD_NAME__ << " building sampler tree number: " << i << G4endl;
      //G4String name="samp"+BDSGlobalConstants::Instance()->StringFromInt(i+1);
      G4String name=SampName[i];
      G4cout << __METHOD_NAME__ << " named: " << name << G4endl;
      BuildSamplerTree(name);
    }
  for(G4int i=0;i<BDSSamplerCylinder::GetNSamplers();i++)
    {
      //G4String name="samp"+BDSGlobalConstants::Instance()->StringFromInt(i+1);
      G4String name=CSampName[i];
      BuildSamplerTree(name);
    }

  if(BDSGlobalConstants::Instance()->GetStoreTrajectory() || BDSGlobalConstants::Instance()->GetStoreMuonTrajectories() || BDSGlobalConstants::Instance()->GetStoreNeutronTrajectories()) 
    // create a tree with trajectories
    {
      //G4cout<<"BDSOutput::storing trajectories set"<<G4endl;
      TTree* TrajTree = new TTree("Trajectories", "Trajectories");
      TrajTree->Branch("x",&x,"x (mum)/F");
      TrajTree->Branch("y",&y,"y (mum)/F");
      TrajTree->Branch("z",&z,"z (m)/F");
      TrajTree->Branch("part",&part,"part/I");
    }

  // build energy loss histogram
  G4int nBins = G4int(zMax/(BDSGlobalConstants::Instance()->GetElossHistoBinWidth()*m));
  G4double wx=(BDSGlobalConstants::Instance()->GetTunnelRadius()+BDSGlobalConstants::Instance()->GetTunnelOffsetX())*2;
  G4double wy=(BDSGlobalConstants::Instance()->GetTunnelRadius()+BDSGlobalConstants::Instance()->GetTunnelOffsetY())*2;
  G4double bs=BDSGlobalConstants::Instance()->GetComponentBoxSize();
  G4double wmax=std::max(wx,wy);
  wmax=std::max(wmax,bs);

  EnergyLossHisto = new TH1F("ElossHisto", "Energy Loss",nBins,0.,zMax/m);
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
#endif // USE_ROOT
}

void BDSOutput::WriteAsciiHit(G4int PDGType, G4double Mom, G4double X, G4double Y, G4double S, G4double XPrime, G4double YPrime, G4int EventNo, G4double Weight, G4int ParentID, G4int TrackID){
  of<<PDGType
    <<" "
    <<Mom/GeV
    <<" "
    <<X/micrometer
    <<" "
    <<Y/micrometer
    <<" "
    <<S / m
    <<" "
    <<XPrime / radian
    <<" "
    <<YPrime / radian
    <<" "
    <<EventNo 
    <<" "
    <<Weight
    <<" "
    <<ParentID
    <<" "
    <<TrackID
    <<G4endl;
}

void BDSOutput::WriteRootHit(G4String Name, G4double   InitMom, G4double    InitX, G4double    InitY, G4double     InitZ, G4double     InitXPrime, G4double    InitYPrime, G4double InitZPrime, G4double  InitT, G4double  Mom, G4double X, G4double Y, G4double Z, G4double XPrime, G4double YPrime, G4double ZPrime, G4double T, G4double GlobalX, G4double GlobalY, G4double GlobalZ, G4double GlobalXPrime, G4double GlobalYPrime, G4double GlobalZPrime, G4double S, G4double Weight, G4int  PDGtype, G4int  EventNo, G4int   ParentID,G4int  TrackID){

  TTree* sTree=(TTree*)gDirectory->Get(Name);
  if(!sTree) G4Exception("BDSOutput: ROOT Sampler not found!", "-1", FatalException, "");
  E0=InitMom / GeV;
  x0=InitX / micrometer;
  y0=InitY / micrometer;
  z0=InitZ / m;
  xp0=InitXPrime / radian;
  yp0=InitYPrime / radian;
  zp0=InitZPrime / radian;
  t0=InitT / ns;
  E=Mom / GeV;
  //Edep=Edep / GeV;
  x=X / micrometer;
  y=Y / micrometer;
  z=Z / m;
  xp=XPrime / radian;
  yp=YPrime / radian;
  zp=ZPrime / radian;
  t=T / ns;
  X=GlobalX / m;
  Y=GlobalY / m;
  Z=GlobalZ / m;
  Xp=GlobalXPrime / radian;
  Yp=GlobalYPrime / radian;
  Zp=GlobalZPrime / radian;
  s=S / m;
  weight=Weight;
  part=PDGtype; 
  nev=EventNo; 
  pID=ParentID; 
  track_id=TrackID;
  sTree->Fill();
}

void BDSOutput::WritePrimary(G4String samplerName, G4double E,G4double x0,G4double y0,G4double z0,G4double xp,G4double yp,G4double zp,G4double t,G4double weight,G4int PDGType, G4int nEvent){
#ifdef USE_ROOT
  if( format == _ROOT) {
    bdsOutput->WriteRootHit(samplerName, E, x0, y0, z0, xp, yp, zp, t, E, x0, y0, z0, xp, yp, zp, t, x, y, z, xp, yp, zp, z, weight, PDGType, nEvent, 0, 1);
  }
#endif
  
  if( format == _ASCII) {
    bdsOutput->WriteAsciiHit(PDGType, E, x0, y0, z0, xp, yp, nEvent, weight, 0, 1);
  }
}

void BDSOutput::WriteHits(BDSSamplerHitsCollection *hc)
{
  if( format == _ASCII) {
    int G4precision = G4cout.precision();
    G4cout.precision(15);
    
    for (G4int i=0; i<hc->entries(); i++)
      {
	WriteAsciiHit(
		      (*hc)[i]->GetPDGtype(),
		      (*hc)[i]->GetMom(),
		      (*hc)[i]->GetX(),
		      (*hc)[i]->GetY(),
		      (*hc)[i]->GetS(),
		      (*hc)[i]->GetXPrime(),
		      (*hc)[i]->GetYPrime(),
		      (*hc)[i]->GetEventNo(),
		      (*hc)[i]->GetWeight(),
		      (*hc)[i]->GetParentID(),
		      (*hc)[i]->GetTrackID()
		      );
      }
    of.flush();
    // set precision back
    G4cout.precision(G4precision);
  }
  
  if( format == _ROOT) {
#ifdef USE_ROOT
    G4String name;
    G4cout << __METHOD_NAME__ << " hc->endtries() = " << hc->entries() << G4endl;
    for (G4int i=0; i<hc->entries(); i++)
      {
	G4String name = (*hc)[i]->GetName();
	G4cout << "Writing hit to sampler " << name << G4endl;
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
		     (*hc)[i]->GetTrackID()
		     );
      }
#endif //USE_ROOT
  }
}

G4int BDSOutput::WriteTrajectory(std::vector<G4VTrajectory*> TrajVec){
  //  G4cout<<"a trajectory stored..."<<G4endl;
  
  //  G4int tID;
  G4TrajectoryPoint* TrajPoint;
  G4ThreeVector TrajPos;
  
#ifdef USE_ROOT
  TTree* TrajTree;
    
  G4String name = "Trajectories";
  
  TrajTree=(TTree*)gDirectory->Get(name);

  if(TrajTree == NULL) { G4cerr<<"TrajTree=NULL"<<G4endl; return -1;}
#endif
  
  
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
	      
	      x = TrajPos.x() / m;
	      y = TrajPos.y() / m;
	      z = TrajPos.z() / m;
              
#ifdef USE_ROOT
	      if( format == _ROOT) 
		TrajTree->Fill();
#endif
              
	      //G4cout<<"TrajPos="<<TrajPos<<G4endl;
	    }
	}
    }
  
  return 0;
}

// write a trajectory to a root/ascii file
G4int BDSOutput::WriteTrajectory(TrajectoryVector* TrajVec)
{
//  G4cout<<"a trajectory stored..."<<G4endl;
  
//  G4int tID;
  G4TrajectoryPoint* TrajPoint;
  G4ThreeVector TrajPos;

#ifdef USE_ROOT
  TTree* TrajTree;
    
  G4String name = "Trajectories";
      
  TrajTree=(TTree*)gDirectory->Get(name);

  if(TrajTree == NULL) { G4cerr<<"TrajTree=NULL"<<G4endl; return -1;}
#endif


  if(TrajVec)
    {
      TrajectoryVector::iterator iT;
      
      for(iT=TrajVec->begin();iT<TrajVec->end();iT++)
	{
	  G4Trajectory* Traj=(G4Trajectory*)(*iT);
	  
	  //	  tID=Traj->GetTrackID();	      
	  part = Traj->GetPDGEncoding();
	  
	  for(G4int j=0; j<Traj->GetPointEntries(); j++)
	    {
	      TrajPoint=(G4TrajectoryPoint*)Traj->GetPoint(j);
	      TrajPos=TrajPoint->GetPosition();
	      
	      x = TrajPos.x() / m;
	      y = TrajPos.y() / m;
	      z = TrajPos.z() / m;

#ifdef USE_ROOT
	      if( format == _ROOT) 
		TrajTree->Fill();
#endif

	      //G4cout<<"TrajPos="<<TrajPos<<G4endl;
	    }
	}
    }
  
  return 0;
}

// make energy loss histo

void BDSOutput::WriteEnergyLoss(BDSEnergyCounterHitsCollection* hc)
{
  if( format == _ROOT) {
#ifdef USE_ROOT
    G4int n_hit = hc->entries();
    for (G4int i=0;i<n_hit;i++)
      {
	//all regions fill the energy loss tree....
        E_el=(*hc)[i]->GetEnergy()/GeV;
	z_el=(*hc)[i]->GetEnergyWeightedZ()*10*(1e-6)/(cm*E_el);
	EnergyLossHisto->Fill(z_el,E_el);
	EnergyLossTree->Fill();

	if((*hc)[i]->GetPrecisionRegion()){ //Only the precision region fills this tree, preserving every hit, its position and weight, instead of summing weighted energy in each beam line component.
	  weight_el_p=(*hc)[i]->GetWeight();
	  E_el_p=((*hc)[i]->GetEnergy()/GeV)/weight_el_p;
	  x_el_p=((*hc)[i]->GetEnergyWeightedX()/(cm*1e5*E_el_p))/weight_el_p;
	  y_el_p=((*hc)[i]->GetEnergyWeightedY()*10/(cm*E_el_p))/weight_el_p;
	  z_el_p=((*hc)[i]->GetEnergyWeightedZ()*10*(1e-6)/(cm*E_el_p))/weight_el_p;
	  part_el_p=(*hc)[i]->GetPartID();
	  G4String temp = (*hc)[i]->GetName()+'\0';
	  strcpy(volumeName_el_p,temp.c_str());
	  PrecisionRegionEnergyLossTree->Fill();
	}
	
      }
#endif
  }

 if( format == _ASCII) {
    G4int n_hit = hc->entries();

    for (G4int i=0;i<n_hit;i++)
      {
        G4double Energy=(*hc)[i]->GetEnergy();
	G4double Zpos=(*hc)[i]->GetZ();
	G4int partID = (*hc)[i]->GetPartID();
	G4double weight = (*hc)[i]->GetWeight();

	ofEloss<< Zpos/m<<"  "<<Energy/GeV<<"  "<<partID<<"  " <<weight<<G4endl;
      }
    ofEloss.flush();
 }
}

// write some comments to the output file
// only for ASCII output
void BDSOutput::Echo(G4String str)
{
  if(format == _ASCII)  of<<"#"<<str<<G4endl;
  else // default
    G4cout<<"#"<<str<<G4endl;
}

//G4int BDSOutput::Commit(G4int FileNum)
G4int BDSOutput::Commit()
{
#ifdef USE_ROOT
  Write();
  Init(outputFileNumber++);
#endif
  return 0;
}

void BDSOutput::Write()
{
#ifdef USE_ROOT
  if(format == _ROOT){
    if(theRootOutputFile->IsOpen())
      {
	G4cout << __METHOD_NAME__ << " writing to root file..." << G4endl;
	//Dump all other quantities to file...
	theRootOutputFile->Write();
	//	G4cout << __METHOD_NAME__ << " closing root file..." << G4endl;
	theRootOutputFile->Close();
	//	G4cout << __METHOD_NAME__ << " deleting root file..." << G4endl;
	delete theRootOutputFile;
	//	G4cout << __METHOD_NAME__ << " setting root file to NULL..." << G4endl;
	theRootOutputFile=NULL;
      }
  }
#endif
}
