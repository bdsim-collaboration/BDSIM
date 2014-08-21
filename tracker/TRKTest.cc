/// test file for tracker

#include <iostream>
#include <string>

#include "TRKAperture.hh"
#include "TRKDrift.hh"
#include "TRKLine.hh"
#include "TRKPlacement.hh"
#include "TRKQuadrupole.hh"

#include "vector3.hh"
#include "vector6.hh"

int main() {

#ifdef TRKDEBUG 
  std::cout << "Debug on" << std::endl;
#endif

  /// vector testing
  vector3 v1(1,0,0);
  vector3 v2(0,2,0);
  vector3 v3(v2);
  vector3 v4 = v3;
  vector3 v5 = v3 + v4;
  v5 = v5 - v1;
  v5 = v5 * 2;
  v5 = v5 / 5;
  std::cout << v5 << " " << v5.mag() << " " << v5.unit() << std::endl;

  vector6 v6(v1,v2);
  vector6 v7 = v6 * 2;
  vector6 v8 = v7 - v6;
  
  std::cout << v8 << std::endl;

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

  double vIn[6] = {1.0, 0.0, 0.0, 0.0, 0.01, 1}; 
  double vOut[6]; 
  
  std::cout << "Tracking" << std::endl;
  line.Track(vIn,vOut);

  std::cout << "Test successful!" << std::endl;
  return 0;
}
