#ifndef TRKFactory_h
#define TRKFactory_h 1

#include "TRKElement.hh"
#include "TRK.hh"

#include <string>

class  ElementList;
struct Element;
struct Options;

class TRKAperture;
class TRKLine;
class TRKPlacement;
class TRKStrategy;

/**
 * @brief factory to create beamline elements
 */

class TRKFactory {
 public:
  TRKFactory(Options& options);

  TRKLine*     createLine(ElementList& beamline_list);
  TRKStrategy* createStrategy();

  friend std::ostream& operator<< (std::ostream& out, const TRKFactory &factory);

private:
  TRK::Strategy setStrategyEnum(std::string sIn);
  TRK::Aperture setApertureEnum(std::string aIn);
  
  TRKAperture* createAperture(Element& element);
  
  TRKElement* createElement(Element& element);

  TRKElement* createLine(Element& element);
  TRKElement* createDrift(Element& element);
  //TRKElement* createSBend(Element& element);
  //TRKElement* createRBend(Element& element);
  TRKElement* createDipole(Element& element);
  TRKElement* createQuadrupole(Element& element);
  TRKElement* createSextupole(Element& element);
  TRKElement* createOctupole(Element& element);
  TRKElement* createDecapole(Element& element);
  TRKElement* createSolenoid(Element& element);
  //TRKElement* createMultipole(Element& element);
  //TRKElement* createGmadElement(Element& element);
  TRKElement* createSampler(Element& element);
  //TRKElement* createTransform3D(Element& element);

  /// particle properties
  double charge;
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
