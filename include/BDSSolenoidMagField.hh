#ifndef BDSSOLENOIDMAGFIELD
#define BDSSOLENOIDMAGFIELD

#include "G4Types.hh"
#include "G4MagneticField.hh"

#include "BDSAuxiliaryNavigator.hh"

class BDSSolenoidMagField: public G4MagneticField, BDSAuxiliaryNavigator
{
public:
  BDSSolenoidMagField(G4double aBField);
  virtual ~BDSSolenoidMagField();

  /// Get Field Value (independent of position, since solenoid field is uniform)
  virtual void  GetFieldValue(const G4double* /*Point[4]*/,
			      G4double *Bfield) const;

  void SetBField(G4double aBField);
  
private:
  G4double itsBField;
};

inline void BDSSolenoidMagField::SetBField(G4double aBField)
{ itsBField = aBField; }

#endif /* BDSSOLENOIDMAGFIELD */
