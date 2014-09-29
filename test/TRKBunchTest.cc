#include "parser/gmad.h"
#include "parser/options.h"

#include "tracker/TRKBunch.hh"
#include "tracker/TRKLine.hh"
#include "tracker/TRKFactory.hh"
#include "tracker/TRKStrategy.hh"
#include "tracker/TRKTracker.hh"
#include "tracker/TRKHybrid.hh"
#include "tracker/TRKThin.hh"
#include "tracker/TRKAperture.hh"
#include "tracker/TRKQuadrupole.hh"

#include "BDSExecOptions.hh"

#include <iostream>

extern Options options;
extern ElementList beamline_list;

int main (int argc, char** argv){
  //for now, need exec options parsing from bdsim
  BDSExecOptions::Instance(argc,argv)->Print();
  
  //parse input
  gmad_parser(BDSExecOptions::Instance()->GetInputFilename());

  //build bunch - DONE
  TRKBunch* bunch   = new TRKBunch(options);

  std::cout << *bunch << std::endl;

  //create strategy / set of routines
  TRKStrategy* strategy = new TRKThin(1/*ntrackingsteps*/);

  //create a single element
  TRKAperture* ap = new TRKAperture(5.0,5.0,TRKAperture::rectangle);
  TRKPlacement* pl = new TRKPlacement();
  TRKQuadrupole* quad = new TRKQuadrupole(0.1,"quad1",2.5,ap,pl);  

  strategy->Track(quad,bunch);

  std::cout << *bunch << std::endl;

  TRKParticle& part = *(bunch->begin());
  part.SetPos(vector3(1,2,3));
  std::cout << part << std::endl;
  part.SetMom(vector3(0.001,0.001,1.0));
  std::cout << part << std::endl;
  part.ConfirmNewCoordinates();
  std::cout << part << std::endl;
  
  return 0;
}
