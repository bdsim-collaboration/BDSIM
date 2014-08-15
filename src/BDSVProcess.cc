#include "BDSVProcess.hh"

BDSVProcess::BDSVProcess(const G4String &aName, G4ProcessType aType):G4VProcess(aName, aType){;}

BDSVProcess::BDSVProcess(const BDSVProcess &right):G4VProcess(right){;}

BDSVProcess::~BDSVProcess(){;}
