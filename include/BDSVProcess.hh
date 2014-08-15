#ifndef __G4_V_PROCESS_HH_
#define __G4_V_PROCESS_HH_
#include "G4VProcess.hh"
#include "G4ProcessType.hh"

class BDSVProcess : public G4VProcess {
 public:
  BDSVProcess(const G4String& aName="NoName", G4ProcessType aType=fNotDefined);
  BDSVProcess(const BDSVProcess &right);
  ~BDSVProcess();
  virtual G4double GetCrossSecFactor() = 0; //Want to make this polymorphically available.
};

#endif
