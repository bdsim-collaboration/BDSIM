#ifndef BDSCOMPONENTFACTORY_H
#define BDSCOMPONENTFACTORY_H

#include <list>
#include "globals.hh"
#include "parser/element.h"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSMagnetOuterInfo.hh"

class BDSTiltOffset;

/**
 * @brief Factory to produce all types of BDSAcceleratorComponent s.
 * 
 * Loops over the beam line list from the parser and creates the appropriate
 * object (that inherits BDSAcceleratorComponent) and returns it. Will return
 * nullptr if invalid type or nothing to be constructed for that particular type.
 * Basic calculations on field strength and angle as well as basic parameter validity
 * (zero length?) are done here.
 * 
 */

class BDSComponentFactory
{
public:
  BDSComponentFactory();
  ~BDSComponentFactory();

  /// Create component from parser Element
  /// Pointers to next and previous Element for lookup
  BDSAcceleratorComponent* CreateComponent(GMAD::Element* elementIn, GMAD::Element* prevElementIn, GMAD::Element* nextElementIn);

  // for each of them - special cases need only for ring logic
  /// Public creation method for ring logic
  BDSAcceleratorComponent* CreateTerminator();
  /// Public creation method for ring logic
  BDSAcceleratorComponent* CreateTeleporter();
  /// Create the tilt and offset information object by inspecting the parser element
  BDSTiltOffset*           CreateTiltOffset(GMAD::Element* element);
 
private:
  /// length safety from global constants
  G4double lengthSafety;
  /// charge from global constants
  G4double charge;
  /// rigidity in T*m for beam particles
  G4double brho;
  
  /// element for storing instead of passing around
  GMAD::Element* element;
  /// element access to previous element (can be nullptr)
  GMAD::Element* prevElement;
  /// element access to previous element (can be nullptr)
  GMAD::Element* nextElement;
  
  BDSAcceleratorComponent* CreateSampler();
  BDSAcceleratorComponent* CreateCSampler();
  BDSAcceleratorComponent* CreateDump();
  BDSAcceleratorComponent* CreateDrift();
  BDSAcceleratorComponent* CreateRF();
  BDSAcceleratorComponent* CreateSBend();
  BDSAcceleratorComponent* CreateRBend();
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

  /// Testing function
  G4bool HasSufficientMinimumLength(GMAD::Element* element);
  
  ///@{ Utility function to prepare model info
  BDSMagnetOuterInfo* PrepareMagnetOuterInfo(GMAD::Element* element);
  G4double            PrepareOuterDiameter(GMAD::Element* element);
  BDSBeamPipeInfo*    PrepareBeamPipeInfo(GMAD::Element* element);
  ///@}

  /// Utility function to check if the combination of outer diameter, angle and length
  /// will result in overlapping entrance and exit faces and therefore whether to abort.
  void CheckBendLengthAngleWidthCombo(G4double chordLength,
				      G4double angle,
				      G4double outerDiameter,
				      G4String name = "not given");
};
#endif
