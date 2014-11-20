#include <iostream>

#include "TFile.h" 
#include "TTree.h"

void MakeClasses(char *fileName) { 
  TFile *f = new TFile(fileName);
  TTree *pt = (TTree*)f->Get("primaries"); // Template for the samplers 
  TTree *el = (TTree*)f->Get("ElossTree"); // Template for the eloss and peloss 
  TTree *pl = (TTree*)f->Get("PrecisionRegionElossTree"); // Precision region eloss 
  
  std::cout << "MakeClasses> Primaries            :  " << pt << std::endl;
  std::cout << "MakeClasses> Eloss, Ploss         :  " << el << std::endl;
  std::cout << "MakeClasses> PrecisionRegionEloss :  " << pl << std::endl;

  pt->MakeClass("Sampler");
  el->MakeClass("Eloss");
  pl->MakeClass("PrecisionEloss");   
}
