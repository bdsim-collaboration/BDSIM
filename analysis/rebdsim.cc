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
  }

  Config    *cf = Config::Instance(argv[1]);
  DataLoader dl = DataLoader(*cf);
}
