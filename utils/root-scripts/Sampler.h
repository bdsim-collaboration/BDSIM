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

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class Sampler {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

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

   Sampler(TTree *tree=0);
   virtual ~Sampler();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Sampler_cxx
Sampler::Sampler(TTree *tree) : fChain(0) 
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
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Sampler::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Sampler::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Sampler::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("E0", &E0, &b_E0);
   fChain->SetBranchAddress("x0", &x0, &b_x0);
   fChain->SetBranchAddress("y0", &y0, &b_y0);
   fChain->SetBranchAddress("z0", &z0, &b_z0);
   fChain->SetBranchAddress("xp0", &xp0, &b_xp0);
   fChain->SetBranchAddress("yp0", &yp0, &b_yp0);
   fChain->SetBranchAddress("zp0", &zp0, &b_zp0);
   fChain->SetBranchAddress("t0", &t0, &b_t0);
   fChain->SetBranchAddress("E", &E, &b_E);
   fChain->SetBranchAddress("x", &x, &b_x);
   fChain->SetBranchAddress("y", &y, &b_y);
   fChain->SetBranchAddress("z", &z, &b_z);
   fChain->SetBranchAddress("xp", &xp, &b_xp);
   fChain->SetBranchAddress("yp", &yp, &b_yp);
   fChain->SetBranchAddress("zp", &zp, &b_zp);
   fChain->SetBranchAddress("t", &t, &b_t);
   fChain->SetBranchAddress("X", &X, &b_X);
   fChain->SetBranchAddress("Y", &Y, &b_Y);
   fChain->SetBranchAddress("Z", &Z, &b_Z);
   fChain->SetBranchAddress("Xp", &Xp, &b_Xp);
   fChain->SetBranchAddress("Yp", &Yp, &b_Yp);
   fChain->SetBranchAddress("Zp", &Zp, &b_Zp);
   fChain->SetBranchAddress("s", &s, &b_s);
   fChain->SetBranchAddress("weight", &weight, &b_weight);
   fChain->SetBranchAddress("partID", &partID, &b_partID);
   fChain->SetBranchAddress("nEvent", &nEvent, &b_nEvent);
   fChain->SetBranchAddress("parentID", &parentID, &b_parentID);
   fChain->SetBranchAddress("trackID", &trackID, &b_trackID);
   fChain->SetBranchAddress("turnnumber", &turnnumber, &b_turnnumber);
   Notify();
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
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Sampler::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Sampler_cxx
