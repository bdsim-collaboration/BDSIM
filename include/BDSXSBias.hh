//Cross section biasing

#ifndef GAMMACONVERSIONTOMUONS_HH
#define GAMMACONVERSIONTOMUONS_HH

#include "G4WrapperProcess.hh"

class BDSXSBias  :public G4WrapperProcess
{
public: 
  //  destructor 
  virtual ~BDSXSBias();

public: // with description
  //  constructor requires the process name and type
  BDSXSBias(const G4String& aName =  "X-",
                 G4ProcessType   aType = fNotDefined );

  //  copy constructor copies the name but does not copy the 
  //  physics table (0 pointer is assigned)
  BDSXSBias(const BDSXSBias &right);

  ////////////////////////////
  // DoIt    /////////////////
  ///////////////////////////
  virtual G4VParticleChange* PostStepDoIt(
				  const G4Track& track,
				  const G4Step&  stepData
				  );
 
  inline G4double eFactor() const {return _eFactor;}
  inline void eFactor(G4double val){_eFactor = val;}
 
private:
  /// assignment constructor not implemented nor used
  BDSXSBias& operator=(const BDSXSBias&);
  G4double _eFactor;
  
};


#endif













