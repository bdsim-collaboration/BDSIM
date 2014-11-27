//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Nov 20 18:05:49 2014 by ROOT version 5.34/21
// from TTree PrecisionRegionElossTree/Energy Loss
// found on file: ./1_0.root
//////////////////////////////////////////////////////////

#ifndef PrecisionEloss_h
#define PrecisionEloss_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class PrecisionEloss {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Float_t         x;
   Float_t         y;
   Float_t         z;
   Float_t         s;
   Float_t         E;
   Float_t         weight;
   Int_t           partID;
   Char_t          volumeName;
   Int_t           turnnumber;

   // List of branches
   TBranch        *b_x;   //!
   TBranch        *b_y;   //!
   TBranch        *b_z;   //!
   TBranch        *b_s;   //!
   TBranch        *b_E;   //!
   TBranch        *b_weight;   //!
   TBranch        *b_partID;   //!
   TBranch        *b_volumeName;   //!
   TBranch        *b_turnnumber;   //!

   PrecisionEloss(TTree *tree=0);
   virtual ~PrecisionEloss();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef PrecisionEloss_cxx
PrecisionEloss::PrecisionEloss(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("./1_0.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("./1_0.root");
      }
      f->GetObject("PrecisionRegionElossTree",tree);

   }
   Init(tree);
}

PrecisionEloss::~PrecisionEloss()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t PrecisionEloss::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t PrecisionEloss::LoadTree(Long64_t entry)
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

void PrecisionEloss::Init(TTree *tree)
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

   fChain->SetBranchAddress("x", &x, &b_x);
   fChain->SetBranchAddress("y", &y, &b_y);
   fChain->SetBranchAddress("z", &z, &b_z);
   fChain->SetBranchAddress("s", &s, &b_s);
   fChain->SetBranchAddress("E", &E, &b_E);
   fChain->SetBranchAddress("weight", &weight, &b_weight);
   fChain->SetBranchAddress("partID", &partID, &b_partID);
   fChain->SetBranchAddress("volumeName", &volumeName, &b_volumeName);
   fChain->SetBranchAddress("turnnumber", &turnnumber, &b_turnnumber);
   Notify();
}

Bool_t PrecisionEloss::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void PrecisionEloss::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t PrecisionEloss::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef PrecisionEloss_cxx
