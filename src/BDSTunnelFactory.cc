#include "BDSTunnelFactory.hh"
#include "BDSTunnel.hh"
#include "../parser/enums.h"
#include "BDSRectangleTunnel.hh"
#include "BDSRoundTunnel.hh"
#include "BDSDebug.hh"

#define BDSDEBUG 1

BDSTunnelFactory::BDSTunnelFactory(){
}

BDSTunnelFactory::~BDSTunnelFactory(){
}

//Build a tunnel using data from an element.
BDSTunnel* BDSTunnelFactory::makeTunnel(Element& val){
  _element = val;
return  makeTunnel();
}

BDSTunnel* BDSTunnelFactory::makeTunnel(){

#ifdef BDSDEBUG
  G4cout << "BDSTunnelFactory::makeTunnel() element name = " << _element.name << G4endl;  
#endif
  
  BDSTunnel* tunnel = NULL;
  
  switch(_element.tunnelType){
  case _RECT_TUNNEL:
#ifdef BDSDEBUG
    G4cout << "BDSTunnelFactory  - making rectangle tunnel" << G4endl;
#endif
    tunnel = makeRectangleTunnel(); break;
  case _ROUND_TUNNEL:
#ifdef BDSDEBUG
    G4cout << "BDSTunnelFactory  - making round tunnel" << G4endl;
#endif
    tunnel = makeRoundTunnel(); break; 
  default:
#ifdef BDSDEBUG
    G4cout << "BDSTunnelFactory: tunnelType: " << _element.tunnelType << G4endl; 
#endif
    G4Exception("Error: BDSTunnelFactory: tunnelType not found.", "-1", FatalErrorInArgument, "");   
    exit(1);
    break;
  }

#ifdef BDSDEBUG  
  G4cout << __METHOD_END__ << G4endl;
#endif
  return tunnel;
}

BDSTunnel* BDSTunnelFactory::makeRectangleTunnel(){
  BDSTunnel* aTunnel = new BDSRectangleTunnel(_element);
  return aTunnel;
}

BDSTunnel* BDSTunnelFactory::makeRoundTunnel(){
  G4cout << __METHOD_NAME__ << G4endl;
  BDSTunnel* aTunnel = new BDSRoundTunnel(_element);
  return aTunnel;
}
