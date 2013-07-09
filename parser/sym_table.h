/* common header  for the lexer and the parser 
to share 
Symbol table for numeric variables and functions
*/

#include <list>

#define _INT 0
#define _DOUBLE 1
#define _ARRAY 2
 
#define NSYMS 65532
 
struct symtab {
  int is_reserved;
  int type;
  char *name;
  double (*funcptr)(double);
  double value;
  std::list<double> array;
};
 
struct symtab *symlook();


