#include "sym_table.h"

#include "array.h"

using namespace GMAD;

void Symtab::Set(Array* a)
{
  array.clear();
  for(unsigned int i=0;i<a->data.size();i++)
    array.push_back(a->data[i]);
  type = symtabtype::_ARRAY;
}

void Symtab::Print()
{
  printf("\t%s = ",name.c_str());
  std::list<double>::iterator it;
  switch(type) {
  case Symtab::symtabtype::_NUMBER:
    printf ("%.10g\n", value);
    break;

  case Symtab::symtabtype::_STRING:
    printf ("%s\n", str.c_str());
    break;

  case Symtab::symtabtype::_ARRAY:
    
    printf("\t%s = {",name.c_str());
    for(it=array.begin();it!=array.end();it++)
      {
	printf(" %.10g ",(*it));
      }
    printf("} \n");
    break;
    
  default:
    break;
  }
}
