#include <cstdlib>
#include <iostream>

#include "TRKFactory.hh"

//individual beam elements
#include "TRKLine.hh"
#include "TRKDipole.hh"
#include "TRKDecapole.hh"
#include "TRKQuadrupole.hh"
#include "TRKOctupole.hh"
#include "TRKSextupole.hh"

//tracking strategies / routines
#include "TRKStrategy.hh"
#include "TRK.hh"
#include "TRKThin.hh"
#include "TRKThinSymplectic.hh"
#include "TRKThick.hh"
#include "TRKHybrid.hh"

//parser info
#include "parser/element.h"
#include "parser/elementlist.h"
#include "parser/enums.h"
#include "parser/options.h"

#include "CLHEP/Units/SystemOfUnits.h"

TRKFactory::TRKFactory(Options& options) {

  // define charge and momentum from options
  charge = 1;
  momentum = 0;
  // magnetic rigidity
  brho = options.ffact * momentum / 0.299792458 / charge;

  // perhaps also define default aperture
  //can get from options
  aper = NULL;
  /// start placement, could be updated after every new element
  placement = new TRKPlacement();
  /// circular flag? //not available from Options, as command line option!
  // could use GC.nturns instead!
  //  bool circular = options.

  //pull out strategy info
  strategy      = setStrategyEnum(options.trackingType);
  trackingsteps = options.trackingSteps;
}

TRK::Strategy TRKFactory::setStrategyEnum(std::string sIn)
{
  if (sIn == "thin") {return TRK::THIN;}
  else if (sIn == "thinsymplectic") {return TRK::THINSYMPLECTIC;}
  else if (sIn == "thick") {return TRK::THICK;}
  else if (sIn == "hybrid") {return TRK::HYBRID;}
  else {
    std::cout << "Unknown strategy " << sIn << std::endl;
    exit(1);
  }
}

TRKStrategy* TRKFactory::createStrategy() {
  switch(strategy) {

  case TRK::THIN:
    return new TRKThin(trackingsteps);
  case TRK::THINSYMPLECTIC:
    return new TRKThinSymplectic(trackingsteps);
  case TRK::THICK:
    return new TRKThick(trackingsteps);
  case TRK::HYBRID:
    return new TRKHybrid(trackingsteps);
  default:
    return NULL;
  }
}

TRKLine* TRKFactory::createLine(ElementList& beamline_list) {
  bool circular = false;
  TRKLine* line = new TRKLine("beamline",circular);
  std::list<struct Element>::iterator it = beamline_list.begin();

  for(;it!=beamline_list.end();it++){
    TRKElement* element = createElement(*it);
    if (element) {
      line->AddElement(element);
      std::cout << "element created: " << *element << std::endl;
      // update placement
    }
  }
  /// teleporter / terminator for rings?
  return line;
}


TRKElement* TRKFactory::createElement(Element& element) {
  switch (element.type) {

  case _LINE:
    return createLine(element);
  case _DRIFT:
    return createDrift(element);
  case _VKICK:
    //temporary
    return createDrift(element);
  case _HKICK:
    return createDipole(element);
  case _QUAD:
    return createQuadrupole(element);
  case _SEXTUPOLE:
    return createSextupole(element);
  case _OCTUPOLE:
    return createOctupole(element);
  // case _DECAPOLE:
  //   return createDecapole(element);
  default:
    return NULL;
  }
}

TRKElement* TRKFactory::createLine(Element& /*element*/) {
  // method not needed?
  return NULL;
}

TRKElement* TRKFactory::createDrift(Element& element) {
#ifdef TRKDEBUG
  std::cout << "create Drift" << std::endl;
#endif
  return new TRKDrift(element.name,
		      element.l,
		      aper,
		      placement);
}

TRKElement* TRKFactory::createDipole(Element& /*element*/) {
  // bfield , see componentfactory and bdskicker.cc
  // strength (bprime)
  return NULL;
}

TRKElement* TRKFactory::createQuadrupole(Element& element) {

#ifdef TRKDEBUG
  std::cout << "create Quadrupole" << std::endl;
#endif
  double bPrime = - brho * (element.k1 / CLHEP::m2);

  return new TRKQuadrupole(bPrime,
			   element.name,
			   element.l,
			   aper,
			   placement);
}

TRKElement* TRKFactory::createSextupole(Element& element) {
#ifdef TRKDEBUG
  std::cout << "create Sextupole" << std::endl;
#endif
  double bPrime = - brho * (element.k2 / CLHEP::m3); // to be checked
  return new TRKSextupole(bPrime,
			  element.name,
			  element.l,
			  aper,
			  placement);
}

TRKElement* TRKFactory::createOctupole(Element& element) {
#ifdef TRKDEBUG
  std::cout << "create Quadrupole" << std::endl;
#endif
  double bPrime = - brho * (element.k3 / CLHEP::m2 / CLHEP::m2); // to be checked
  return new TRKOctupole(bPrime,
			 element.name,
			 element.l,
			 aper,
			 placement);
}

TRKElement* TRKFactory::createDecapole(Element& /*element*/) {
  return NULL;
}
