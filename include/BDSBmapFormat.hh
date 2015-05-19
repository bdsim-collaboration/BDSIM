#ifndef __BDSBMAPFORMAT_HH
#define __BDSBMAPFORMAT_HH

#include "BDSSpec.hh"

class BDSBmapFormat: public BDSSpec{
public:
  ~BDSBmapFormat();
  BDSBmapFormat(G4String format);

private:
  virtual std::list<G4String> allowedSpecs();
};
#endif


