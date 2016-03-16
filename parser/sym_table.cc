#include "sym_table.h"

#include <cstdlib>
#include <iostream>

#include "array.h"

using namespace GMAD;

Symtab::Symtab(std::string s):
  name(s),
  is_reserved(false),
  type(symtabtype::NUMBER),
  funcptr(nullptr),
  value(0.0)
{
}

void Symtab::Set(Array* a)
{
  array.clear();
  for(unsigned int i=0;i<a->data.size();i++)
    array.push_back(a->data[i]);
  type = symtabtype::ARRAY;
}

void Symtab::Set(std::string a)
{
  str = a;
  type = symtabtype::STRING;
}


void Symtab::Set(double a, bool reserved)
{
  value = a;
  is_reserved = reserved;
  type = symtabtype::NUMBER;
}

void Symtab::Set(Symtab::function a)
{
  funcptr = a;
  type = symtabtype::FUNCTION;
}

std::string Symtab::GetName()const
{
  return name;
}

Symtab::symtabtype Symtab::GetType()const
{
  return type;
}

bool Symtab::IsReserved()const
{
  return is_reserved;
}

double Symtab::GetNumber()const
{
  if (type!=symtabtype::NUMBER)
    {
      std::cout << "Symbol is not a number!" << std::endl;
      exit(1);
    }
  return value;
}

std::string Symtab::GetString()const
{
  if (type!=symtabtype::STRING)
    {
      std::cout << "Symbol is not a string!" << std::endl;
      exit(1);
    }
  return str;
}

std::list<double> Symtab::GetArray()const
{
  if (type!=symtabtype::ARRAY)
    {
      std::cout << "Symbol is not a array!" << std::endl;
      exit(1);
    }
  return array;
}

Symtab::function Symtab::GetFunction()const
{
  if (type!=symtabtype::FUNCTION)
    {
      std::cout << "Symbol is not a function!" << std::endl;
      exit(1);
    }
  return funcptr;
}

void Symtab::Print()
{
  printf("\t%s = ",name.c_str());
  std::list<double>::iterator it;
  switch(type) {
  case Symtab::symtabtype::NUMBER:
    printf ("%.10g\n", value);
    break;

  case Symtab::symtabtype::STRING:
    printf ("%s\n", str.c_str());
    break;

  case Symtab::symtabtype::ARRAY:
    
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
