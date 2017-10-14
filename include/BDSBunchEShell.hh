#ifndef BDSBUNCHESHELL_H
#define BDSBUNCHESHELL_H 

#include "BDSBunch.hh"

namespace CLHEP
{
  class RandFlat;
}

/**
 * @brief An uncorrelated uniform random distribution within an elliptical shell.
 * 
 * @author Stewart Boogert
 */

class BDSBunchEShell: public BDSBunch
{
protected : 
  G4double shellX;
  G4double shellXp;
  G4double shellY; 
  G4double shellYp;
  G4double shellXWidth;
  G4double shellXpWidth;
  G4double shellYWidth;
  G4double shellYpWidth;

  CLHEP::RandFlat* FlatGen;    

public: 
  BDSBunchEShell();  
  virtual ~BDSBunchEShell(); 
  virtual void SetOptions(const GMAD::Beam& beam,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity);
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);

protected:
  void SetShellX (G4double shellXIn)            {shellX       = shellXIn;      }
  void SetShellXp(G4double shellXpIn)           {shellXp      = shellXpIn;     }
  void SetShellY (G4double shellYIn)            {shellY       = shellYIn;      }
  void SetShellYp(G4double shellYpIn)           {shellYp      = shellYpIn;     }
  void SetShellXWidth (G4double shellXWidthIn)  {shellXWidth  = shellXWidthIn; }
  void SetShellXpWidth(G4double shellXpWidthIn) {shellXpWidth = shellXpWidthIn;}
  void SetShellYWidth (G4double shellYWidthIn)  {shellYWidth  = shellYWidthIn; }
  void SetShellYpWidth(G4double shellYpWidthIn) {shellYpWidth = shellYpWidthIn;}
};

#endif
