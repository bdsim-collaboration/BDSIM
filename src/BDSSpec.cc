#include "BDSSpec.hh"
#include "BDSDebug.hh"
#include <typeinfo>

BDSSpec::~BDSSpec()
{
}


BDSSpec::BDSSpec()
{
}

void BDSSpec::printAllowed() const{
  G4cout << __METHOD_NAME__ << G4endl;
  for(std::list<G4String>::const_iterator it = _allowedSpecs.begin();
      it != _allowedSpecs.end();
      it++){
    G4cout << (*it) << G4endl;
  }
  G4cout << __METHOD_END__ << G4endl;
}

G4bool BDSSpec::checkAllowed(G4String val) const{
  G4cout << __METHOD_NAME__ << G4endl;
  for(std::list<G4String>::const_iterator it = _allowedSpecs.begin();
      it != _allowedSpecs.end();
      it++){
    G4cout << __METHOD_NAME__ << "Looping..." << G4endl;
    G4cout << __METHOD_NAME__ << (*it) << " vs. " << val << G4endl;
    if((*it) == val){
      G4cout << __METHOD_NAME__ << " - value is allowed." << G4endl;
      return true;
    }
  }
  G4cout << __METHOD_NAME__ << " - value \"" << val << "\" is not allowed." << G4endl;
  G4String exceptionString = (G4String)__METHOD_NAME__ + (G4String)" - invalid specification : " + val;
  G4Exception(exceptionString.c_str(), "-1", FatalException, "");
  return false;
}

void BDSSpec::addAllowedSpec(G4String val){
  _allowedSpecs.push_back(val);
  G4cout << __METHOD_NAME__ << " - added to allowedSpecs: " << val << G4endl;
}

void BDSSpec::spec(G4String val){
  if (checkAllowed(val)) _spec = val;
}

G4String BDSSpec::spec(){
  return _spec;
}

G4bool BDSSpec::compare(BDSSpec* val) const{
  G4cout << __METHOD_NAME__ << G4endl;
  if(compare(val->spec())){
    G4cout << __METHOD_NAME__ << " - specs " << val->spec() << " and " << _spec << " are matching." << G4endl;
    return true;
  } else {
    G4cout << __METHOD_NAME__ << " - specs " << val->spec() << " and " << _spec << " are not matching." << G4endl;
    return false;
  }
}


G4bool BDSSpec::compare(G4String val) const{
  //  BDSGeometryFormat* testf = new BDSGeometryFormat(val);
  //  G4bool result = compare(testf);
  if(checkAllowed(val)){
    return (_spec == val);
  }
  return false;
}

G4bool BDSSpec::compare(const char* val) const{
  return compare((G4String) val);
}


