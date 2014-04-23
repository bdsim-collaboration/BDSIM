//Back scatter biasing

#ifndef BACKSCATTERBIAS_HH
#define BACKSCATTERBIAS_HH

#include "G4WrapperProcess.hh"
#include "G4RunManager.hh"

class BDSBackScatterBias  :public G4WrapperProcess
{
public: 
  //  destructor 
  virtual ~BDSBackScatterBias();

public: // with description
  //  constructor requires the process name and type
  BDSBackScatterBias(const G4String& aName =  "X-",
                 G4ProcessType   aType = fNotDefined );

  //  copy constructor copys the name but does not copy the 
  //  physics table (0 pointer is assigned)
  BDSBackScatterBias(const BDSBackScatterBias &right);

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
  BDSBackScatterBias& operator=(const BDSBackScatterBias&);

  G4double eFactor; // enhancement factor to the cross-setion

};

inline G4double BDSBackScatterBias::GetEnhanceFactor () const
  { return eFactor;}
  
inline  void  BDSBackScatterBias::SetEnhanceFactor (G4double dval)
  { eFactor = dval;}

#endif













