#ifndef __BDSGEOMETRYFORMAT_HH
#define __BDSGEOMETRYFORMAT_HH

#include "BDSSpec.hh"
#include <list>

class BDSGeometryFormat: public BDSSpec{
public:
  BDSGeometryFormat();
  ~BDSGeometryFormat();
  BDSGeometryFormat(G4String format);
  //  virtual G4bool compare(G4String val) const; //Compare to string
  
private:
  virtual void allowedSpecs();
  
};
#endif


