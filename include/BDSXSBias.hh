//Cross section biasing

#ifndef GAMMACONVERSIONTOMUONS_HH
#define GAMMACONVERSIONTOMUONS_HH

#include "G4WrapperProcess.hh"
#include "G4RunManager.hh"

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

  //  Set/Get methods for the enhancement factor
  void SetEnhanceFactor( G4double ) ;
  G4double GetEnhanceFactor ( ) const; 
  //

  ////////////////////////////
  // DoIt    /////////////////
  ///////////////////////////
  virtual G4VParticleChange* PostStepDoIt(
				  const G4Track& track,
				  const G4Step&  stepData
				  );
  
private:
  /// assignment constructor not implemented nor used
  BDSXSBias& operator=(const BDSXSBias&);

  G4double eFactor; // enhancement factor to the cross-setion

};

inline G4double BDSXSBias::GetEnhanceFactor () const
  { return eFactor;}
  
inline  void  BDSXSBias::SetEnhanceFactor (G4double dval)
  { eFactor = dval;}

#endif













