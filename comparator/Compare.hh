#ifndef COMPCOMPARE_H
#define COMPCOMPARE_H

#include "Result.hh"

#include <vector>

class ResultHistogram;
class ResultTree;
class TFile;
class TH1;
class TTree;

/**
 * @brief Comparison utility functions
 * @author Stewart Boogert
 */

namespace Compare
{
  /// Compare two files.
  std::vector<Result*> Files(TFile *f1, TFile *f2);

  /// Compare two histogams.
  ResultHistogram* Histograms(TH1 *h1, TH1 *h2);

  /// Compare two TTrees.
  ResultTree* Trees(TTree* t1, TTree* t2);

  /// Print results from a serious of tests.
  void PrintResults(std::vector<Result*> results);

  /// Print failures from a serious of tests.
  void PrintFailure(std::vector<Result*> results);

  /// Check the results from a serious of tests and return true if all passed.
  bool CheckResults(std::vector<Result*> results);
}
  
#endif
