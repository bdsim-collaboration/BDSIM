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

  CLHEP::RandFlat  *FlatGen;    

public: 
  BDSBunchEShell();  
  BDSBunchEShell(G4double shellX,  G4double shellXp,
		 G4double shellY,  G4double shellYp,
		 G4double X0,      G4double Y0,      G4double Z0,   G4double T0, 
		 G4double Xp0,     G4double Yp0,     G4double Zp0,
		 G4double sigmaT,  G4double sigmaE);
  ~BDSBunchEShell(); 
  void SetOptions(struct Options& opt);
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);

  G4double GetShellX() {return shellX;}
  G4double GetShellXp() {return shellX;}
  G4double GetShellY() {return shellY;}
  G4double GetShellYp() {return shellYp;}

protected:
  void SetShellX(G4double shellXIn)   { shellX  = shellXIn;}
  void SetShellXp(G4double shellXpIn) { shellXp = shellXpIn;}
  void SetShellY(G4double shellYIn)   { shellY  = shellYIn;}
  void SetShellYp(G4double shellYpIn) { shellYp = shellYpIn;}
};

#endif
