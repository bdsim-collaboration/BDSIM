#include "BDSExecOptions.hh"
#include "BDSOutput.hh"
#include "BDSSamplerSD.hh"
#include <ctime>

// extern G4String outputFilename;

BDSOutput::BDSOutput():outputFileNumber(1)
{
  format = _ASCII; // default - write an ascii file
#ifdef USE_ROOT
  theRootOutputFile = NULL;
  EnergyLossHisto = NULL;
  PrecisionRegionEnergyLossTree = NULL;
  EnergyLossTree = NULL;
#endif
}

BDSOutput::BDSOutput(BDSOutputFormat fmt):format(fmt),outputFileNumber(1)
{
#ifdef USE_ROOT
  theRootOutputFile = NULL;
  EnergyLossHisto = NULL;
  PrecisionRegionEnergyLossTree = NULL;
  EnergyLossTree = NULL;
#endif
}

BDSOutput::~BDSOutput()
{
  if(format==_ASCII)
    of.close();
#ifdef USE_ROOT
  if(format==_ROOT){
    if (theRootOutputFile && theRootOutputFile->IsOpen()) {
      // theRootOutputFile->Write();
      theRootOutputFile->Close();
      delete theRootOutputFile;
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
      of<<"# PT E[GeV] X[mum] Y[mum] Z[m] Xp[rad] Yp[rad]  "<<G4endl;

    }
  if( format == _ROOT)
    {
      G4cout<<"output format ROOT"<<G4endl;
    }
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
  for(G4int i=0;i<BDSSampler::GetNSamplers();i++)
    {
      //G4String name="samp"+BDSGlobalConstants::Instance()->StringFromInt(i+1);
      G4String name=SampName[i];
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
  for(G4int i=0;i<BDSSamplerCylinder::GetNSamplers();i++)
    {
      //G4String name="samp"+BDSGlobalConstants::Instance()->StringFromInt(i+1);
      G4String name=CSampName[i];
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

void BDSOutput::WriteHits(BDSSamplerHitsCollection *hc)
{
  if( format == _ASCII) {
    
    //of<<"#hits (PDGtype  E[GeV],x[micron],y[micron],z[m],x'[rad],y'[rad]):"<<G4endl;
    int G4precision = G4cout.precision();
    G4cout.precision(15);
    
    for (G4int i=0; i<hc->entries(); i++)
      {
	of<<(*hc)[i]->GetPDGtype()
	  <<" "
	  <<(*hc)[i]->GetMom()/GeV
	  <<" "
	  <<(*hc)[i]->GetX()/micrometer
	  <<" "
	  <<(*hc)[i]->GetY()/micrometer
	  <<" "
	  <<(*hc)[i]->GetS() / m
	  <<" "
	  <<(*hc)[i]->GetXPrime() / radian
	  <<" "
	  <<(*hc)[i]->GetYPrime() / radian
	  <<G4endl;
      }
    
//    of<<G4endl; // remove whitespace in output file when no events hit sampler
      of.flush();
    //of<<"end of hits collection"<<G4endl;
      
      // set precision back
      G4cout.precision(G4precision);
  }
 
  if( format == _ROOT) {
#ifdef USE_ROOT
    G4String name;


    for (G4int i=0; i<hc->entries(); i++)
      {
	//if ((*hc)[i]->GetType()=="plane") 
	//name="samp";
	//else if ((*hc)[i]->GetType()=="cylinder")
	//name ="cyln";
	//name="samp" + BDSGlobalConstants::Instance()->StringFromInt((*hc)[i]->GetNumber());
	
	TTree* sTree=(TTree*)gDirectory->Get((*hc)[i]->GetName());
	
	if(!sTree) G4Exception("BDSOutput: ROOT Sampler not found!", "-1", FatalException, "");
	
	E0=(*hc)[i]->GetInitMom() / GeV;
	x0=(*hc)[i]->GetInitX() / micrometer;
	y0=(*hc)[i]->GetInitY() / micrometer;
	z0=(*hc)[i]->GetInitZ() / m;
	xp0=(*hc)[i]->GetInitXPrime() / radian;
	yp0=(*hc)[i]->GetInitYPrime() / radian;
	zp0=(*hc)[i]->GetInitZPrime() / radian;
	t0=(*hc)[i]->GetInitT() / ns;
	
	E=(*hc)[i]->GetMom() / GeV;
	//Edep=(*hc)[i]->GetEdep() / GeV;
	x=(*hc)[i]->GetX() / micrometer;
	y=(*hc)[i]->GetY() / micrometer;
	z=(*hc)[i]->GetZ() / m;
	xp=(*hc)[i]->GetXPrime() / radian;
	yp=(*hc)[i]->GetYPrime() / radian;
	zp=(*hc)[i]->GetZPrime() / radian;
	t=(*hc)[i]->GetT() / ns;
	
	X=(*hc)[i]->GetGlobalX() / m;
	Y=(*hc)[i]->GetGlobalY() / m;
	Z=(*hc)[i]->GetGlobalZ() / m;
	Xp=(*hc)[i]->GetGlobalXPrime() / radian;
	Yp=(*hc)[i]->GetGlobalYPrime() / radian;
	Zp=(*hc)[i]->GetGlobalZPrime() / radian;
	
	s=(*hc)[i]->GetS() / m;
	
	weight=(*hc)[i]->GetWeight();
	part=(*hc)[i]->GetPDGtype(); 
	nev=(*hc)[i]->GetEventNo(); 
	pID=(*hc)[i]->GetParentID(); 
	track_id=(*hc)[i]->GetTrackID();
	
#ifdef DEBUG
	if(track_id==1){
	  G4cout << "BDSOutput::WriteHits> primary particles:" << (*hc)[i]->GetName() << " " << nev << G4endl; 
	}
#endif
	sTree->Fill();
      }
#endif
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
    
    of<<"#Energy loss: Z[m] E[GeV] partID weight"<<G4endl;
   
    for (G4int i=0;i<n_hit;i++)
      {
        G4double Energy=(*hc)[i]->GetEnergy();
	G4double Zpos=(*hc)[i]->GetZ();
	G4int partID = (*hc)[i]->GetPartID();
	G4double weight = (*hc)[i]->GetWeight();

	of<< Zpos/m<<"  "<<Energy/GeV<<"  "<<partID<<"  " <<weight<<G4endl;

      }
      of.flush();
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
	//Dump all other quantities to file...
	theRootOutputFile->Write();
	theRootOutputFile->Close();
	delete theRootOutputFile;
      }
  }
#endif
}
