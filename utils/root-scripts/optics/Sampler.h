//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Mar 12 11:53:11 2014 by ROOT version 5.34/14
// from TTree Sampler_13430_DRIFT458_13429_phys_1/Sampler output
// found on file: track3_0.root
//////////////////////////////////////////////////////////

#ifndef Sampler_h
#define Sampler_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <fstream>

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
  TBranch        *b_E0 ; // (GeV);   //!
  TBranch        *b_x0 ; // (mum);   //!
  TBranch        *b_y0 ; // (mum);   //!
  TBranch        *b_z0 ; // (m);   //!
  TBranch        *b_xp0 ; // (rad);   //!
  TBranch        *b_yp0 ; // (rad);   //!
  TBranch        *b_zp0 ; // (rad);   //!
  TBranch        *b_t0 ; // (ns);   //!
  TBranch        *b_E ; // (GeV);   //!
  TBranch        *b_x ; // (mum);   //!
  TBranch        *b_y ; // (mum);   //!
  TBranch        *b_z ; // (m);   //!
  TBranch        *b_xp ; // (rad);   //!
  TBranch        *b_yp ; // (rad);   //!
  TBranch        *b_zp ; // (rad);   //!
  TBranch        *b_t ; // (ns);   //!
  TBranch        *b_X ; // (mum);   //!
  TBranch        *b_Y ; // (mum);   //!
  TBranch        *b_Z ; // (m);   //!
  TBranch        *b_Xp ; // (rad);   //!
  TBranch        *b_Yp ; // (rad);   //!
  TBranch        *b_Zp ; // (rad);   //!
  TBranch        *b_s ; // (m);   //!
  TBranch        *b_weight;   //!
  TBranch        *b_partID;   //!
  TBranch        *b_nEvent;   //!
  TBranch        *b_parentID;   //!
  TBranch        *b_trackID;   //!
  TBranch        *b_turnnumber;   //!
  
  // My Data Members
  // sums - initialised to zero as that's what they start at
  double wgt;
  double x_s;  
  double y_s;    
  double xp_s;   
  double yp_s;   
  double E_s;    
  double EE_s;   
  double xx_s;   
  double xxp_s;  
  double xpxp_s; 
  double xpE_s;  
  double xE_s;   
  double yy_s;   
  double yyp_s;  
  double ypyp_s; 
  double ypE_s;  
  double yE_s;   
  double emitt_x, emitt_y;
  double beta_x, beta_y, alph_x, alph_y, disp_x, disp_xp, disp_y, disp_yp;
  
  Sampler(TTree *tree=0);
  virtual ~Sampler();
  virtual Int_t    Cut(Long64_t entry);
  virtual Int_t    GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual void     Init(TTree *tree);
  virtual void     CalculateOpticalFunctions();
  virtual Bool_t   Notify();
  virtual void     Show(Long64_t entry = -1);
  virtual void     AppendOpticalData(std::ofstream *ofs);
};

#endif

#ifdef Sampler_cxx
Sampler::Sampler(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("st_100_0.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("st_100_0.root");
      }
      f->GetObject("Sampler_11280_TAS1L1_11279_phys_1",tree);

   }
   Init(tree);
   
   //initialise sums
   wgt    = 0; //weight already used in data
   x_s    = 0;
   y_s    = 0;
   xp_s   = 0;
   yp_s   = 0;
   E_s    = 0;
   EE_s   = 0;
   xx_s   = 0;
   xxp_s  = 0;
   xpxp_s = 0;
   xpE_s  = 0;
   xE_s   = 0;
   yy_s   = 0;
   yyp_s  = 0;
   ypyp_s = 0;
   ypE_s  = 0;
   yE_s   = 0;
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

   fChain->SetBranchAddress("E0", &E0, &b_E0 ); //(GeV));
   fChain->SetBranchAddress("x0", &x0, &b_x0 ); //(mum));
   fChain->SetBranchAddress("y0", &y0, &b_y0 ); //(mum));
   fChain->SetBranchAddress("z0", &z0, &b_z0 ); //(m));
   fChain->SetBranchAddress("xp0", &xp0, &b_xp0 ); //(rad));
   fChain->SetBranchAddress("yp0", &yp0, &b_yp0 ); //(rad));
   fChain->SetBranchAddress("zp0", &zp0, &b_zp0 ); //(rad));
   fChain->SetBranchAddress("t0", &t0, &b_t0 ); //(ns));
   fChain->SetBranchAddress("E", &E, &b_E ); //(GeV));
   fChain->SetBranchAddress("x", &x, &b_x ); //(mum));
   fChain->SetBranchAddress("y", &y, &b_y ); //(mum));
   fChain->SetBranchAddress("z", &z, &b_z ); //(m));
   fChain->SetBranchAddress("xp", &xp, &b_xp ); //(rad));
   fChain->SetBranchAddress("yp", &yp, &b_yp ); //(rad));
   fChain->SetBranchAddress("zp", &zp, &b_zp ); //(rad));
   fChain->SetBranchAddress("t", &t, &b_t ); //(ns));
   fChain->SetBranchAddress("X", &X, &b_X ); //(mum));
   fChain->SetBranchAddress("Y", &Y, &b_Y ); //(mum));
   fChain->SetBranchAddress("Z", &Z, &b_Z ); //(m));
   fChain->SetBranchAddress("Xp", &Xp, &b_Xp ); //(rad));
   fChain->SetBranchAddress("Yp", &Yp, &b_Yp ); //(rad));
   fChain->SetBranchAddress("Zp", &Zp, &b_Zp ); //(rad));
   fChain->SetBranchAddress("s", &s, &b_s ); //(m));
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

void Sampler::AppendOpticalData(std::ofstream *output)
{
  *output << s       << "\t";
  *output << beta_x  << "\t";
  *output << beta_y  << "\t";
  *output << alph_x  << "\t";
  *output << alph_y  << "\t";
  *output << disp_x  << "\t";
  *output << disp_xp << "\t";
  *output << disp_y  << "\t";
  *output << disp_yp << "\t";
  *output << emitt_x << "\t";
  *output << emitt_y << "\n";
}


#endif // #ifdef Sampler_cxx

