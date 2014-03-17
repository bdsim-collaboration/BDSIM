#ifndef __BDSCOMPONENTFACTORY_
#define __BDSCOMPONENTFACTORY_

#include <list>
#include "globals.hh"
#include "parser/element.h"
#include "parser/gmad.h"
#include "BDSAcceleratorComponent.hh"

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
  
  G4double _charge, _momentum, _brho;
  G4double _bpRad, _FeRad; 
  
  // stuff for rescaling due to synchrotron radiation, IGNORING
  G4double _synch_factor ;
  // drifts
  G4String _driftName ;
  G4double _driftStartAper, _driftEndAper;
  std::list<struct Element>::iterator _elementIter, _previousElementIter, _nextElementIter;
  
  
  std::list<BDSAcceleratorComponent*> itsBeamline;
  Element _element, _previousElement, _nextElement;
  BDSAcceleratorComponent* createSampler();
  BDSAcceleratorComponent* createCSampler();
  BDSAcceleratorComponent* createDump();
  BDSAcceleratorComponent* createDrift();
  BDSAcceleratorComponent* createPCLDrift();
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
  BDSAcceleratorComponent* createECol();
  BDSAcceleratorComponent* createRCol();
  BDSAcceleratorComponent* createMuSpoiler();
  BDSAcceleratorComponent* createLaser();
  BDSAcceleratorComponent* createScreen();
  BDSAcceleratorComponent* createTransform3D();
};
#endif
