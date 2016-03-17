#ifndef SYMBOLMAP_H
#define SYMBOLMAP_H

#include <map>
#include "sym_table.h"

namespace GMAD {
  /** 
      @brief Class that hold the symbol table
  */

  class SymbolMap {
  public:
    /// Destructor that clears memory
    void clear();
    /// Create new parser symbol
    Symtab * symcreate(std::string s);
    /// Look up parser symbol
    Symtab * symlook(std::string s);

  private:
    /// Parser symbol map
    std::map<std::string, Symtab*> symtab_map;
  };
}

#endif
