/*
 *  interactive gmad
 *  Ilya Agapov 2005
 */

#include <cstdio>
#include <iostream>

#include "gmad.h"

int main(int argc, char *argv[])
{
  std::cout<<std::endl;
  if(argc<2) return -1;
  gmad_parser(argv[1]);
}
