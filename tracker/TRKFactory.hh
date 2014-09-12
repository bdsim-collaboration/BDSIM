#ifndef TRKFactory_h
#define TRKFactory_h

#include "TRKTrackingElement.hh"

class ElementList;
struct Element;
struct Options;

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
  TRKTrackingElement* createSextupole(Element& element);
  TRKTrackingElement* createOctopole(Element& element);
  TRKTrackingElement* createDecapole(Element& element);

  /// tracking type
  TRKTrackingElement::TRKType type;
  /// particle properties
  double charge;
  double momentum;
  double brho;
  /// default aperture
  TRKAperture* aper;
  /// global placement position
  TRKPlacement* placement;
  /// circular flag
  bool circular;
  
};

#endif
