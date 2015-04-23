#ifndef BDSLine_h
#define BDSLine_h 

#include <vector>
#include "BDSAcceleratorComponent.hh"

/**
 * @brief a class that hold multiple accelerator components
 * 
 * Not physically used in Geant4, only as placeholder.
 * Therefore virtual methods are not specified, but could be in the future.
 *
 * Memory of components is not owned
 */

class BDSLine :public BDSAcceleratorComponent
{
public:

  /// define iterator for ease of reading
  // chosen not to distinguish between non-const and const cases
  typedef std::vector<BDSAcceleratorComponent*>::iterator BDSLineIterator;

  BDSLine(G4String aName,std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta);
  ~BDSLine(){};

  void addComponent(BDSAcceleratorComponent* component);
  BDSLineIterator begin(){return line.begin();}
  BDSLineIterator end(){return line.end();}

private:
  /// define pure virtual method
  virtual void BuildMarkerLogicalVolume(){};

  /// vector
  std::vector<BDSAcceleratorComponent*> line;
};

#endif
