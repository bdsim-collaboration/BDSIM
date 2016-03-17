#include "symbolmap.h"

#include <cstdlib>
#include <iostream>

using namespace GMAD;

Symtab * SymbolMap::symcreate(std::string s)
{
  std::map<std::string,Symtab*>::iterator it = symtab_map.find(s);
  if (it!=symtab_map.end()) {
    std::cerr << "ERROR Variable " << s << " is already defined!" << std::endl;
    exit(1);
  }
    
  Symtab* sp = new Symtab(s);
  std::pair<std::map<std::string,Symtab*>::iterator,bool> ret = symtab_map.insert(std::make_pair(s,sp));
  return (*(ret.first)).second;
}
  
Symtab * SymbolMap::symlook(std::string s)
{
  std::map<std::string,Symtab*>::iterator it = symtab_map.find(s);
  if (it==symtab_map.end()) {
    return nullptr;
  } 
  return (*it).second;
}

void SymbolMap::clear()
{
  std::map<std::string,Symtab*>::iterator it;
  for(it=symtab_map.begin();it!=symtab_map.end();++it) {
    delete (*it).second;
  }
  symtab_map.clear();
}
