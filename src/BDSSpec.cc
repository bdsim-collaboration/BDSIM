#include "BDSSpec.hh"
#include "BDSDebug.hh"
#include <typeinfo>

BDSSpec::~BDSSpec()
{;}


BDSSpec::BDSSpec(G4String spec, std::list<G4String> allowedSpecs)
{
  _allowedSpecs = allowedSpecs;
  for(std::list<G4String>::const_iterator it = _allowedSpecs.begin();
      it != _allowedSpecs.end();
      it++){
    if((*it) == spec){
      _spec = spec;
      return;
    }
  }
  //Have not returned in the above loop, therefore spec is not in the list of allowed values...
  G4String exceptionString = (G4String)__METHOD_NAME__ + (G4String)" - invalid specification : " + spec;
  G4Exception(exceptionString.c_str(), "-1", FatalException, "");
}

G4String BDSSpec::spec() const{
  return _spec;
}

G4bool BDSSpec::compare(BDSSpec* val) const{
  //First check that val is of the same derived class (which will have the same list of allowed specs).
  if (typeid(this) != typeid(val)){
    G4String exceptionString = (G4String)__METHOD_NAME__ + (G4String)" - cannot compare values of diffent BDSSpec derived classes.";
    G4Exception(exceptionString.data(), "-1", FatalException, "");
  }
  //Then compare the value of spec to this one.
  return compare(val->spec());
}

G4bool BDSSpec::compare(G4String val) const{
  return(val == spec());
}



