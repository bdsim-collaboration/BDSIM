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

struct BDSBeamPipeInfo;

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

  /// Function to check relevant aperture values are set based on a preassembled BDSBeamPipeInfo instance
  void CheckApertureInfo(BDSBeamPipeInfo*& info);


  /// Function to check whether a parameter is set (using BDSUtilities BDS::IsFinite() ). If the
  /// accompanying G4bool setAper1 (for example) is true, the parameter will be checked for
  /// being set. All parameters required to be checked are checked and appropriate warnings
  /// printed before exiting if any fail that test.
  void CheckRequiredParametersSet(G4double& aper1, G4bool setAper1,
				  G4double& aper2, G4bool setAper2,
				  G4double& aper3, G4bool setAper3,
				  G4double& aper4, G4bool setAper4);
  
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
