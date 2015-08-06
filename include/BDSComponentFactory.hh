#ifndef BDSCOMPONENTFACTORY_H
#define BDSCOMPONENTFACTORY_H

#include <list>
#include "globals.hh"
#include "parser/element.h"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSMagnetOuterInfo.hh"

class ElementList;
class BDSTiltOffset;

class BDSComponentFactory{
public:
  BDSComponentFactory();
  ~BDSComponentFactory();

  /// Create component from parser Element
  BDSAcceleratorComponent* CreateComponent(Element& element);

  // for each of them - special cases need only for ring logic
  BDSAcceleratorComponent* CreateTerminator();
  BDSAcceleratorComponent* CreateTeleporter();
  /// Create the tilt and offset information object by inspecting the parser element
  BDSTiltOffset*           CreateTiltOffset(Element& element);
 
private:
  G4bool   verbose;

  G4double lengthSafety;
  
  G4double _charge, _momentum, _brho;
  
  std::list<struct Element>::iterator _elementIter, _previousElementIter, _nextElementIter;
  /// beamline
  std::list<BDSAcceleratorComponent*> itsBeamline;
  /// element for storing instead of passing around
  Element _element;
  
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
  BDSAcceleratorComponent* CreateMultipole();
  BDSAcceleratorComponent* CreateElement();
  BDSAcceleratorComponent* CreateSolenoid();
  BDSAcceleratorComponent* CreateRectangularCollimator();
  BDSAcceleratorComponent* CreateEllipticalCollimator();
  BDSAcceleratorComponent* CreateMuSpoiler();
  BDSAcceleratorComponent* CreateLaser();
  BDSAcceleratorComponent* CreateScreen();
  BDSAcceleratorComponent* CreateAwakeScreen();
  BDSAcceleratorComponent* CreateTransform3D();

  /// Testing function
  G4bool HasSufficientMinimumLength(Element& element);
  
  ///@{ Utility function to prepare beampipe
  G4Material*        PrepareBeamPipeMaterial(Element& element);
  G4Material*        PrepareVacuumMaterial(Element& element);
  BDSMagnetOuterInfo PrepareMagnetOuterInfo(Element& element);
  G4double           PrepareOuterDiameter(Element& element);
  BDSBeamPipeInfo*   PrepareBeamPipeInfo(Element& element);
  ///@}
};
#endif
