#ifndef BDSCOMPONENTFACTORY_H
#define BDSCOMPONENTFACTORY_H

#include <map>

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "CLHEP/Units/PhysicalConstants.h"

namespace GMAD {
  struct Element;
}
class BDSAcceleratorComponent;
class BDSBeamPipe;
class BDSBeamPipeInfo;
class BDSCavityInfo;
class BDSLine;
class BDSTiltOffset;

struct BDSMagnetOuterInfo;

/**
 * @brief Factory to produce all types of BDSAcceleratorComponents.
 * 
 * Creates from a parser Element the appropriate
 * object (that inherits BDSAcceleratorComponent) and returns it. Will return
 * nullptr if invalid type or nothing to be constructed for that particular type.
 * Basic calculations on field strength and angle as well as basic parameter validity
 * (zero length) are done here.
 */

class BDSComponentFactory
{
public:
  BDSComponentFactory();
  ~BDSComponentFactory();

  /// Create component from parser Element
  /// Pointers to next and previous Element for lookup
  BDSAcceleratorComponent* CreateComponent(GMAD::Element* elementIn,
					   GMAD::Element* prevElementIn,
					   GMAD::Element* nextElementIn);

  // for each of them - special cases need only for ring logic
  /// Public creation method for ring logic
  BDSAcceleratorComponent* CreateTerminator();
  /// Public creation method for ring logic
  BDSAcceleratorComponent* CreateTeleporter();
  /// Create the tilt and offset information object by inspecting the parser element
  BDSTiltOffset*           CreateTiltOffset(GMAD::Element const* element) const;
 
private:
  /// length safety from global constants
  G4double lengthSafety;
  /// charge from global constants
  G4double charge;
  /// rigidity in T*m for beam particles
  G4double brho;
  
  /// element for storing instead of passing around
  GMAD::Element* element = nullptr;
  /// element access to previous element (can be nullptr)
  GMAD::Element* prevElement = nullptr;
  /// element access to previous element (can be nullptr)
  GMAD::Element* nextElement = nullptr;
  
  BDSAcceleratorComponent* CreateDrift(G4ThreeVector inputFaceNormal, G4ThreeVector outputFaceNormal);
  BDSAcceleratorComponent* CreateRF();
  BDSAcceleratorComponent* CreateSBend(G4double angleIn, G4double angleOut);
  BDSAcceleratorComponent* CreateRBend(G4double angleIn, G4double angleOut);
  BDSAcceleratorComponent* CreateHKick();
  BDSAcceleratorComponent* CreateVKick();
  BDSAcceleratorComponent* CreateQuad();  
  BDSAcceleratorComponent* CreateSextupole();
  BDSAcceleratorComponent* CreateOctupole();
  BDSAcceleratorComponent* CreateDecapole();
  BDSAcceleratorComponent* CreateMultipole();
  BDSAcceleratorComponent* CreateElement();
  BDSAcceleratorComponent* CreateSolenoid();
  BDSAcceleratorComponent* CreateRectangularCollimator();
  BDSAcceleratorComponent* CreateEllipticalCollimator();
  BDSAcceleratorComponent* CreateMuSpoiler();
  BDSAcceleratorComponent* CreateDegrader();
  BDSAcceleratorComponent* CreateLaser();
  BDSAcceleratorComponent* CreateScreen();
  BDSAcceleratorComponent* CreateAwakeScreen();
  BDSAcceleratorComponent* CreateTransform3D();

  /// Creates line of components for sbend
  BDSLine* CreateSBendLine(GMAD::Element const* element,
			   G4int nSbends,
			   G4double bField,
			   G4double bPrime);

  /// Test the component length is sufficient for practical construction.
  G4bool HasSufficientMinimumLength(GMAD::Element* element);

  /// Check whether the pole face rotation angles are too big for practical construction.
  void   PoleFaceRotationsNotTooLarge(GMAD::Element* element,
				      G4double maxAngle=0.5*CLHEP::halfpi);
  
  ///@{ Utility function to prepare model info
  BDSMagnetOuterInfo* PrepareMagnetOuterInfo(GMAD::Element const* element) const;
  BDSMagnetOuterInfo* PrepareMagnetOuterInfo(GMAD::Element const* element,
					     const G4double angleIn,
					     const G4double angleOut) const;
  G4double            PrepareOuterDiameter  (GMAD::Element const* element) const;
  BDSBeamPipeInfo*    PrepareBeamPipeInfo   (GMAD::Element const* element,
					     const G4ThreeVector inputFaceNormal = G4ThreeVector(0,0,-1),
					     const G4ThreeVector outputFaceNormal = G4ThreeVector(0,0,1)) const;

  BDSCavityInfo*      PrepareCavityModelInfo(GMAD::Element const* element) const;
  ///@}
  
  /// Utility function to calculate the number of segments an sbend should be split into.
  /// Based on aperture error tolerance - default is 1mm.
  G4int CalculateNSBendSegments(GMAD::Element const* element,
				const G4double aperturePrecision = 1.0) const;

  /// Utility function to check if the combination of outer diameter, angle and length
  /// will result in overlapping entrance and exit faces and therefore whether to abort.
  void CheckBendLengthAngleWidthCombo(G4double chordLength,
				      G4double angle,
				      G4double outerDiameter,
				      G4String name = "not given");

  /// Prepare all RF cavity models in the component factory. Kept here and copies delivered.
  /// This class deletes them upon destruction.
  void PrepareCavityModels();

  G4String PrepareColour(GMAD::Element const* element, const G4String fallback) const;

  /// Map of cavity model info instances by name
  std::map<G4String, BDSCavityInfo*> cavityInfos;
};
#endif
