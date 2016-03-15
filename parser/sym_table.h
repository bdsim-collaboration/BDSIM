#ifndef __SYM_TABLE_H
#define __SYM_TABLE_H

#include <list>
#include <string>

namespace GMAD {
  /** 
      @brief Common header for the lexer and the parser 
      to share Symbol table for numeric variables and functions
  */
  class Array;
  
  class Symtab {
    // TODO: needs better data protection
  public:
    /// Symtabs and arrays are kind of interchangeable
    friend class Array;

    enum class symtabtype {
      _INT=0,
      _DOUBLE=1,
      _STRING=2,
      _ARRAY=3
    };

    int is_reserved;
    symtabtype type;
    std::string name;

    ///@{ data
    double (*funcptr)(double);
    double value;
    std::string str;
    std::list<double> array;
    ///@}
    
    // constructors
    explicit Symtab(std::string s):is_reserved(0),type(symtabtype::_INT),name(s),funcptr(nullptr),value(0.0){}
    /// Set to Array value
    void Set(Array*);
    /// Print method
    void Print();
  };
}

#endif
