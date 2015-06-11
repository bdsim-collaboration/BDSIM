#ifndef BDSSOLENOIDMAGFIELD
#define BDSSOLENOIDMAGFIELD

#include "G4Types.hh"
#include "BDSUniformMagField.hh"

class BDSSolenoidMagField : public BDSUniformMagField
{
public:  
  BDSSolenoidMagField(G4double aBField);
  virtual ~BDSSolenoidMagField();
  void SetBField(G4double aBField);
private:
  G4double itsBField;
};
#endif /* BDSSOLENOIDMAGFIELD */
