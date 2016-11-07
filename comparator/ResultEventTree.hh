#ifndef COMPRESULTEVENTTREE_H
#define COMPRESULTEVENTTREE_H

#include "Result.hh"
#include "ResultEvent.hh"

#include <sstream>
#include <string>

/**
 * @brief Result of comparison of all entries in an Event tree in BDSIM output.
 *
 * @author Laurie Nevay
 */

class ResultEventTree: public Result
{ 
public:
  int t1NEntries;
  int t2NEntries;
  std::vector<int> offendingEvents;
  std::vector<ResultEvent> eventResults;

  virtual std::string print() const
  {
    std::stringstream ss; 
    ss << Result::print();
    ss << "Event Tree (1/2) entries (" << t1NEntries << "/" << t2NEntries << ")\n";
    ss << "Offending event numbers: ";
    for (const auto& eventNo : offendingEvents)
      {ss << eventNo << " ";}
    for (const auto& eventResult : eventResults)
      {
	if (!eventResult.passed)
	  {ss << eventResult.print();}
      }
    ss << "\n";
    return ss.str();
  }
};

#endif
