#ifndef TRKSimulation_h
#define TRKSimulation_h

#include "TRKLine.h"

class TRKSimulation {
private:
  std::string name; 
  TRKLine *line;

public :
  TRKSimulation();
  TRKSimulation(std::string name);
  

}

#endif
