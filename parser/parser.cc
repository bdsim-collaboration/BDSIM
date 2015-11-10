/*
 *  interactive gmad
 *  Ilya Agapov 2005
 */

#include <cstdio>
#include <iostream>

#include "gmad.h"

using namespace GMAD;

int main(int argc, char *argv[])
{
  if(argc<2) {
    std::cout << "GMAD parser needs an input file" << std::endl;
    return 1;
  }
  if(argc>2) {
    std::cout << "GMAD parser needs only one input file" << std::endl;
    return 1;
  }
  gmad_parser(argv[1]);
  return 0;
}
