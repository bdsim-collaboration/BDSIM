#ifndef BDSBunchEShell_h
#define BDSBunchEShell_h 

#include "BDSBunchInterface.hh"
#include "Randomize.hh"

namespace CLHEP {
  class RandFlat;
}

class BDSBunchEShell : public BDSBunchInterface { 

protected : 
  G4double shellX;
  G4double shellXp;
  G4double shellY; 
  G4double shellYp;
  G4double shellXWidth;
  G4double shellXpWidth;
  G4double shellYWidth;
  G4double shellYpWidth;

  CLHEP::RandFlat  *FlatGen;    

public: 
  BDSBunchEShell();  
  ~BDSBunchEShell(); 
  void SetOptions(GMAD::Options& opt);
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);

  G4double GetShellX()       {return shellX;      }
  G4double GetShellXp()      {return shellXp;     }
  G4double GetShellY()       {return shellY;      }
  G4double GetShellYp()      {return shellYp;     }
  G4double GetShellXWidth()  {return shellXWidth; }
  G4double GetShellXpWidth() {return shellXpWidth;}
  G4double GetShellYWidth()  {return shellYWidth; }
  G4double GetShellYpWidth() {return shellYpWidth;}

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
