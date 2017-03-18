#ifndef BDSFIELDMAGINTERPOLATED_H
#define BDSFIELDMAGINTERPOLATED_H

#include "BDSFieldMag.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

class BDSFieldMagInterpolated: public BDSFieldMag
{
public:
  BDSFieldMagInterpolated(G4Transform3D offset);
  BDSFieldMagInterpolated(G4Transform3D offset,
			  G4double      scalingIn);

  virtual ~BDSFieldMagInterpolated(){;}

  inline G4double Scaling() const {return scaling;}
  inline void     SetScaling(G4double scalingIn) {scaling = scalingIn;}
  
private:
  G4double scaling; ///< Field value scaling.
};

#endif
