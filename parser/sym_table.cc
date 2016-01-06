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
  if(type == Symtab::symtabtype::_ARRAY)
    {
      printf("\t%s = {",name.c_str());
      std::list<double>::iterator it;
      for(it=array.begin();it!=array.end();it++)
	{
	  printf(" %.10g ",(*it));
	}
      printf("} \n");
    }
  else
    {
      printf ("%.10g\n", value);
    }
}
