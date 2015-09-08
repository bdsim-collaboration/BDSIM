#ifndef TRKFactory_h
#define TRKFactory_h 1

#include "TRKElement.hh"
#include "TRK.hh"

#include "parser/element.h"
#include "parser/fastlist.h"
#include "parser/options.h"

#include <string>

struct Element;

class TRKAperture;
class TRKLine;
class TRKPlacement;
class TRKStrategy;

/**
 * @brief factory to create beamline elements
 */

class TRKFactory {
 public:
  TRKFactory(GMAD::Options& options);

  TRKLine*     createLine(GMAD::FastList<GMAD::Element>& beamline_list);
  TRKStrategy* createStrategy();

  friend std::ostream& operator<< (std::ostream& out, const TRKFactory &factory);

private:
  TRK::Strategy setStrategyEnum(std::string sIn);
  TRK::Aperture setApertureEnum(std::string aIn);
  
  TRKAperture* createAperture(GMAD::Element& element);
  
  TRKElement* createElement(GMAD::Element& element);

  TRKElement* createLine(GMAD::Element& element);
  TRKElement* createDrift(GMAD::Element& element);
  TRKElement* createSBend(GMAD::Element& element);
  TRKElement* createRBend(GMAD::Element& element);
  TRKElement* createDipole(GMAD::Element& element);
  TRKElement* createQuadrupole(GMAD::Element& element);
  TRKElement* createSextupole(GMAD::Element& element);
  TRKElement* createOctupole(GMAD::Element& element);
  TRKElement* createDecapole(GMAD::Element& element);
  TRKElement* createSolenoid(GMAD::Element& element);
  //TRKElement* createMultipole(GMAD::Element& element);
  //TRKElement* createGmadElement(GMAD::Element& element);
  TRKElement* createSampler(GMAD::Element& element);
  //TRKElement* createTransform3D(GMAD::Element& element);

  /// set common properties
  void addCommonProperties(TRKElement* trkel, GMAD::Element& el);

  /// particle properties
  int charge;
  /// beam momentum in GeV
  double momentum;
  double brho;

  /// global placement position
  TRKPlacement* placement;
  
  /// circular flag
  bool circular;
  
  /// tracking strategy
  TRK::Strategy strategy;
  int   trackingsteps;
  
  /// aperture
  TRKAperture* defaultaperture; /// default aperture
  TRK::Aperture aperturetype;   /// enum of aperture type
  double beampiperadius;
  bool   dontuseaperture;
};

#endif
