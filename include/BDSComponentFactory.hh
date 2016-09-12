#ifndef BDSCOMPONENTFACTORY_H
#define BDSCOMPONENTFACTORY_H

#include "BDSFieldInfo.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include <map>
#include <vector>

namespace GMAD {
  struct Element;
}
class BDSAcceleratorComponent;
class BDSBeamPipe;
class BDSBeamPipeInfo;
class BDSCavityInfo;
class BDSIntegratorSet;
class BDSLine;
class BDSMagnetOuterInfo;
class BDSMagnetStrength;
class BDSTiltOffset;

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
  BDSAcceleratorComponent* CreateComponent(GMAD::Element*                     elementIn,
					   const std::vector<GMAD::Element*>& prevElementIn,
					   const std::vector<GMAD::Element*>& nextElementIn);
  
  /// Public creation method for ring logic
  BDSAcceleratorComponent* CreateTerminator();

  /// Public creation method for ring logic
  BDSAcceleratorComponent* CreateTeleporter(const G4ThreeVector teleporterDetla);

  /// Create the tilt and offset information object by inspecting the parser element
  BDSTiltOffset*           CreateTiltOffset(GMAD::Element const* element) const;

protected:
  /// length safety from global constants
  G4double lengthSafety;
  /// charge from global constants
  G4double charge;
  /// rigidity in T*m for beam particles
  G4double brho;
  /// don't split sbends into multiple segments
  G4bool notSplit;
  /// include thin fringe field element(s) in dipoles
  G4bool includeFringe;
  /// length of a thin element
  G4double thinElementLength;
  
  /// Utility function to check if the combination of outer diameter, angle and length
  /// will result in overlapping entrance and exit faces and therefore whether to abort.
  void CheckBendLengthAngleWidthCombo(G4double chordLength,
				      G4double angle,
				      G4double outerDiameter,
				      G4String name = "not given");
  
  ///@{ Utility function to prepare model info
  BDSMagnetOuterInfo* PrepareMagnetOuterInfo(GMAD::Element const* element) const;
  BDSMagnetOuterInfo* PrepareMagnetOuterInfo(GMAD::Element const* element,
					     const G4double angleIn,
					     const G4double angleOut) const;
  G4double            PrepareOuterDiameter  (GMAD::Element const* element) const;
  BDSBeamPipeInfo*    PrepareBeamPipeInfo   (GMAD::Element const* element,
					     const G4ThreeVector inputFaceNormal  = G4ThreeVector(0,0,-1),
					     const G4ThreeVector outputFaceNormal = G4ThreeVector(0,0,1)) const;

  // A legacy one for now...
  BDSBeamPipeInfo*    PrepareBeamPipeInfo(GMAD::Element const* element,
					  G4double angleIn,
					  G4double angleOut) const;

  BDSCavityInfo*      PrepareCavityModelInfo(GMAD::Element const* element) const;
  ///@}

  /// Check whether the pole face rotation angles are too big for practical construction.
  void   PoleFaceRotationsNotTooLarge(GMAD::Element* element,
				      G4double maxAngle=0.5*CLHEP::halfpi);

private: 
  /// element for storing instead of passing around
  GMAD::Element* element = nullptr;
  /// element access to previous element (can be nullptr)
  GMAD::Element* prevElement = nullptr;
  /// element access to previous element (can be nullptr)
  GMAD::Element* nextElement = nullptr;
  
  BDSAcceleratorComponent* CreateDrift(G4ThreeVector inputFaceNormal, G4ThreeVector outputFaceNormal);
  BDSAcceleratorComponent* CreateRF();
  BDSAcceleratorComponent* CreateSBend();
  BDSAcceleratorComponent* CreateRBend(G4double angleIn, G4double angleOut);
  BDSAcceleratorComponent* CreateKicker(G4bool isVertical);
  BDSAcceleratorComponent* CreateQuad();
  BDSAcceleratorComponent* CreateSextupole();
  BDSAcceleratorComponent* CreateOctupole();
  BDSAcceleratorComponent* CreateDecapole();
  BDSAcceleratorComponent* CreateMultipole();
  BDSAcceleratorComponent* CreateThinMultipole(G4double angleIn);
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
  BDSLine* CreateSBendLine(GMAD::Element*     element,
			   G4int              nSbends,
			   BDSMagnetStrength* st);

  /// Test the component length is sufficient for practical construction.
  G4bool HasSufficientMinimumLength(GMAD::Element* element);
  
  /// Utility function to calculate the number of segments an sbend should be split into.
  /// Based on aperture error tolerance - default is 1mm.
  G4int CalculateNSBendSegments(GMAD::Element const* element,
				const G4double aperturePrecision = 1.0) const;

  /// Prepare all RF cavity models in the component factory. Kept here and copies delivered.
  /// This class deletes them upon destruction.
  void PrepareCavityModels();

  G4String PrepareColour(GMAD::Element const* element, const G4String fallback) const;
  
  /// Map of cavity model info instances by name
  std::map<G4String, BDSCavityInfo*> cavityInfos;

  /// Local copy of reference to integrator set to use.
  const BDSIntegratorSet* integratorSet;
};
#endif
