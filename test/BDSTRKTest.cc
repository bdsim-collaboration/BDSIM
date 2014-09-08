/// test file for tracker

#include <iostream>
#include <string>

#include "tracker/TRKAperture.hh"
#include "tracker/TRKDrift.hh"
#include "tracker/TRKFactory.hh"
#include "tracker/TRKLine.hh"
#include "tracker/TRKPlacement.hh"
#include "tracker/TRKOctopole.hh"
#include "tracker/TRKQuadrupole.hh"
#include "tracker/TRKSextupole.hh"

#include "tracker/vector3.hh"
#include "tracker/vector6.hh"

// GMAD parser
#include "parser/gmad.h"
#include "parser/options.h"

extern ElementList beamline_list;
extern Options options;

int main(int argc,char** argv) {

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
  std::string sextname = "sext";
  std::string octname = "oct";
  double strength = 1.0;
  std::cout << "Create Quadrupole" << std::endl;
  TRKQuadrupole quad(strength, type, TRK::DEFAULT_TRACKING_STEPS,
		     quadname,length,size_x,size_y,aper,plac);
  
  std::cout << "Create Sextupole" << std::endl;
  TRKSextupole sext(strength, type, TRK::DEFAULT_TRACKING_STEPS,
		    sextname,length,size_x,size_y,aper,plac);

  std::cout << "Create Octopole" << std::endl;
  TRKOctopole oct(strength, type, TRK::DEFAULT_TRACKING_STEPS,
		  octname,length,size_x,size_y,aper,plac);


  std::cout << "Create Line" << std::endl;
  TRKLine line("line");
  line.AddElement(&drift);
  line.AddElement(&quad);
  line.AddElement(&sext);
  line.AddElement(&oct);
  std::cout << "Line created" << std::endl; 
  std::cout << line << std::endl;
  TRKElement* element = line.FindElement("drift");
  if (element) std::cout << element->GetName() << " has been found" << std::endl;

  double vIn[6] = {1.0, 0.0, 0.0, 0.0, 0.01, 1}; 
  double vOut[6]; 
  
  std::cout << "Thin Tracking" << std::endl;
  line.Track(vIn,vOut);

  // second method of creation, input file and factory 

  // default filename
  std::string filename = "./BDSTrackingTestFiles/gmad";

  /// first argument is gmad file
  if (argc>1){
    filename = argv[1];
  }

  // fill options from file 
  gmad_parser(filename);

  std::cout << "Thick tracking" << std::endl;

  TRKTrackingElement::TRKType type2 = TRKTrackingElement::thick;

  TRKFactory trkfactory = TRKFactory(type2,options);
  TRKTrackingElement* createdLine = trkfactory.createLine(beamline_list);
  TRKLine* line2 = dynamic_cast<TRKLine*>(createdLine);
  if (line2) {
    std::cout << "line created from gmad file: " << std::endl;
    std::cout << *line2 << std::endl;

    line2->Track(vIn,vOut);
  }

  std::cout << "Hybrid tracking" << std::endl;

  TRKTrackingElement::TRKType type3 = TRKTrackingElement::hybrid;

  TRKFactory trkfactory3 = TRKFactory(type3,options);
  TRKTrackingElement* createdLine3 = trkfactory3.createLine(beamline_list);
  TRKLine* line3 = dynamic_cast<TRKLine*>(createdLine3);
  if (line3) line3->Track(vIn,vOut);

  std::cout << "Test successful!" << std::endl;
  return 0;
}
