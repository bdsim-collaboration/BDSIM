/// test file for tracker

#include <iostream>
#include <string>

#include "TRKAperture.hh"
#include "TRKDrift.hh"
#include "TRKLine.hh"
#include "TRKPlacement.hh"
#include "TRKQuadrupole.hh"

int main() {
  /// instantiate elements
  std::string driftname = "drift";
  double length = 3.0, size_x = 1.0, size_y = 1.0;
  TRKAperture* aper = NULL;
  TRKPlacement* plac = NULL;

  std::cout << "Create Drift" << std::endl;
  TRKDrift drift(driftname,length,size_x,size_y,aper,plac);

  std::string quadname = "quad";
  double strength = 1.0;
  std::cout << "Create Quadrupole" << std::endl;
  TRKQuadrupole quad(quadname,length,size_x,size_y,strength,aper,plac);
  
  std::cout << "Test successful!" << std::endl;
  return 0;
}
