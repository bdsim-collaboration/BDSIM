#ifndef BDSQUADMAGFIELD
#define BDSQUADMAGFIELD

#include "G4Types.hh"
#include "G4MagneticField.hh"

#include "BDSAuxiliaryNavigator.hh"

class BDSQuadMagField: public G4MagneticField, BDSAuxiliaryNavigator
{
public:
  BDSQuadMagField(G4double aBGrad);
  virtual ~BDSQuadMagField();

  virtual void  GetFieldValue(const G4double Point[4],
			      G4double *Bfield ) const;
  
  void SetBGrad(G4double aBGrad);
  
private:
  G4double itsBGrad;
};

inline void BDSQuadMagField::SetBGrad(G4double aBGrad)
{ itsBGrad = aBGrad; }

#endif /* BDSQUADMAGFIELD */
