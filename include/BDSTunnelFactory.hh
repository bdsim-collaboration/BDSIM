#ifndef __BDSTUNNELFACTORY_
#define __BDSTUNNELFACTORY_

#include "globals.hh"
#include "parser/element.h"
#include "BDSTunnel.hh"

class BDSTunnelFactory{
public:
  BDSTunnelFactory();
  ~BDSTunnelFactory();
  
  BDSTunnel* makeTunnel(Element& val);
 
private:
  BDSTunnel* makeTunnel();
  BDSTunnel* makeRectangleTunnel();
  BDSTunnel* makeRoundTunnel();
  Element _element;
};
#endif
