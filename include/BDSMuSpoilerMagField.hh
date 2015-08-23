#ifndef BDSMUSPOILERMAGFIELD
#define BDSMUSPOILERMAGFIELD

#include "G4Types.hh"
#include "G4MagneticField.hh"

#include "BDSAuxiliaryNavigator.hh"

class BDSMuSpoilerMagField: public G4MagneticField, BDSAuxiliaryNavigator
{
public:
  BDSMuSpoilerMagField();
  BDSMuSpoilerMagField(G4double aField);

  void SetFieldValue(G4double aField);

  virtual ~BDSMuSpoilerMagField();
  // Constructor and destructor. No actions.
  
  virtual void  GetFieldValue( const G4double Point[4],
			       G4double *Bfield ) const;

private:
  G4double itsBField;
};

inline void BDSMuSpoilerMagField::SetFieldValue(G4double aField)
{itsBField=aField;}

#endif /* BDSMUSPOILERERMAGFIELD */
