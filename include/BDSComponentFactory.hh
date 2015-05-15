#ifndef __BDSCOMPONENTFACTORY_
#define __BDSCOMPONENTFACTORY_

#include <list>
#include "globals.hh"
#include "parser/element.h"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeInfo.hh"

class ElementList; 

class BDSComponentFactory{
public:
  BDSComponentFactory();
  ~BDSComponentFactory();
  
  BDSAcceleratorComponent* createComponent(std::list<struct Element>::iterator elementIter, ElementList& beamline_list);
  BDSAcceleratorComponent* createComponent(Element& aElement, Element& previousElement, Element& nextElement);
  BDSAcceleratorComponent* createComponent();
 
private:
  G4bool   verbose;

  G4double lengthSafety;
  
  G4double _charge, _momentum, _brho;
  
  std::list<struct Element>::iterator _elementIter, _previousElementIter, _nextElementIter;
  /// beamline
  std::list<BDSAcceleratorComponent*> itsBeamline;
  /// parser data
  Element _element, _previousElement, _nextElement;
  /// method to add common properties (not needed at creation!)
  /// like aperture after creation
  void addCommonProperties(BDSAcceleratorComponent* element);
  BDSAcceleratorComponent* createSampler();
  BDSAcceleratorComponent* createCSampler();
  BDSAcceleratorComponent* createDump();
  BDSAcceleratorComponent* createDrift();
  BDSAcceleratorComponent* createRF();
  BDSAcceleratorComponent* createSBend();
  BDSAcceleratorComponent* createRBend();
  BDSAcceleratorComponent* createHKick();
  BDSAcceleratorComponent* createVKick();
  BDSAcceleratorComponent* createQuad();  
  BDSAcceleratorComponent* createSextupole();
  BDSAcceleratorComponent* createOctupole();
  BDSAcceleratorComponent* createMultipole();
  BDSAcceleratorComponent* createElement();
  BDSAcceleratorComponent* createSolenoid();
  BDSAcceleratorComponent* createCollimator();
  BDSAcceleratorComponent* createMuSpoiler();
  BDSAcceleratorComponent* createLaser();
  BDSAcceleratorComponent* createScreen();
  BDSAcceleratorComponent* createAwakeScreen();
  BDSAcceleratorComponent* createTransform3D();
  // for each of them - special cases need only for ring logic
  BDSAcceleratorComponent* createTerminator();
  BDSAcceleratorComponent* createTeleporter();

  /// Utility function to prepare beampipe
  G4Material*     PrepareBeamPipeMaterial(Element& element);
  G4Material*     PrepareVacuumMaterial(Element& element);
  G4double        PrepareBoxSize(Element& element);
  BDSBeamPipeInfo PrepareBeamPipeInfo(Element& element);

};
#endif
