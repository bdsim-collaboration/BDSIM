#ifndef BDSBunchCircle_h
#define BDSBunchCircle_h 

#include "BDSBunchInterface.hh"
#include "BDSGlobalConstants.hh"
#include "Randomize.hh"

namespace CLHEP {
  class RandFlat;
}

class BDSBunchCircle : public BDSBunchInterface { 
protected : 
  G4double envelopeR; 
  G4double envelopeRp; 
  G4double envelopeT;
  G4double envelopeE; 
  
  CLHEP::RandFlat  *FlatGen;    

public: 
  BDSBunchCircle(); 
  BDSBunchCircle(G4double envelopeR , G4double envelopeRp,
		 G4double envelopeT , G4double envelopeE);
  ~BDSBunchCircle(); 
  void SetOptions(struct Options& opt);
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);  
  
  G4double GetEnvelopeR() {return envelopeR;}
  G4double GetEnvelopeRp(){return envelopeRp;}
  G4double GetEnvelopeT() {return envelopeT;}
  G4double GetEnvelopeE() {return envelopeE;}

  void SetEnvelopeR(G4double envelopeRIn)  {envelopeR = envelopeRIn;}
  void SetEnvelopeRp(G4double envelopeRpIn) {envelopeRp= envelopeRpIn;}
  void SetEnvelopeT(G4double envelopeTIn)  {envelopeT = envelopeTIn;}
  void SetEnvelopeE(G4double envelopeEIn)  {envelopeE = envelopeEIn;}



};

#endif
