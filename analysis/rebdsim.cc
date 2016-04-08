#include <iostream>

#include "TChain.h"

#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventModel.hh"

#include "Config.hh"
#include "DataLoader.hh"

int main(int argc, char *argv[])
{
  // check input
  if(argc != 2)
  {
    std::cout << "usage rebdsim <ConfigFileName>" << std::endl;
    exit(1);
  }

  std::cout << "rebdsim> ConfigFileName : " << argv[1] << std::endl;

  try {
    Config::Instance(argv[1]);
  }
  catch(std::string e) {
    std::cout << e << std::endl;
    exit(1);
  }

  try {
    DataLoader dl = DataLoader();
  }
  catch(std::string e) {
    std::cout << e << std::endl;
    exit(1);    
  }

}
