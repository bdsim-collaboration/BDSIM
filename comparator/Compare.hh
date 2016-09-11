#ifndef COMPCOMPARE_H
#define COMPCOMPARE_H

#include "Result.hh"

#include <vector>

class ResultHistogram;
class ResultTree;
class TFile;
class TH1;
class TTree;

namespace Compare
{
  std::vector<Result*>  Files(TFile *f1, TFile *f2);
  ResultHistogram*      Histograms(TH1 *h1, TH1 *h2); 
  ResultTree*           Trees(TTree* t1, TTree* t2);
  void                  PrintResults(std::vector<Result*> results);
  void                  PrintFailure(std::vector<Result*> results);
  bool                  CheckResults(std::vector<Result*> results);
}
  
#endif
