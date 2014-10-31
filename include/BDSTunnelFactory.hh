#ifndef __BDSTUNNELFACTORY_
#define __BDSTUNNELFACTORY_

#include "globals.hh"
#include "parser/element.h"
#include "BDSTunnel.hh"
#include "BDSAcceleratorComponent.hh"

class BDSTunnelFactory{
public:
  BDSTunnelFactory();
  ~BDSTunnelFactory();
  
  BDSTunnel* makeTunnel(Element& val1, BDSAcceleratorComponent* val2);
 
private:
  BDSTunnel* makeTunnel();
  BDSTunnel* makeRectangleTunnel();
  BDSTunnel* makeRoundTunnel();
  Element _element;
  BDSAcceleratorComponent* _component;
};
#endif
