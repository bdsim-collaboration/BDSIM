 /*
 * GMAD interface
 *
 */
#include "parser.h"

#include <cstdio>
#include <iostream>

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
  Parser::Instance(std::string(argv[1]));
  return 0;
}

