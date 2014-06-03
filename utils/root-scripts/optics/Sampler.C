#define Sampler_cxx
#include "Sampler.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <fstream>
#include <cmath>

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
   
   //loop over events in branch
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
     Long64_t ientry = LoadTree(jentry);
     if (ientry < 0) break;
     nb = fChain->GetEntry(jentry);   nbytes += nb;
     
     //cuts here
     
     // cut for primaries only
     if (partID != 2212) continue;
     // cut for second turn particles
     if (s > 26580.0) continue;
     
     //your function here
     wgt    += 1.0;
     x_s    += x;
     y_s    += y;
     xp_s   += xp*1e6;
     yp_s   += yp*1e6;
     E_s    += E;
     EE_s   += E*E;
     xx_s   += x*x;
     xxp_s  += x*xp*1e6;
     xpxp_s += xp*1e6*xp*1e6;
     xpE_s  += xp*1e6*E;
     xE_s   += x*E;
     yy_s   += y*y;
     yyp_s  += y*yp*1e6;
     ypyp_s += yp*1e6*yp*1e6;
     ypE_s  += yp*1e6*E;
     yE_s   += y*E;
   }
   x_s  /= wgt;
   y_s  /= wgt;
   xp_s /= wgt;
   yp_s /= wgt;
   E_s  /= wgt;
   EE_s /= wgt;
   xx_s /= wgt;
   xxp_s /= wgt;
   xpE_s /= wgt;
   xE_s  /= wgt;
   yy_s  /= wgt;
   yyp_s /= wgt;
   ypyp_s /= wgt;
   ypE_s /= wgt;
   yE_s /= wgt;



   //Calculate moments using the sums
   
   xx_s   -= x_s*x_s;
   xxp_s  -= x_s*xp_s;
   xpxp_s -= xp_s*xp_s;
   yy_s   -= y_s*y_s;  
   yyp_s  -= y_s*yp_s; 
   ypyp_s -= yp_s*yp_s;

   /*
   std::cout << "xx_s   " << xx_s   << std::endl;
   std::cout << "xpxp_s " << xpxp_s << std::endl;
   std::cout << "xxp_s  " << xxp_s  << std::endl;
   std::cout << "emit   " << xx_s*xpxp_s - xxp_s*xxp_s <<std::endl;
   */
   emitt_x = sqrt(xx_s*xpxp_s - xxp_s*xxp_s);
   emitt_y = sqrt(yy_s*ypyp_s - yyp_s*yyp_s);
   
   beta_x  =  xx_s  / emitt_x;
   beta_y  =  yy_s  / emitt_y;
   alph_x  = -xxp_s / emitt_x;
   alph_y  = -yyp_s / emitt_y;
   
   disp_x  = (xE_s  - (x_s  * E_s)) / (EE_s - (E_s * E_s));
   disp_xp = (xpE_s - (xp_s * E_s)) / (EE_s - (E_s * E_s));
   disp_y  = (yE_s  - (y_s  * E_s)) / (EE_s - (E_s * E_s));
   disp_yp = (ypE_s - (yp_s * E_s)) / (EE_s - (E_s * E_s));

   /*
   std::cout << "B(x): " << beta_x   << std::endl;
   std::cout << "B(y): " << beta_y   << std::endl;
   std::cout << "A(x): " << alph_x   << std::endl;
   std::cout << "A(y): " << alph_y   << std::endl;
   std::cout << std::endl;
   std::cout << "E(x): " << emitt_x  << std::endl;
   std::cout << "E(y): " << emitt_y  << std::endl;
   std::cout << std::endl;
   std::cout << "D(x): " << disp_x   << std::endl;
   std::cout << "D(x): " << disp_xp  << std::endl;
   std::cout << "D(x): " << disp_y   << std::endl;
   std::cout << "D(x): " << disp_yp  << std::endl;
   */
}

