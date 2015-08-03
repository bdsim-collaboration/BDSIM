/* common header  for the lexer and the parser 
to share 
Symbol table for numeric variables and functions
*/

#include <cstring>
#include <list>


struct symtab {
  enum class symtabtype {
    _INT=0,
    _DOUBLE=1,
    _ARRAY=2
  };

  int is_reserved;
  symtabtype type;
  char *name;
  double (*funcptr)(double);
  double value;
  std::list<double> array;

  // constructors
  explicit symtab(const char* s):is_reserved(0),type(symtabtype::_INT),funcptr(nullptr),value(0.0){
    name=strdup(s);
  }
};
 
struct symtab *symlook();


