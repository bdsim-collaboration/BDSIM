#ifndef __BDSCOMPONENTFACTORY_
#define __BDSCOMPONENTFACTORY_

#include "parser/gmad.h"
#include "BDSAcceleratorComponent.hh"
#include <list>

class BDSComponentFactory{
public:
  BDSComponentFactory();
  ~BDSComponentFactory();
  BDSAcceleratorComponent* createComponent(std::list<struct Element>::iterator elementIter, std::list<struct Element>& beamline_list);
  BDSAcceleratorComponent* createComponent(Element aElement, Element previousElement, Element nextElement);
  BDSAcceleratorComponent* createComponent();
  
private:
  G4bool   verbose;
  
  G4double _charge, _momentum, _brho;
  G4double _bpRad, _FeRad; 
  // magnetic field moments (depending on the magnet type)
  G4double _bField;       // dipole (constant) field (G4 units)
  G4double _bPrime;       // quadrupole field gradient dBy/dx (G4 units)
  G4double _bDoublePrime; // sextupole field coefficient d^2 By/dx^2 (G4 units)
  G4double _bTriplePrime; // octupole field coefficient d^3 By/dy^3 (G4 units)
  
  // stuff for rescaling due to synchrotron radiation, IGNORING
  G4double _synch_factor ;
  // drifts
  G4double _driftLength;
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
  BDSAcceleratorComponent* createTransform3D();
};
#endif
