//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Aug  5 16:15:01 2014 by ROOT version 5.34/18
// from TTree primaries/Sampler output
// found on file: ../../../bdsim-mac/test/BDSTrackingTestFiles/output_0.root
//////////////////////////////////////////////////////////

#ifndef Sampler_h
#define Sampler_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include <cstring>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class Sampler {
public :
   TTree          *fChainIn;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   TTree          *fChainOut;

   // Declaration of leaf types
   Float_t         E0;
   Float_t         x0;
   Float_t         y0;
   Float_t         z0;
   Float_t         xp0;
   Float_t         yp0;
   Float_t         zp0;
   Float_t         t0;
   Float_t         E;
   Float_t         x;
   Float_t         y;
   Float_t         z;
   Float_t         xp;
   Float_t         yp;
   Float_t         zp;
   Float_t         t;
   Float_t         X;
   Float_t         Y;
   Float_t         Z;
   Float_t         Xp;
   Float_t         Yp;
   Float_t         Zp;
   Float_t         s;
   Float_t         weight;
   Int_t           partID;
   Int_t           nEvent;
   Int_t           parentID;
   Int_t           trackID;
   Int_t           turnnumber;

   // List of branches
   TBranch        *b_E0 ; //(GeV);   //!
   TBranch        *b_x0 ; //(mum);   //!
   TBranch        *b_y0 ; //(mum);   //!
   TBranch        *b_z0 ; //(m);   //!
   TBranch        *b_xp0 ; //(rad);   //!
   TBranch        *b_yp0 ; //(rad);   //!
   TBranch        *b_zp0 ; //(rad);   //!
   TBranch        *b_t0 ; //(ns);   //!
   TBranch        *b_E ; //(GeV);   //!
   TBranch        *b_x ; //(mum);   //!
   TBranch        *b_y ; //(mum);   //!
   TBranch        *b_z ; //(m);   //!
   TBranch        *b_xp ; //(rad);   //!
   TBranch        *b_yp ; //(rad);   //!
   TBranch        *b_zp ; //(rad);   //!
   TBranch        *b_t ; //(ns);   //!
   TBranch        *b_X ; //(mum);   //!
   TBranch        *b_Y ; //(mum);   //!
   TBranch        *b_Z ; //(m);   //!
   TBranch        *b_Xp ; //(rad);   //!
   TBranch        *b_Yp ; //(rad);   //!
   TBranch        *b_Zp ; //(rad);   //!
   TBranch        *b_s ; //(m);   //!
   TBranch        *b_weight;   //!
   TBranch        *b_partID;   //!
   TBranch        *b_nEvent;   //!
   TBranch        *b_parentID;   //!
   TBranch        *b_trackID;   //!
   TBranch        *b_turnnumber;   //!

   // List of branches
   TString        sSamplerName;
   TBranch        *bOut_SamplerName; 
   TBranch        *bOut_E0 ; //(GeV);   //!
   TBranch        *bOut_x0 ; //(mum);   //!
   TBranch        *bOut_y0 ; //(mum);   //!
   TBranch        *bOut_z0 ; //(m);   //!
   TBranch        *bOut_xp0 ; //(rad);   //!
   TBranch        *bOut_yp0 ; //(rad);   //!
   TBranch        *bOut_zp0 ; //(rad);   //!
   TBranch        *bOut_t0 ; //(ns);   //!
   TBranch        *bOut_E ; //(GeV);   //!
   TBranch        *bOut_x ; //(mum);   //!
   TBranch        *bOut_y ; //(mum);   //!
   TBranch        *bOut_z ; //(m);   //!
   TBranch        *bOut_xp ; //(rad);   //!
   TBranch        *bOut_yp ; //(rad);   //!
   TBranch        *bOut_zp ; //(rad);   //!
   TBranch        *bOut_t ; //(ns);   //!
   TBranch        *bOut_X ; //(mum);   //!
   TBranch        *bOut_Y ; //(mum);   //!
   TBranch        *bOut_Z ; //(m);   //!
   TBranch        *bOut_Xp ; //(rad);   //!
   TBranch        *bOut_Yp ; //(rad);   //!
   TBranch        *bOut_Zp ; //(rad);   //!
   TBranch        *bOut_s ; //(m);   //!
   TBranch        *bOut_weight;   //!
   TBranch        *bOut_partID;   //!
   TBranch        *bOut_nEvent;   //!
   TBranch        *bOut_parentID;   //!
   TBranch        *bOut_trackID;   //!
   TBranch        *bOut_turnnumber;   //!

   Sampler(TTree *tree=0);
   virtual ~Sampler();
   //   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   virtual void     InitOut(TTree *, int iSampler, TString sSamplerName);
};

