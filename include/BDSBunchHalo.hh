#ifndef BDSBunchHalo_h
#define BDSBunchHalo_h

class BDSBunchHalo : public BDSBunchInterface { 

private : 
  /* Twiss parameters */
  G4double betaX;
  G4double betaY;
  G4double alphaX;
  G4double alphaY;
  G4double emitX;
  G4double emitY; 
  G4double gammaX;
  G4double gammaY;  

  G4double envelopeX; 
  G4double envelopeY;
  G4double envelopeXp; 
  G4double envelopeYp; 

public : 
  BDSBunchHalo();
  BDSBunchHalo(G4double betaX,      G4double betaY, 
	       G4double alphaX,     G4double alphaY,
	       G4double emitX,      G4double emitY,
	       G4double envelopeX , G4double envelopeY,
	       G4double envelopeXp, G4double envelopeYp,	       
	       G4double X0,         G4double Y0,    G4double Z0,   G4double T0, 
	       G4double Xp0,        G4double Yp0,   G4double Zp0,
	       G4double sigmaT,     G4double sigmaE);
  ~BDSBunchHalo();
  void SetOptions(struct Options &opt);
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);  

  G4double GetBetaX()     {return betaX;}
  G4double GetBetaY()     {return betaY;}
  G4double GetAlphaX()    {return alphaX;}
  G4double GetAlphaY()    {return alphaY;} 
  G4double GetEmitX()     {return emitX;}
  G4double GetEmitY()     {return emitY;} 
  G4double GetEnvelopeX() {return envelopeX;}
  G4double GetEnvelopeY() {return envelopeY;}
  G4double GetEnvelopeXp(){return envelopeXp;}
  G4double GetEnvelopeYp(){return envelopeYp;}
  G4double GetEnvelopeT() {return envelopeT;}
  G4double GetEnvelopeE() {return envelopeE;}

protected:
  void     SetBetaX(double newBetaX) {betaX    = newBetaX;}
  void     SetBetaY(double newBetaY) {betaY    = newBetaY;}
  void     SetAlphaX(double newAlphaX) {alphaX = newAlphaX;}
  void     SetAlphaY(double newAlphaY) {alphaY = newAlphaY;}
  void     SetEmitX(double newEmitX)   {emitX  = newEmitX;}
  void     SetEmitY(double newEmitY)   {emitY  = newEmitY;}   
  void     SetEnvelopeX(G4double envelopeXIn)  {envelopeX = envelopeXIn;}
  void     SetEnvelopeY(G4double envelopeYIn)  {envelopeY = envelopeYIn;}
  void     SetEnvelopeXp(G4double envelopeXpIn){envelopeXp= envelopeXpIn;}
  void     SetEnvelopeYp(G4double envelopeYpIn){envelopeYp= envelopeYpIn;}
};

#endif
