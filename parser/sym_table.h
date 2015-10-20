#ifndef __SYM_TABLE_H
#define __SYM_TABLE_H

#include <list>
#include <string>

namespace GMAD {
  /** 
      @brief Common header for the lexer and the parser 
      to share Symbol table for numeric variables and functions
  */
  struct symtab {
    enum class symtabtype {
      _INT=0,
      _DOUBLE=1,
      _ARRAY=2
    };

    int is_reserved;
    symtabtype type;
    std::string name;
    double (*funcptr)(double);
    double value;
    std::list<double> array;
    
    // constructors
    explicit symtab(std::string s):is_reserved(0),type(symtabtype::_INT),name(s),funcptr(nullptr),value(0.0){}
  };
}

#endif
