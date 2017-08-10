/**
 * @file comparator.cc
 */

#include "Compare.hh"

#include <cmath>
#include <iostream>
#include <vector>

#include "TFile.h"
#include "TH1.h"
#include "TTree.h"

class Result;

void usage();

enum EXIT_CODE {
  _EXIT_SUCCESS        = 0,
  _EXIT_FAILED         = 1,
  _EXIT_INCORRECT_ARGS = 2,
  _EXIT_FILE_NOT_FOUND = 3
};

int main(int argc, char* argv[])
{
  if(argc != 3)
    { 
      usage();
      return _EXIT_INCORRECT_ARGS;    
    }

  // try to open files - check validity
  TFile *f1 = new TFile(argv[1]);
  TFile *f2 = new TFile(argv[2]);

  if(f1->IsZombie())
    {
      std::cout << "error : could not open " << argv[1] << std::endl;    
      return _EXIT_FILE_NOT_FOUND;
    }
  if(f2->IsZombie())
    {
      std::cout << "error : could not open " << argv[2] << std::endl;    
      return _EXIT_FILE_NOT_FOUND;
    }

  std::vector<Result*> results = Compare::Files(f1,f2);

  bool allPassed = Compare::Summarise(results);
  if (!allPassed)
    {
      std::cout << "TESTS_FAILED" << std::endl; // key to look for
      return _EXIT_FAILED;
    }
  else
    {return _EXIT_SUCCESS;}
}

void usage()
{ 
  std::cout << "Usage: comparator <rootfile1> <rootfile2>" << std::endl;
  std::cout << "Compares <rootfile2> to <rootfile1> - ie <rootfile1> is the reference." << std::endl;
}
