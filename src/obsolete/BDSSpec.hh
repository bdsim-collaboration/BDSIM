#ifndef __BDSSPEC_HH
#define __BDSSPEC_HH

#include "BDSGlobalConstants.hh"
#include <list>

//Defines a specification class which contains a G4String that can only contain certain values (to be added to allowed specs
//in the initialise method). The class can be compared to other BDSSpec objects of the same subclass or to a G4String.
//Used to specify, for example, geometry format, or field map format.

class BDSSpec{
public:
  BDSSpec();
  ~BDSSpec();
  G4bool compare(BDSSpec* spec) const; //Compare to another spec object 
  G4bool compare(G4String val) const; //Compare to string
  G4bool compare(const char* val) const; //Compare to string
  //  G4bool compare(G4String spec) const; 
  void spec(G4String val);
  G4String spec();
  void printAllowed() const;
  
protected:
  BDSSpec(G4String spec, std::list<G4String> allowedSpecs);
  void addAllowedSpec(G4String val);

private:
  virtual void allowedSpecs() = 0;
  G4String _spec;
  std::list<G4String> _allowedSpecs;
  G4bool checkAllowed(G4String val) const;
};
#endif


