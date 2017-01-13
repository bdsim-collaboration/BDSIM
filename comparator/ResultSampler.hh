#ifndef COMPRESULTSAMPLER_H
#define COMPRESULTSAMPLER_H

#include "Result.hh"

#include <sstream>
#include <string>

/**
 * @brief Result of comparison of a sampler branch.
 * 
 * @author Laurie Nevay
 */

class ResultSampler: public Result
{ 
public:
  ResultSampler(std::string name):
    Result(name, "Sampler")
  {;}
  
  std::vector<std::string> offendingLeaves;

  virtual std::string print() const
  {
    std::stringstream ss; 
    ss << Result::print();
    ss << "Offending branches: ";
    for (const auto& leafName : offendingLeaves)
      {ss << leafName << " ";}
    ss << "\n";
    return ss.str();
  }
};

#endif
