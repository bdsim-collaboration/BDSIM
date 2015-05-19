#ifndef __BDSSPEC_HH
#define __BDSSPEC_HH

#include "BDSGlobalConstants.hh"
#include <list>

//Defines a specification class which contains a G4String that can only contain certain values (to be added to allowed specs
//in the initialise method). The class can be compared to other BDSSpec objects of the same subclass or to a G4String.
//Used to specify, for example, geometry format, or field map format.

class BDSSpec{
public:
  ~BDSSpec();
  G4bool compare(BDSSpec* spec) const; //Compare to another spec object 
  
protected:
  BDSSpec(G4String spec, std::list<G4String> allowedSpecs);
  G4String spec() const;

private:
  virtual std::list<G4String> allowedSpecs() = 0;
  G4String _spec;
  std::list<G4String> _allowedSpecs;
  G4bool compare(G4String spec) const; 
};
#endif


