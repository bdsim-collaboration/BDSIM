#ifndef BDSBEAMPIPETYPE_H
#define BDSBEAMPIPETYPE_H

#include "BDSTypeSafeEnum.hh"

struct beampipetypes_def {
  enum type { circular, elliptical, rectangular };
};

typedef BDSTypeSafeEnum<beampipetypes_def,int> BDSBeamPipeType;

#endif
