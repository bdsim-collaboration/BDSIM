/* common header  for the lexer and the parser 
to share 
Symbol table for numeric variables and functions
*/

#include <cstring>
#include <list>

enum {
  _INT=0,
  _DOUBLE=1,
  _ARRAY=2
};
 
struct symtab {
  int is_reserved;
  int type;
  char *name;
  double (*funcptr)(double);
  double value;
  std::list<double> array;

  // constructors
  explicit symtab(const char* s):is_reserved(0),type(0),funcptr(nullptr),value(0.0){
    name=strdup(s);
  }
};
 
struct symtab *symlook();


