#define Sampler_cxx
#include "Sampler.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void Sampler::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L Sampler.C
//      Root > Sampler t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChainIn->SetBranchStatus("*",0);  // disable all branches
//    fChainIn->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChainIn->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChainIn == 0) return;

   Long64_t nentries = fChainIn->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChainIn->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}
