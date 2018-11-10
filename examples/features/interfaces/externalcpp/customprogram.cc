#include "BDSIMClass.hh"

#include <iostream>

int main(int argc, char** argv)
{
  BDSIM* bds = new BDSIM(argc, argv);
  if (!bds->Initialised())
    {std::cout << "Intialisation failed" << std::endl; return 1;}

  std::cout << "Custom stuff here" << std::endl;
  
  bds->BeamOn();
  delete bds;
  return 0;
}
