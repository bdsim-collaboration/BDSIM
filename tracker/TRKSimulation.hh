#ifndef TRKSimulation_h
#define TRKSimulation_h

#include "TRKLine.hh"

class TRKSimulation {
private:
  std::string name; 
  TRKLine *line;

public :
  TRKSimulation();
  TRKSimulation(std::string name);
  ~TRKSimulation();
 
};

#endif