#endif

#ifdef Sampler_cxx
Sampler::Sampler(TTree *tree) : fChainIn(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("../../../bdsim-mac/test/BDSTrackingTestFiles/output_0.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("../../../bdsim-mac/test/BDSTrackingTestFiles/output_0.root");
      }
      f->GetObject("primaries",tree);

   }
   Init(tree);
}

Sampler::~Sampler()
{
   if (!fChainIn) return;
   delete fChainIn->GetCurrentFile();
}

Int_t Sampler::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChainIn) return 0;
   return fChainIn->GetEntry(entry);
}
Long64_t Sampler::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChainIn) return -5;
   Long64_t centry = fChainIn->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChainIn->GetTreeNumber() != fCurrent) {
      fCurrent = fChainIn->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Sampler::Init(TTree *tree){
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChainIn = tree;
   fCurrent = -1;
   fChainIn->SetMakeClass(1);

   fChainIn->SetBranchAddress("E0", &E0, &b_E0);
   fChainIn->SetBranchAddress("x0", &x0, &b_x0);
   fChainIn->SetBranchAddress("y0", &y0, &b_y0);
   fChainIn->SetBranchAddress("z0", &z0, &b_z0);
   fChainIn->SetBranchAddress("xp0", &xp0, &b_xp0);
   fChainIn->SetBranchAddress("yp0", &yp0, &b_yp0);
   fChainIn->SetBranchAddress("zp0", &zp0, &b_zp0);
   fChainIn->SetBranchAddress("t0", &t0, &b_t0);
   fChainIn->SetBranchAddress("E", &E, &b_E);
   fChainIn->SetBranchAddress("x", &x, &b_x);
   fChainIn->SetBranchAddress("y", &y, &b_y);
   fChainIn->SetBranchAddress("z", &z, &b_z);
   fChainIn->SetBranchAddress("xp", &xp, &b_xp);
   fChainIn->SetBranchAddress("yp", &yp, &b_yp);
   fChainIn->SetBranchAddress("zp", &zp, &b_zp);
   fChainIn->SetBranchAddress("t", &t, &b_t);
   fChainIn->SetBranchAddress("X", &X, &b_X);
   fChainIn->SetBranchAddress("Y", &Y, &b_Y);
   fChainIn->SetBranchAddress("Z", &Z, &b_Z);
   fChainIn->SetBranchAddress("Xp", &Xp, &b_Xp);
   fChainIn->SetBranchAddress("Yp", &Yp, &b_Yp);
   fChainIn->SetBranchAddress("Zp", &Zp, &b_Zp);
   fChainIn->SetBranchAddress("s", &s, &b_s);
   fChainIn->SetBranchAddress("weight", &weight, &b_weight);
   fChainIn->SetBranchAddress("partID", &partID, &b_partID);
   fChainIn->SetBranchAddress("nEvent", &nEvent, &b_nEvent);
   fChainIn->SetBranchAddress("parentID", &parentID, &b_parentID);
   fChainIn->SetBranchAddress("trackID", &trackID, &b_trackID);
   fChainIn->SetBranchAddress("turnnumber", &turnnumber, &b_turnnumber);
   Notify();
}

void Sampler::InitOut(TTree *tree, int iSampler, TString sSamplerNameIn) {
  sSamplerName = sSamplerNameIn;

  fChainOut = tree;
  char cSampler[10];
  sprintf(cSampler,"%i",iSampler);
  TString sSampler = TString(cSampler); 
    
  bOut_SamplerName = fChainOut->Branch(sSampler.Copy().Append("_name"),&sSamplerName,sSampler.Copy().Append("_name/S"));
  bOut_E0  = fChainOut->Branch(sSampler.Copy().Append("_E0"), &E0,sSampler.Copy().Append("E0/F"));
  bOut_x0  = fChainOut->Branch(sSampler.Copy().Append("_x0"), &x0,sSampler.Copy().Append("x0/F"));
  bOut_y0  = fChainOut->Branch(sSampler.Copy().Append("_y0"), &y0,sSampler.Copy().Append("y0/F"));
  bOut_z0  = fChainOut->Branch(sSampler.Copy().Append("_z0"), &z0,sSampler.Copy().Append("z0/F"));
  bOut_xp0 = fChainOut->Branch(sSampler.Copy().Append("_xp0"), &xp0,sSampler.Copy().Append("xp0/F"));
  bOut_yp0 = fChainOut->Branch(sSampler.Copy().Append("_yp0"), &yp0,sSampler.Copy().Append("yp0/F"));
  bOut_zp0 = fChainOut->Branch(sSampler.Copy().Append("_zp0"), &zp0,sSampler.Copy().Append("zp0/F"));
  bOut_t0  = fChainOut->Branch(sSampler.Copy().Append("_t0"), &t0,sSampler.Copy().Append("t0/F"));
  bOut_E   = fChainOut->Branch(sSampler.Copy().Append("_E"), &E,sSampler.Copy().Append("E/F"));
  bOut_x   = fChainOut->Branch(sSampler.Copy().Append("_x"), &x,sSampler.Copy().Append("x/F"));
  bOut_y   = fChainOut->Branch(sSampler.Copy().Append("_y"), &y,sSampler.Copy().Append("y/F"));
  bOut_z   = fChainOut->Branch(sSampler.Copy().Append("_z"), &z,sSampler.Copy().Append("z/F"));
  bOut_xp  = fChainOut->Branch(sSampler.Copy().Append("_xp"), &xp,sSampler.Copy().Append("xp/F"));
  bOut_yp  = fChainOut->Branch(sSampler.Copy().Append("_yp"), &yp,sSampler.Copy().Append("yp/F"));
  bOut_zp  = fChainOut->Branch(sSampler.Copy().Append("_zp"), &zp,sSampler.Copy().Append("zp/F"));
  bOut_t   = fChainOut->Branch(sSampler.Copy().Append("_t"), &t,sSampler.Copy().Append("t/F"));
  bOut_X   = fChainOut->Branch(sSampler.Copy().Append("_X"), &X,sSampler.Copy().Append("X/F"));
  bOut_Y   = fChainOut->Branch(sSampler.Copy().Append("_Y"), &Y,sSampler.Copy().Append("Y/F"));
  bOut_Z   = fChainOut->Branch(sSampler.Copy().Append("_Z"), &Z,sSampler.Copy().Append("Z/F"));
  bOut_Xp  = fChainOut->Branch(sSampler.Copy().Append("_Xp"), &Xp,sSampler.Copy().Append("Xp/F"));
  bOut_Yp  = fChainOut->Branch(sSampler.Copy().Append("_Yp"), &Yp,sSampler.Copy().Append("Yp/F"));
  bOut_Zp  = fChainOut->Branch(sSampler.Copy().Append("_Zp"), &Zp,sSampler.Copy().Append("Zp/F"));
  bOut_s   = fChainOut->Branch(sSampler.Copy().Append("_s"), &s,sSampler.Copy().Append("s/F"));
  bOut_weight     = fChainOut->Branch(sSampler.Copy().Append("_weight"), &weight,sSampler.Copy().Append("weight/F"));
  bOut_partID     = fChainOut->Branch(sSampler.Copy().Append("_partID"), &partID,sSampler.Copy().Append("partID/I"));
  bOut_nEvent     = fChainOut->Branch(sSampler.Copy().Append("_nEvent"), &nEvent,sSampler.Copy().Append("nEvent/I"));
  bOut_parentID   = fChainOut->Branch(sSampler.Copy().Append("_parentID"), &parentID,sSampler.Copy().Append("parentID/I"));
  bOut_trackID    = fChainOut->Branch(sSampler.Copy().Append("_trackID"), &trackID,sSampler.Copy().Append("trackID/I"));
  bOut_turnnumber = fChainOut->Branch(sSampler.Copy().Append("_turnnumber"), &turnnumber,sSampler.Copy().Append("turnnumber/I"));
}

Bool_t Sampler::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Sampler::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChainIn) return;
   fChainIn->Show(entry);
}
#if 0
Int_t Sampler::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.  
  
  return 1;
}
#endif

#endif // #ifdef Sampler_cxx
