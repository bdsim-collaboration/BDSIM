#ifndef COMPRESULTEVENT_H
#define COMPRESULTEVENT_H

#include "Result.hh"
#include "ResultSampler.hh"

#include <sstream>
#include <string>

/**
 * @brief Result of comparison of single entry of an Event tree in BDSIM output.
 *
 * @author Laurie Nevay
 */

class ResultEvent: public Result
{ 
public:
  std::vector<std::string> offendingBranches;
  std::vector<ResultSampler> samplerResults;

  virtual std::string print() const
  {
    std::stringstream ss; 
    ss << Result::print();
    ss << "Offending branches: ";
    for (const auto& branchName : offendingBranches)
      {ss << branchName << " ";}
    ss << "\nOffending samplers: ";
    for (const auto& samplerResult : samplerResults)
      {
	if (!samplerResult.passed)
	  {ss << samplerResult.print();}
      }
    ss << "\n";
    return ss.str();
  }
};

#endif
