#ifndef BDSFIELDDATA_H
#define BDSFIELDDATA_H

#include "BDSArray2D.hh"
#include "BDSArray2DCoords.hh"
#include "BDSThreeVector.hh"

#include "globals.hh"

// F for field
// each set of typedefs must have the same name to work
#ifdef FIELDDOUBLE
typedef BDSThreeVector<G4double>           BDSThreeVectorF;
typedef BDSArray2D<BDSThreeVectorF >       BDSArray2D3VF;
typedef BDSArray2DCoords<BDSThreeVectorF > BDSArray2DCoords3VF;
#else
typedef BDSThreeVector<G4float>                     BDSThreeVectorF;
typedef BDSArray2D<BDSThreeVector<G4float> >        BDSArray2D3VF;
typedef BDSArray2DCoords<BDSThreeVector<G4float> >  BDSArray2DCoords3VF;
#endif

#endif
