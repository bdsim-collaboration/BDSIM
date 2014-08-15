#include "BDSVProcess.hh"

BDSVProcess::BDSVProcess(const G4String &aName="NoName", G4ProcessType aType=fNotDefined):G4VProcess(aName, aType){;}

BDSVProcess::BDSVProcess(const BDSVProcess &right):G4VProcess(right){;}

BDSVProcess::~BDSVProcess(){;}
