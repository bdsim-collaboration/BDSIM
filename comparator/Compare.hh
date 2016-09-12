#ifndef COMPCOMPARE_H
#define COMPCOMPARE_H

#include "Result.hh"

#include <vector>

class TDirectory;
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
  std::vector<Result*> Files(TFile* f1, TFile* f2);

  /// Compare two directories by changing into d1 and inspecting all objects.
  /// The results are recorded via a reference to a results vector. The
  /// original directory is changed back to at the end of the method.
  void Directories(TDirectory* d1,
		   TDirectory* d2,
		   std::vector<Result*>& results);

  /// Compare two histogams.
  void Histograms(TH1* h1, TH1* h2, std::vector<Result*>& results);
  
  /// Compare two TTrees.
  void Trees(TTree* t1, TTree* t2, std::vector<Result*>& results);

  /// Compare an optics TTree specifically. This relies on the known variable
  /// names in the tree and naming scheme.
  void Optics(TTree* t1, TTree* t2, std::vector<Result*>& results);

  /// Print results from a serious of tests.
  void PrintResults(std::vector<Result*> results);

  /// Print failures from a serious of tests.
  void PrintFailure(std::vector<Result*> results);

  /// Check the results from a serious of tests and return true if any failed.
  bool AnyFailed(std::vector<Result*> results);

  /// Simply print out feedback warning that a matching object wasn't found and
  /// no comparison is being done.
  void PrintNoMatching(std::string className, std::string objectName);

  /// Loop over results and print any failed ones. Returns true if all passed.
  bool Summarise(std::vector<Result*> results);

  /// Check wether a string is prefixed with another string.
  bool StringStartsWith(std::string aString, std::string prefix);
}
  
#endif
