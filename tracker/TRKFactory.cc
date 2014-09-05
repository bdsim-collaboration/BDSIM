#include "TRKFactory.hh"

#include "TRKLine.hh"
#include "TRKDrift.hh"
//#include "TRKDipole.hh"
#include "TRKQuadrupole.hh"

#include "parser/element.h"
#include "parser/elementlist.h"
#include "parser/enums.h"
#include "parser/options.h"

#include "CLHEP/Units/SystemOfUnits.h"

TRKFactory::TRKFactory(TRKTrackingElement::TRKType typeIn, Options& options) {

  type = typeIn;
  
  // define charge and momentum from options
  charge = 1;
  momentum = 0;
  // magnetic rigidity
  brho = options.ffact * momentum / 0.299792458 / charge;

  // perhaps also define default aperture
  aper = NULL;
  /// start placement
  placement = new TRKPlacement();
}

TRKLine* TRKFactory::createLine(ElementList& beamline_list) {
  TRKLine* line = new TRKLine("beamline");
  std::list<struct Element>::iterator it = beamline_list.begin();

  for(;it!=beamline_list.end();it++){
    TRKTrackingElement* element = createElement(*it);
    if (element) {
      line->AddElement(element);
      std::cout << "element created: " << *element << std::endl;
    }
  }
  return line;
}


TRKTrackingElement* TRKFactory::createElement(Element& element) {
  switch (element.type) {

  case _LINE:
    return createLine(element);
  case _DRIFT:
    return createDrift(element);
  case _VKICK:
  case _HKICK:
    return createDipole(element);
  case _QUAD:
    return createQuadrupole(element);
  default:
    return NULL;
  }
}

TRKTrackingElement* TRKFactory::createLine(Element& /*element*/) {
  return NULL;
}

TRKTrackingElement* TRKFactory::createDrift(Element& /*element*/) {
  return NULL;
}

TRKTrackingElement* TRKFactory::createDipole(Element& /*element*/) {
  return NULL;
}

TRKTrackingElement* TRKFactory::createQuadrupole(Element& element) {

#ifdef TRKDEBUG
  std::cout << "create Quadrupole" << std::endl;
#endif
  double bPrime = - brho * (element.k1 / CLHEP::m2);

  return new TRKQuadrupole(bPrime,
			   type,
			   TRK::DEFAULT_TRACKING_STEPS,
			   element.name,
			   element.l,
			   0,
			   0,
			   aper,
			   placement);
}
