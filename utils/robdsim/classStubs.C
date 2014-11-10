#include "TFile.h"
#include "TTree.h"

void classStubs(char *fileName) {
  TFile *f = new TFile(fileName);

  TTree *primaries = (TTree*)f->Get("primaries");                   // Samplers and primary tree
  TTree *eloss     = (TTree*)f->Get("ElossTree");                   // Eloss and ploss tress
  TTree *peloss    = (TTree*)f->Get("PrecisionRegionElossTree");    // Precision region eloss tree
  
  primaries->MakeClass("Sampler");
  eloss->MakeClass("Eloss");
  peloss->MakeClass("PrecisionRegionEloss");
}
