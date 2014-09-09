#ifndef TRKSimulation_h
#define TRKSimulation_h

#include "TRKLine.hh"

/**
 * @brief what does this class do?
 */
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
