#include "BDSSpec.hh"
#include "BDSDebug.hh"
#include <typeinfo>

BDSSpec::~BDSSpec()
{;}


BDSSpec::BDSSpec(G4String val, std::list<G4String> allowedSpecs)
{
  _allowedSpecs = allowedSpecs;
  spec(val);

}

G4bool BDSSpec::checkAllowed(G4String val) const{
  for(std::list<G4String>::const_iterator it = _allowedSpecs.begin();
      it != _allowedSpecs.end();
      it++){
    if((*it) == val){
      return true;
    }
  }
  return false;
}

void BDSSpec::spec(G4String val){
  if (checkAllowed(val)) _spec = val;
  else {
    G4String exceptionString = (G4String)__METHOD_NAME__ + (G4String)" - invalid specification : " + val;
    G4Exception(exceptionString.c_str(), "-1", FatalException, "");
  }
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
  if(checkAllowed(val)) return(val == spec());
  else {
    G4String exceptionString = (G4String)__METHOD_NAME__ + (G4String)" - spec string for comparison is not one of the allowed values.";
    G4Exception(exceptionString.data(), "-1", FatalException, "");
  }
  return false;
}



