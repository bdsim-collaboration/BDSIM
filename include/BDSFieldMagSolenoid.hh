#ifndef BDSFIELDMAGSOLENOID_H
#define BDSFIELDMAGSOLENOID_H

#include "G4Types.hh"
#include "G4MagneticField.hh"

#include "BDSAuxiliaryNavigator.hh"

class BDSFieldMagSolenoid: public G4MagneticField, public BDSAuxiliaryNavigator
{
public:
  BDSFieldMagSolenoid(G4double aBField);
  virtual ~BDSFieldMagSolenoid();

  /// Get Field Value (independent of position, since solenoid field is uniform)
  virtual void  GetFieldValue(const G4double* /*Point[4]*/,
			      G4double *Bfield) const;

  void SetBField(G4double aBField);
  
private:
  G4double itsBField;
};

inline void BDSFieldMagSolenoid::SetBField(G4double aBField)
{ itsBField = aBField; }

#endif
