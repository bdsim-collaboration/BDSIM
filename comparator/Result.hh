#ifndef COMPRESULT_H
#define COMPRESULT_H

#include <ostream>
#include <sstream>
#include <string>

class Result
{ 
public:
  std::string name;
  std::string objtype;
  int         iStatus;

  virtual std::string print() const
  {
    std::stringstream ss;
    ss << name << " " << objtype << " " << iStatus << " " ;
    return ss.str();
  }

  friend std::ostream& operator<<(std::ostream &ostr, const Result& rhs)
  {
    ostr << rhs.print(); 
    return ostr;
  }
};

#endif
