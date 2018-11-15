#include "BDSIMClass.hh" // bdsim interface

#include "UDipole.hh"
#include "UDipoleConstructor.hh"

#include <iostream>

int main(int argc, char** argv)
{
  // construct an instance of bdsim
  BDSIM* bds = new BDSIM();

  // register a custom component by name udipole with a user-provided constructor
  // BDSIM will delete the constructor at the end.
  bds->RegisterUserComponent("udipole", new UDipoleConstructor());

  // construct geometry and physics
  bds->Initialise(argc, argv);
  if (!bds->Initialised()) // check if there was a problem.
    {std::cout << "Intialisation failed" << std::endl; return 1;}
  
  bds->BeamOn(); // run the simulation
  delete bds;    // clean up
  return 0;      // exit nicely
}
