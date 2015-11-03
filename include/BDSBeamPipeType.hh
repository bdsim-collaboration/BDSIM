#ifndef BDSBEAMPIPETYPE_H
#define BDSBEAMPIPETYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for beampipes - used for comparison
 * in factory methods
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

struct beampipetypes_def {
  enum type { circular, rectangular, elliptical, lhc, lhcdetailed, rectellipse, racetrack};
};

typedef BDSTypeSafeEnum<beampipetypes_def,int> BDSBeamPipeType;

namespace BDS {
  /// function that gives corresponding enum value for string (case-insensitive)
  BDSBeamPipeType DetermineBeamPipeType(G4String apertureType);

  /// function to check relevant aperture values are set.  This is really a dispatch function
  /// for other aperture specific methods below
  void CheckApertureInfo(BDSBeamPipeType beamPipeTypeIn,
			 G4double& aper1,
			 G4double& aper2,
			 G4double& aper3,
			 G4double& aper4);

  /// aperture info check for circular aperture
  void InfoOKForCircular(G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4);

  /// aperture info check for elliptical aperture
  void InfoOKForElliptical(G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4);

  /// aperture info check for rectangular aperture
  void InfoOKForRectangular(G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4);

  /// aperture info check for lhc aperture
  void InfoOKForLHC(G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4);

  /// aperture info check for lhc detailed aperture
  void InfoOKForLHCDetailed(G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4);

  /// aperture info check for rectellipse aperture
  void InfoOKForRectEllipse(G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4);

  /// aperture info check for racetrack aperture
  void InfoOKForRaceTrack(G4double& aper1, G4double& aper2, G4double& aper3, G4double& aper4);
}

#endif
