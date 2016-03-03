#ifndef BDSBEAMPIPEINFO_H
#define BDSBEAMPIPEINFO_H

#include "BDSBeamPipeType.hh"
#include "globals.hh"         // geant4 types / globals

class G4Material;

/**
 * @brief Holder class for all information required to describe a beam pipe model. 
 * 
 * This reduces the number of argument
 * to all magnet constructors plus aggregates common tasks
 * in the component factory.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSBeamPipeInfo
{
public:
  /// extra constructor to assign all members at once
  BDSBeamPipeInfo(BDSBeamPipeType beamPipeTypeIn,
		  G4double        aper1In,
		  G4double        aper2In,
		  G4double        aper3In,
		  G4double        aper4In,
		  G4Material*     vacuumMaterialIn,
		  G4double        beamPipeThicknessIn,
		  G4Material*     beamPipeMaterialIn,
		  G4double        angleInIn = 0,
		  G4double        angleOutIn = 0);

  /// Constructor with string descriptors of materials and type. Automatically determined
  /// using BDSBeamPipeType and BDSMaterials
  BDSBeamPipeInfo(G4String beamPipeTypeIn,
		  G4double aper1In,
		  G4double aper2In,
		  G4double aper3In,
		  G4double aper4In,
		  G4String vacuumMaterialIn,
		  G4double beamPipeThicknessIn,
		  G4String beamPipeMaterialIn,
		  G4double angleInIn = 0,
		  G4double angleOutIn = 0);

  /// Constructor that allows a default model to be used as backup. Checks on parameter
  /// validity are done after substituting unset values by values from defaultInfo.
  BDSBeamPipeInfo(BDSBeamPipeInfo* defaultInfo,
		  G4String beamPipeTypeIn,
		  G4double aper1In,
		  G4double aper2In,
		  G4double aper3In,
		  G4double aper4In,
		  G4String vacuumMaterialIn,
		  G4double beamPipeThicknessIn,
		  G4String beamPipeMaterialIn,
		  G4double angleInIn = 0,
		  G4double angleOutIn = 0);

  /// Function to check relevant aperture values are set.  This is really a dispatch function
  /// for other aperture specific methods below
  void CheckApertureInfo();

  ///@{ Public member for direct access
  BDSBeamPipeType beamPipeType;
  G4double        aper1;
  G4double        aper2;
  G4double        aper3;
  G4double        aper4;
  G4Material*     vacuumMaterial;
  G4double        beamPipeThickness;
  G4Material*     beamPipeMaterial;
  G4double        angleIn;
  G4double        angleOut;
  ///@}
  
private:
  /// Private default constructor to ensure one of supplied constructors is used.
  BDSBeamPipeInfo();
  
  /// Function to check whether a parameter is set (using BDSUtilities BDS::IsFinite() ). If the
  /// accompanying G4bool setAper1 (for example) is true, the parameter will be checked for
  /// being set. All parameters required to be checked are checked and appropriate warnings
  /// printed before exiting if any fail that test.
  void CheckRequiredParametersSet(G4bool setAper1, G4bool setAper2,
				  G4bool setAper3, G4bool setAper4);
  
  /// aperture info check for circular aperture
  void InfoOKForCircular();

  /// aperture info check for elliptical aperture
  void InfoOKForElliptical();

  /// aperture info check for rectangular aperture
  void InfoOKForRectangular();

  /// aperture info check for lhc aperture
  void InfoOKForLHC();

  /// aperture info check for lhc detailed aperture
  void InfoOKForLHCDetailed();

  /// aperture info check for rectellipse aperture
  void InfoOKForRectEllipse();

  /// aperture info check for racetrack aperture
  void InfoOKForRaceTrack();

  /// aperture info check for octagon aperture
  void InfoOKForOctagonal();
  
};

#endif
