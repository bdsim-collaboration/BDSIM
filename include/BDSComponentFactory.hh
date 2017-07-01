#ifndef BDSCOMPONENTFACTORY_H
#define BDSCOMPONENTFACTORY_H

#include "BDSFieldType.hh"
#include "BDSMagnetType.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include <map>

namespace GMAD
{
  struct Element;
}
class BDSAcceleratorComponent;
class BDSBeamPipeInfo;
class BDSCavityInfo;
class BDSIntegratorSet;
class BDSMagnet;
class BDSMagnetOuterInfo;
class BDSMagnetStrength;
class BDSTiltOffset;

/**
 * @brief Factory to produce all types of BDSAcceleratorComponents.
 * 
 * Creates from a parser Element the appropriate object (that inherits 
 * BDSAcceleratorComponent) and returns it. Will return nullptr if invalid 
 * type or nothing to be constructed for that particular type. Calculations 
 * on field strength and angle as well as basic parameter validity (minimum 
 * length) are done here.
 *
 * The main interface requires both a previous and next GMAD::Element that the
 * element is constructed with respect to. This is so that drifts can be matched
 * to the faces of bends with pole face rotations etc.  If required in future, a
 * simple interface for a single element could be added that uses this one with 
 * nullptrs.
 * 
 * Functions for common functionality, checks and preparation of recipe classes 
 * are provided publicly (and statically) for use elsewhere.
 */

class BDSComponentFactory
{
public:
  BDSComponentFactory();
  ~BDSComponentFactory();

  /// Create component from parser Element
  /// Pointers to next and previous Element for lookup
  BDSAcceleratorComponent* CreateComponent(GMAD::Element const* elementIn,
					   GMAD::Element const* prevElementIn,
					   GMAD::Element const* nextElementIn);
  
  /// Public creation for object that dynamically stops all particles once the primary
  /// has completed a certain number of turns.
  BDSAcceleratorComponent* CreateTerminator();

  /// Public creation for object that accounts for slight offset between ends of a ring.
  /// The z component of the delta three vector is used for the length of the teleporter.
  BDSAcceleratorComponent* CreateTeleporter(const G4ThreeVector teleporterDelta);

  /// Create the tilt and offset information object by inspecting the parser element
  BDSTiltOffset*           CreateTiltOffset(GMAD::Element const* element) const;

  /// Prepare the recipe for a piece of beam pipe. Static and public so it can be used by
  /// SBendBuilder.
  static BDSBeamPipeInfo* PrepareBeamPipeInfo(GMAD::Element const* element,
					      const G4ThreeVector inputFaceNormal  = G4ThreeVector(0,0,-1),
					      const G4ThreeVector outputFaceNormal = G4ThreeVector(0,0,1));

  /// Interface to other PrepareBeamPipeInfo() for convenience to avoid preparing
  /// face normal vectors repeatedly.
  static BDSBeamPipeInfo* PrepareBeamPipeInfo(GMAD::Element const* element,
					      G4double angleIn,
					      G4double angleOut);

  /// Determine which side the yoke of an asymmetric bend should go on based on the angle
  /// of the bend and the overriding setting in the element.
  static G4bool YokeOnLeft(const GMAD::Element*     element,
			   const BDSMagnetStrength* st);

  /// Prepare the element outer diameter in Geant4 units - if not set, use the global
  /// default.
  static G4double PrepareOuterDiameter  (GMAD::Element const* element);
  
  /// Prepare the recipe for magnet outer geometry for an element. This uses a
  /// strength instance which (we assume) represents the element. Evenly splits angle
  /// between input and output faces.
  static BDSMagnetOuterInfo* PrepareMagnetOuterInfo(const GMAD::Element* element,
						    const BDSMagnetStrength* st);

  /// Prepare the recipe for magnet outer geometry with full control of the angled faces
  /// and which side the yoke is on. The angle in and out are the face angles relative
  /// to a chord for a straight section of outer magnet geometry.
  static BDSMagnetOuterInfo* PrepareMagnetOuterInfo(const GMAD::Element* element,
						    const G4double angleIn,
						    const G4double angleOut,
						    const G4bool   yokeOnLeft = false);

  /// Utility function to check if the combination of outer diameter, angle and length
  /// will result in overlapping entrance and exit faces and therefore whether to abort.
  static void CheckBendLengthAngleWidthCombo(G4double arcLength,
					     G4double angle,
					     G4double outerDiameter,
					     G4String name = "not given");

  /// Check whether the pole face rotation angles are too big for practical construction.
  static void PoleFaceRotationsNotTooLarge(const GMAD::Element* element,
					   G4double       maxAngle = 0.5*CLHEP::halfpi);
  
private:
  /// length safety from global constants
  G4double lengthSafety;
  /// rigidity in T*m for beam particles
  G4double brho;
  /// length of a thin element
  G4double thinElementLength;
  
  /// Utility function to prepare model info
  BDSCavityInfo* PrepareCavityModelInfo(GMAD::Element const* element) const;

  /// Utility function to prepare field strength object for rf cavity.
  BDSMagnetStrength* PrepareCavityStrength(GMAD::Element const* element) const;

