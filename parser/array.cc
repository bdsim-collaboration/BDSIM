#include "array.h"

#include "sym_table.h"

#include <iostream>

using namespace GMAD;

Array::Array()
{
}

Array::Array(Symtab* array)
{
  for(double it : array->array)
    {
      data.push_back(it);
    }
}

Array* Array::Add(Array* a1, Array* a2)
{
  Array* a = new Array();
  unsigned int size = (a1->data.size() < a2->data.size() )? a1->data.size() : a2->data.size();

  a->data.resize(size);
  for(unsigned int i=0;i<size;i++)
    {
      a->data[i] = a1->data[i] + a2->data[i];
    }
  return a;
}

Array* Array::Subtract(Array* a1, Array* a2)
{
  Array* a = new Array();
  unsigned int size = (a1->data.size() < a2->data.size() )? a1->data.size() : a2->data.size();
  a->data.resize(size);
  for(unsigned int i=0;i<size;i++)
    {
      a->data[i] = a1->data[i] - a2->data[i];
    }
  return a;
}

Array* Array::Add(Array* a1, double d2)
{
  Array* a = new Array();
  unsigned int size = a1->data.size();
  a->data.resize(size);
  for(unsigned int i=0;i<size;i++)
    {
      a->data[i] = a1->data[i] + d2;
    }
  return a;
}

Array* Array::Subtract(Array* a1, double d2)
{
  Array* a = new Array();
  unsigned int size = a1->data.size();
  a->data.resize(size);
  for(unsigned int i=0;i<size;i++)
    {
      a->data[i] = a1->data[i] - d2;
    }
  return a;
}

Array* Array::Multiply(Array* a1, double d2)
{
  Array* a = new Array();
  unsigned int size = a1->data.size();
  a->data.resize(size);
  for(unsigned int i=0;i<size;i++)
    {
      a->data[i] = a1->data[i] * d2;
    }
  return a;
}

Array* Array::Divide(Array* a1, double d2)
{
  Array* a = new Array();
  unsigned int size = a1->data.size();
  a->data.resize(size);
  for(unsigned int i=0;i<size;i++)
    {
      a->data[i] = a1->data[i] / d2;
    }
  return a;
}

double Array::Product(Array* a)
{
  if(data.size() != a->data.size())
    {
      std::cerr << "ERROR: vector dimensions do not match" << std::endl;
      exit(1);
    }

  double dummy = 0;
  for(unsigned int i=0;i<data.size();i++)
    dummy += data[i] * a->data[i];
  return dummy;
}

void Array::Clear()
{
  data.clear();
  symbols.clear();
}

void Array::Print()
{
  for(double d : data)
    {
      std::cout << d << " ";
    }
  std::cout << std::endl;
  for(std::string s : symbols)
    {
      std::cout << s << " ";
    }
  std::cout << std::endl;
}
