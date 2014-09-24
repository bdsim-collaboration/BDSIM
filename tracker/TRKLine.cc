#include "TRKLine.hh"

#include <iostream>

#include "TRKElement.hh"

TRKLine::TRKLine(std::string nameIn, bool circularIn) :
  name(nameIn),circular(circularIn)
{
}

TRKLine::~TRKLine()
{
}

void TRKLine::AddElement(TRKElement* e) {
  elements.push_back(e);
}

TRKElement* TRKLine::FindElement(std::string eName)const {
 TRKLineConstIter elIter = elements.begin();
 TRKLineConstIter elIterEnd = elements.end();
  for (;elIter!=elIterEnd; ++elIter) {
    if ((*elIter)->GetName() == eName) {
      return (*elIter);
    }
  }
  /// if not found return NULL
  return NULL;
}

/// output stream
std::ostream& operator<< (std::ostream &out, const TRKLine &line) {
  TRKLineConstIter elIter = line.begin();
  TRKLineConstIter elIterEnd = line.end();
  for (;elIter!=elIterEnd; ++elIter) {
    out << **elIter << std::endl;
  }
  return out;
}
