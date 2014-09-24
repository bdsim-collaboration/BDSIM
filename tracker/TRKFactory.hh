#ifndef TRKFactory_h
#define TRKFactory_h

#include "TRKElement.hh"

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
  TRKFactory(Options& options);

  TRKLine* createLine(ElementList& beamline_list);

private:
  TRKElement* createElement(Element& element);

  TRKElement* createLine(Element& element);
  TRKElement* createDrift(Element& element);
  TRKElement* createDipole(Element& element);
  TRKElement* createQuadrupole(Element& element);
  TRKElement* createSextupole(Element& element);
  TRKElement* createOctupole(Element& element);
  TRKElement* createDecapole(Element& element);

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
