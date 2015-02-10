#ifndef BDSBEAMPIPETYPE_H
#define BDSBEAMPIPETYPE_H

#include "BDSTypeSafeEnum.hh"

/**
 * @brief type definition for beampipes - used for comparison
 * in factory methods
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

struct beampipetypes_def {
  enum type { circular, elliptical, rectangular };
};

typedef BDSTypeSafeEnum<beampipetypes_def,int> BDSBeamPipeType;

#endif
