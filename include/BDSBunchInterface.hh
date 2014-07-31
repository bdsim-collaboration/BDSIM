#ifndef BDSBunchInterface_h
#define BDSBunchInterface_h 

#include "globals.hh"
#include "parser/options.h"
#include "BDSGlobalConstants.hh"

class BDSBunchInterface { 
protected : 
  // Centre of distributions
  G4double X0;
  G4double Y0;
  G4double Z0;
  G4double T0; 
  G4double Xp0; 
  G4double Yp0;
  G4double Zp0;
  G4double sigmaT; 
  G4double sigmaE;

public : 
  BDSBunchInterface();
  BDSBunchInterface(G4double sigmaT, G4double sigmaE); 
  BDSBunchInterface(G4double X0,     G4double Y0,     G4double Z0, G4double T0,
		    G4double Xp0,    G4double Yp0,    G4double Zp0, 
		    G4double sigmaT, G4double sigmaE);
  virtual void SetOptions(struct Options& opt); 
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
			       G4double& xp, G4double& yp, G4double& zp,
			       G4double& t , G4double&  E, G4double& weight);

  G4double GetX0()     {return X0;}
  G4double GetY0()     {return Y0;}
  G4double GetZ0()     {return Z0;}
  G4double GetT0()     {return T0;}
  G4double GetXp0()    {return Xp0;}
  G4double GetYp0()    {return Yp0;}
  G4double GetZp0()    {return Zp0;}
  G4double GetSigmaE() {return sigmaE;}
  G4double GetSigmaT() {return sigmaT;}

  void     SetX0(G4double X0In)         {X0 = X0In;}
  void     SetY0(G4double Y0In)         {Y0 = Y0In;}
  void     SetZ0(G4double Z0In)         {Z0 = Z0In;}
  void     SetT0(G4double T0In)         {T0 = T0In;}
  void     SetXp0(G4double Xp0In)       {Xp0= Xp0In;}
  void     SetYp0(G4double Yp0In)       {Yp0 = Yp0In;}
  void     SetZp0(G4double Zp0In)       {Zp0 = Zp0In;}
  void     SetSigmaE(G4double sigmaEIn) {sigmaE = sigmaEIn;}
  void     SetSigmaT(G4double sigmaTIn) {sigmaT = sigmaTIn;}

};

#endif
