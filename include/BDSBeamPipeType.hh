#ifndef BDSBEAMPIPETYPE_H
#define BDSBEAMPIPETYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types
#include "parser/element.h"

/**
 * @brief Type definition for beampipes - used for comparison
 * in factory methods
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

struct beampipetypes_def {
  enum type { circular, elliptical, rectangular };
};

typedef BDSTypeSafeEnum<beampipetypes_def,int> BDSBeamPipeType;

namespace BDS {
  BDSBeamPipeType DetermineBeamPipeType(G4String apertureType, G4bool globalCheck=false);
  BDSBeamPipeType DetermineBeamPipeType(Element& element);

  /// function to check relevant aperture values are set.  This is really a dispatch function
  /// for other aperture specific methods below
  void CheckApertureInfo(BDSBeamPipeType& beamPipeTypeIn, G4double& beamPipeRadius,
			 G4double& beamPipeThickness,
			 G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4);
  /// aperture info check for circular aperture
  void InfoOKForCircular(G4double& beamPipeRadius, G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4);
  /// aperture info check for rectangular aperture
  void InfoOKForRectangular(G4double& beamPipeRadius, G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4);
}
#endif



