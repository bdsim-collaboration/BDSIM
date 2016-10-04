#ifndef __BDSBMAPFORMAT_HH
#define __BDSBMAPFORMAT_HH

#include "BDSSpec.hh"

class BDSBmapFormat: public BDSSpec{
public:
  BDSBmapFormat(); //Sets to format "none"
  ~BDSBmapFormat();
  BDSBmapFormat(G4String format);
  //  virtual G4bool compare(G4String val) const; //Compare to string
  
private:
  virtual void allowedSpecs();
};
#endif


