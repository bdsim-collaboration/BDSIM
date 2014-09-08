#ifndef TRKFactory_h
#define TRKFactory_h

#include "TRKTrackingElement.hh"

class ElementList;
class Element;
class Options;

class TRKAperture;
class TRKLine;
class TRKPlacement;

/**
 * @brief factory to create beamline elements
 */

class TRKFactory {
 public:
  TRKFactory(TRKTrackingElement::TRKType type, Options& options);

  TRKLine* createLine(ElementList& beamline_list);

private:
  TRKTrackingElement* createElement(Element& element);

  TRKTrackingElement* createLine(Element& element);
  TRKTrackingElement* createDrift(Element& element);
  TRKTrackingElement* createDipole(Element& element);
  TRKTrackingElement* createQuadrupole(Element& element);

  TRKTrackingElement::TRKType type;
  double charge;
  double momentum;
  double brho;
  TRKAperture* aper;
  TRKPlacement* placement;
  /// circular flag
  bool circular;
  
};

#endif
