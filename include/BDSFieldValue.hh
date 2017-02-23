/// Whatever this typedef defines, it must be called BDSFieldValue!

#include "BDSThreeVector.hh"

#include "G4Types.hh"

#ifdef FIELDDOUBLE
typedef BDSThreeVector<G4double> BDSFieldValue;
#else
typedef BDSThreeVector<G4float>  BDSFieldValue;
#endif
