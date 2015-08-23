#ifndef BDSMULTIPOLEOUTERMAGFIELD
#define BDSMULTIPOLEOUTERMAGFIELD

#include "G4Types.hh"
#include "G4MagneticField.hh"

#include "BDSAuxiliaryNavigator.hh"

class BDSMultipoleOuterMagField: public G4MagneticField, public BDSAuxiliaryNavigator
{
public:
  BDSMultipoleOuterMagField(G4int nPole,G4double polField, G4double phiOffset);
  virtual ~BDSMultipoleOuterMagField();
  // Constructor and destructor. No actions.

  virtual void  GetFieldValue( const G4double *Point,
			       G4double *Bfield ) const;

private:
  G4int itsNPoles;
  G4double itsIronField;
  G4double itsPhiOffset;
  G4double itsSectorPhi;
};

#endif /* BDSMULTIPOELOUTERMAGFIELD */
