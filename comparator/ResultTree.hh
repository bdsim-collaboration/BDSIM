#ifndef COMPRESULTTREE_H
#define COMPRESULTTREE_H

#include "Result.hh"

#include <sstream>
#include <string>

class ResultTree: public Result
{ 
public:
  int t1NEntries;
  int t2NEntries;

  virtual std::string print() const
  {
    std::stringstream ss; 
    ss << Result::print();
    ss << t1NEntries << "(" << t2NEntries << ") ";
    return ss.str();
  }
};

#endif
