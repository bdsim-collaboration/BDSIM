{
  
  TFile tempfile("output_0.root");
  Int_t nsamps = tempfile.GetNkeys() - 2; // allow for h1, h1tuple
  tempfile.Close();
  Float_t r,x,xp,y,yp,z,z0,e,e0;
  Int_t part,nev;
  
  Int_t jstar=1;
  Int_t jend=nsamps;
  cout << "Number of Samplers: " << nsamps << endl;
  Float_t factor;
  Char_t ChainName[256];
  TNtuple *ntuple = new TNtuple("ntuple","ntuple","r:x:xp:y:yp:z:z0:E:E0:part:nev:n");
  
  for (Int_t j=jstar; j <=jend ; j++)
    {
      // Name of the Samplers to be opened
      sprintf(ChainName,"sampler_phys_%d",j);
      TChain PChain(ChainName);
      for( int k = 0; k<23; k++)
	{
	  char filename[200];
	  sprintf(filename,"output_%d.root",k);
	  cout << "Opening File: " << filename << endl;
	  PChain.Add(filename);	
	}
      PChain.SetBranchAddress("x", &x);
      PChain.SetBranchAddress("xp", &xp);
      PChain.SetBranchAddress("y", &y);
      PChain.SetBranchAddress("yp", &yp);
      PChain.SetBranchAddress("z", &z);
      PChain.SetBranchAddress("z0", &z0);
      PChain.SetBranchAddress("E", &e);
      PChain.SetBranchAddress("E0", &e0);
      PChain.SetBranchAddress("part", &part);
      PChain.SetBranchAddress("nev", &nev);
      
      Int_t mmax =  (Int_t)PChain.GetEntries();
      for (Int_t m=0; m<mmax;m++)
	{
	  PChain.GetEntry(m);
	  r=TMath::Sqrt(x*x+y*y);
	  // convert to m and radians
	  x*=1e-6;
	  y*=1e-6;
	  xp*=1e-6;
	  yp*=1e-6;

	  ntuple->Fill(r,x,xp,y,yp,z,z0,e,e0,part,nev,j);
	  
	}
      
    }
  
}
