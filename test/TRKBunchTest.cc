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
#include "tracker/TRKApertureCircular.hh"
#include "tracker/TRKPlacement.hh"
#include "tracker/TRKQuadrupole.hh"

#include "BDSExecOptions.hh"
#include "BDSOutputFactory.hh"

#include <iostream>

extern Options options;
extern ElementList beamline_list;

//GLOBALS
BDSOutputBase* trkOutput=NULL; //output interface

int main (int argc, char** argv){
  //for now, need exec options parsing from bdsim
  BDSExecOptions::Instance(argc,argv)->Print();
  
  //parse input
  gmad_parser(BDSExecOptions::Instance()->GetInputFilename());

  //initialise output
  trkOutput = BDSOutputFactory::createOutput(BDSExecOptions::Instance()->GetOutputFormat());

  //build bunch
  TRKBunch* bunch   = new TRKBunch(options);
  std::cout << *bunch << std::endl;

  //create strategy / set of routines
  TRKStrategy* strategy = new TRKThin(1/*ntrackingsteps*/);

  //create a single element
  TRKAperture* ap = new TRKApertureCircular(0.05);
  TRKPlacement* pl = new TRKPlacement();
  TRKQuadrupole* quad = new TRKQuadrupole(0.1,"quad1",2.5,ap,pl);  
  quad->SetOffset(0.3,0.2);

  //track the bunch through a quad
  strategy->Track(quad,bunch);
  std::cout << *bunch;

  //get a single particle and fiddle it a bit
  TRKParticle& part = *(bunch->begin());
  part.SetPosMom(vector3(1,2,3),vector3(0.001,0.001,1.0));
  std::cout << part << std::endl;
  part.ConfirmNewCoordinates();
  std::cout << part << std::endl;
  
  return 0;
}
