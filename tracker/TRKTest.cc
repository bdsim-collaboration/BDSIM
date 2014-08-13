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
  TRKTrackingElement::TRKType type = TRKTrackingElement::thin;
  TRKDrift drift(type, TRK::DEFAULT_TRACKING_STEPS,
		 driftname,length,size_x,size_y,aper,plac);

  std::string quadname = "quad";
  double strength = 1.0;
  std::cout << "Create Quadrupole" << std::endl;
  TRKQuadrupole quad(strength, type, TRK::DEFAULT_TRACKING_STEPS,
		     quadname,length,size_x,size_y,aper,plac);
  
  std::cout << "Create Line" << std::endl;
  TRKLine line("line");
  line.AddElement(&drift);
  line.AddElement(&quad);
  TRKElement* element = line.FindElement("drift");
  if (element) std::cout << element->GetName() << " has been found" << std::endl;

  std::cout << "Test successful!" << std::endl;
  return 0;
}
