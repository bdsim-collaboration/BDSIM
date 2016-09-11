#include "Compare.hh"
#include "Result.hh"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "TFile.h"
#include "TH1.h"
#include "TTree.h"

void usage();

#define _EXIT_SUCCESS        0;
#define _EXIT_FAILED         1;  
#define _EXIT_INCORRECT_ARGS 2;
#define _EXIT_FILE_NOT_FOUND 3;

int main(int argc, char* argv[])
{
  if(argc != 3)
    { 
      usage();
      return _EXIT_INCORRECT_ARGS;    
    }

  // open files 
  
  TFile *f1 = new TFile(argv[1]);
  TFile *f2 = new TFile(argv[2]);

  if(f1->IsZombie()) {
    std::cout << "error : could not open " << argv[1] << std::endl;    
    return _EXIT_FILE_NOT_FOUND;
  }

  if(f2->IsZombie()) {
    std::cout << "error : could not open " << argv[2] << std::endl;    
    return _EXIT_FILE_NOT_FOUND;
  }

  std::vector<Result*> res = Compare::Files(f1,f2);

  Compare::PrintResults(res);
  //  Compare::PrintFailure(res);

  if(!Compare::CheckResults(res))
    {std::cout << "Tests passed" << std::endl;}
  else
    {
      std::cout << "Tests failed" << std::endl;
      return _EXIT_FAILED;
    }
  
  return _EXIT_SUCCESS;
}

void usage()
{ 
  std::cout << "usage : robdsimComp rootFile1 rootFile2 " << std::endl;
  std::cout << "Compares rootFile2 to rootFile1 - ie rootFile1 is the reference." << std::endl;
}