  /// element for storing instead of passing around
  GMAD::Element const* element = nullptr;
  /// element access to previous element (can be nullptr)
  GMAD::Element const* prevElement = nullptr;
  /// element access to previous element (can be nullptr)
  GMAD::Element const* nextElement = nullptr;
  
  /// Private enum for kicker types.
  enum class KickerType {horizontal, vertical, general};
  
  BDSAcceleratorComponent* CreateDrift(G4double angleIn, G4double angleOut);
  BDSAcceleratorComponent* CreateRF();
  BDSAcceleratorComponent* CreateSBend();
  BDSAcceleratorComponent* CreateRBend();
  BDSAcceleratorComponent* CreateKicker(KickerType type);
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
  BDSAcceleratorComponent* CreateShield();
  BDSAcceleratorComponent* CreateDegrader();
  BDSAcceleratorComponent* CreateLaser();
  BDSAcceleratorComponent* CreateScreen();
  BDSAcceleratorComponent* CreateTransform3D();
#ifdef USE_AWAKE
  BDSAcceleratorComponent* CreateAwakeScreen();
  BDSAcceleratorComponent* CreateAwakeSpectrometer();
#endif

  /// Helper method for common magnet construction
  BDSMagnet* CreateMagnet(BDSMagnetStrength* st,
			  BDSFieldType fieldType,
			  BDSMagnetType magnetType,
			  G4double angle = 0.0) const;

  /// Test the component length is sufficient for practical construction.
  G4bool HasSufficientMinimumLength(GMAD::Element const* element,
				    const G4bool printWarning = true);
  
  /// Prepare all RF cavity models in the component factory. Kept here and copies delivered.
  /// This class deletes them upon destruction.
  void PrepareCavityModels();

  /// Checks if colour is specified for element, else uses fallback color
  G4String PrepareColour(GMAD::Element const* element, const G4String fallback) const;

  /// Set the field definition on a BDSAcceleratorComponent from the string definition
  /// name in a parser element. In the case of a BDSMagnet, (exclusively) set the vacuum
  /// and outer field in place of the one general field.
  void SetFieldDefinitions(GMAD::Element const* element,
			   BDSAcceleratorComponent* component) const;
  
  /// Prepare magnet strength for multipoles
  BDSMagnetStrength* PrepareMagnetStrengthForMultipoles(GMAD::Element const* element) const;

  /// Map of cavity model info instances by name
  std::map<G4String, BDSCavityInfo*> cavityInfos;

  /// Local copy of reference to integrator set to use.
  const BDSIntegratorSet* integratorSet;

  /// Calculate the field from a given angle through a length of field - uses member
  /// rigidity and charge. Length & angle in g4 m / rad units.
  G4double FieldFromAngle(const G4double angle,
			  const G4double arcLength) const;

  /// Calculate the angle through a length of field - uses member
  /// rigidity and charge. Length & field in g4 m / tesla units
  G4double AngleFromField(const G4double field,
			  const G4double arcLength) const;

  /// Calculate field and angle of a sector bend. Note, this uses the MADX convention
  /// of +ve angle -> deflection in -ve x.
  void CalculateAngleAndFieldSBend(GMAD::Element const* element,
				   G4double&            angle,
				   G4double&            field) const;

  /// Calculate the field and angle of an rbend from information in the element noting the
  /// 'l' in an element is the chord length of an rbend. Variables passed by reference and
  /// are updated as output. Note, this uses the MADX convention of +ve angle -> deflection
  /// in -ve x.
  void CalculateAngleAndFieldRBend(const GMAD::Element* element,
				   G4double& arcLength,
				   G4double& chordLength,
				   G4double& field,
				   G4double& angle) const;

  /// Calculate the angle of a bend whether it's an rbend or an sbend.
  G4double BendAngle(const GMAD::Element* element) const;

  /// Calculate the outgoing angle of the face (in the horizontal plane) for
  /// a given element. Calculates the bend angle and applies e1 / e2 pole face
  /// rotations the correct way depending on sign of angle. The angle is w.r.t.
  /// outgoing curvilinear coordinates, so for an rbend with e2=0, the returned
  /// angle will be half the bend angle. For an sbend, with e2=0, it'll be 0.
  G4double OutgoingFaceAngle(const GMAD::Element* element) const;

  /// Calculate the incoming angel of the face (in the horizontal plane) for
  /// a given element. Calculates the bend angle and applies e1 / e2 pole face
  /// rotations the correct way depending on sign of angle. The angle is w.r.t.
  /// incoming curvilinear coordinates, so for an rbend with e1=0, the returned
  /// angle will be half the bend angle. For an sbend, with e1=0, it'll be 0.
  G4double IncomingFaceAngle(const GMAD::Element* element) const;

  /// Pull out the right value - either 'kick' or 'h/vkick' for the appropriate
  /// type of kicker from the current member element.
  void GetKickValue(G4double& hkick,
		    G4double& vkick,
		    const KickerType type) const;

  /// Registry of modified elements stored by original name and number of times
  /// modified - 0 counting. This is so when we modify elements beyond their definition
  /// they get a unique name as they're literally not the same object as the unmodified
  /// one so they don't share the placement counter for unique naming. This allows
  /// accurate geometry trees in the visualiser.
  std::map<G4String, G4int> modifiedElements;

  /// Variable used to pass around the possibly modified name of an element.
  G4String elementName;
};
#endif
