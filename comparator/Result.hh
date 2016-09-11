#ifndef COMPRESULT_H
#define COMPRESULT_H

#include <ostream>
#include <sstream>
#include <string>

/**
 * @brief Base class of a result.
 *
 * The basic information each test result should have. Public
 * member class with compiler provided default constructor to use
 * as a struct.
 * 
 * @author Stewart Boogert
 */

class Result
{ 
public:
  std::string name;    ///< Name of object being compared in files.
  std::string objtype; ///< Name of class of object being compared in files.
  int         iStatus; ///< Whether it passed or not.

  /// Print the result of the test - ie information store in the class.
  virtual std::string print() const
  {
    std::stringstream ss;
    ss << name << " " << objtype << " " << iStatus << " " ;
    return ss.str();
  }

  /// Overload ostream for easy printing.
  friend std::ostream& operator<<(std::ostream &ostr, const Result& rhs)
  {
    ostr << rhs.print(); 
    return ostr;
  }
};

#endif
