#define Sampler_cxx
#include "Sampler.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <cmath>

using namespace std;

void Sampler::CalculateOpticalFunctions()
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
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   //define your variables here
   
   //s denotes sum
   double x_s    = 0;
   double y_s    = 0;
   double z_s    = 0;
   double xp_s   = 0;
   double yp_s   = 0;
   double E_s    = 0;
   double EE_s   = 0;
   double xx_s   = 0;
   double xxp_s  = 0;
   double xpxp_s = 0;
   double xpE_s  = 0;
   double xE_s   = 0;
   double yy_s   = 0;
   double yyp_s  = 0;
   double ypyp_s = 0;
   double ypE_s  = 0;
   double yE_s   = 0;

   //loop over events in branch
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
     Long64_t ientry = LoadTree(jentry);
     if (ientry < 0) break;
     nb = fChain->GetEntry(jentry);   nbytes += nb;
     
     //cuts here
     
     // cut for primaries only
     //if (trackID != parentID) continue;
     
     //your function here
     x_s    += x;
     //cout << x_s << endl;
     y_s    += y;
     z_s    += z;
     xp_s   += xp;
     yp_s   += yp;
     E_s    += E;
     EE_s   += E*E;
     xx_s   += x*x;
     xxp_s  += x*xp;
     xpxp_s += xp*xp;
     xpE_s  += xp*E;
     xE_s   += x*E;
     yy_s   += y*y;
     yyp_s  += y*yp;
     ypyp_s += yp*yp;
     ypE_s  += yp*E;
     yE_s   += y*E;
   }
   
   xx_s   -= x_s*x_s;
   xxp_s  -= x_s*xp_s;
   xpxp_s -= xp_s*xp_s;
   yy_s   -= y_s*y_s;  
   yyp_s  -= y_s*yp_s; 
   ypyp_s -= yp_s*yp_s;

   //cout << "xx_s" << xx_s << endl;
   
   double emitt_x = sqrt(xx_s*xpxp_s - xxp_s*xxp_s);
   double emitt_y = sqrt(yy_s*ypyp_s - yyp_s*yyp_s);

   double beta_x, beta_y, alph_x, alph_y, disp_x, disp_xp, disp_y, disp_yp;
   
   beta_x  =  xx_s  / emitt_x;
   beta_y  =  yy_s  / emitt_y;
   alph_x  = -xxp_s / emitt_x;
   alph_y  = -yyp_s / emitt_y;
   
   disp_x  = (xE_s  - (x_s  * E_s)) / (EE_s - (E_s * E_s));
   disp_xp = (xpE_s - (xp_s * E_s)) / (EE_s - (E_s * E_s));
   disp_y  = (yE_s  - (y_s  * E_s)) / (EE_s - (E_s * E_s));
   disp_yp = (ypE_s - (yp_s * E_s)) / (EE_s - (E_s * E_s));

   cout << "B(x): " << disp_x  << endl;
   cout << "B(y): " << disp_x  << endl;
   cout << "A(x): " << disp_x  << endl;
   cout << "A(y): " << disp_x  << endl;
   cout << endl;
   cout << "E(x): " << emitt_x << endl;
   cout << "E(y): " << emitt_y << endl;
   cout << endl;
   cout << "D(x): " << disp_x  << endl;
   cout << "D(x): " << disp_x  << endl;
   cout << "D(x): " << disp_x  << endl;
   cout << "D(x): " << disp_x  << endl;

}
