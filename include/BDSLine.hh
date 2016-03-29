#ifndef BDSLINE_H
#define BDSLINE_H 

#include "BDSAcceleratorComponent.hh"

#include <vector>
#include <iterator>

/**
 * @brief a class that hold multiple accelerator components
 * 
 * Not physically used in Geant4, only as placeholder.
 * Therefore virtual methods are not specified, but could be in the future.
 *
 * Memory of components is not owned
 */

class BDSLine: public BDSAcceleratorComponent
{
private:
  /// Private typedef first so it can be used throughout. Typedef for member
  /// vector.
  typedef std::vector<BDSAcceleratorComponent*> BDSLineVector;

  /// Member vector to store components
  BDSLineVector line;
  
public:
  BDSLine(G4String name);
  ~BDSLine(){};

  void AddComponent(BDSAcceleratorComponent* component);

  typedef BDSLineVector::iterator       iterator;
  typedef BDSLineVector::const_iterator const_iterator;
  iterator begin() {return line.begin();}
  iterator end()   {return line.end();}
  G4bool   empty() {return line.empty();}
  size_t   size() const {return line.size();}

  /// Accessor for part - exposes functionality of the vector for iteration by index.
  BDSAcceleratorComponent * const& operator[](G4int index) const {return line.at(index);}
  
  /// Override the BDSAccelerator::Initialise() function to loop over the
  /// line and call that function belonging to each member
  virtual void Initialise();

private:
  /// define pure virtual method
  virtual void BuildContainerLogicalVolume(){};

  /// assignment and copy constructor not implemented nor used
  BDSLine& operator=(const BDSLine&);
  BDSLine(BDSLine&);
};

#endif
