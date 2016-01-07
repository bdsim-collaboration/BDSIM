#ifndef BDSFIELDMAGQUADRUPOLE_H
#define BDSFIELDMAGQUADRUPOLE_H

#include "G4Types.hh"
#include "G4MagneticField.hh"

#include "BDSAuxiliaryNavigator.hh"

class BDSFieldMagQuadrupole: public G4MagneticField, public BDSAuxiliaryNavigator
{
public:
  BDSFieldMagQuadrupole(G4double aBGrad);
  virtual ~BDSQuadMagField();

  virtual void  GetFieldValue(const G4double Point[4],
			      G4double *Bfield ) const;
  
  void SetBGrad(G4double aBGrad);
  
private:
  G4double itsBGrad;
};

inline void BDSFieldMagQuadrupole::SetBGrad(G4double aBGrad)
{itsBGrad = aBGrad; }

#endif
