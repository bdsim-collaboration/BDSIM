#ifndef __BDSGEOMETRYFORMAT_HH
#define __BDSGEOMETRYFORMAT_HH

#include "BDSSpec.hh"
#include <list>

class BDSGeometryFormat: public BDSSpec{
public:
  ~BDSGeometryFormat();
  BDSGeometryFormat(G4String format);

private:
  virtual std::list<G4String> allowedSpecs();
  
};
#endif


