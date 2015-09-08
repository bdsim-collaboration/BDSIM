#include <cstdlib>
#include <iostream>

#include "TRKFactory.hh"

//debug
#include "BDSDebug.hh"

//for momentum
#include "BDSGlobalConstants.hh"

//individual beam elements
#include "TRKLine.hh"
//#include "TRKDrift.hh"
#include "TRKDipole.hh"
#include "TRKDecapole.hh"
#include "TRKQuadrupole.hh"
#include "TRKOctupole.hh"
#include "TRKSampler.hh"
#include "TRKSextupole.hh"
#include "TRKSolenoid.hh"
#include "TRKSBend.hh"
#include "TRKRBend.hh"

//tracking strategies / routines
#include "TRKStrategy.hh"
#include "TRK.hh"
#include "TRKParticleDefinition.hh"
#include "TRKThin.hh"
#include "TRKThinSymplectic.hh"
#include "TRKThick.hh"
#include "TRKHybrid.hh"

//parser info
#include "parser/element.h"
#include "parser/elementtype.h"
#include "parser/options.h"

//aperture
#include "TRKAperture.hh"
#include "TRKApertureCircular.hh"
#include "TRKApertureRectangular.hh"
#include "TRKApertureEllipsoidal.hh"

#include "CLHEP/Units/SystemOfUnits.h"

TRKFactory::TRKFactory(GMAD::Options& options)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << "Initialisation" << std::endl;
#endif
  // define charge and momentum from options
  charge = TRKParticleDefinition::Instance()->GetParticleCharge(options.particleName);
  momentum = BDSGlobalConstants::Instance()->GetParticleMomentum()/CLHEP::GeV;
  // magnetic rigidity
  if (charge!=0)
    {brho = options.ffact * momentum / 0.299792458 / charge;}
  else
    {
      // zero charge particles have infinite magnetic rigidity
      brho = 0;
    }
  brho *= CLHEP::tesla*CLHEP::m;
  
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << "Rigidity (Brho) : " << brho/(CLHEP::tesla*CLHEP::m) << " T*m" << std::endl;
  std::cout << "ffact    : " << options.ffact << std::endl;
  std::cout << "momentum : " << momentum << " GeV" << std::endl;
  std::cout << "charge   : " << charge << std::endl;
#endif
  
  /// start placement, could be updated after every new element
  placement = NULL;//new TRKPlacement();
  /// circular flag? //not available from Options, as command line option!
  // could use GC.nturns instead!
  //  bool circular = options.

  //pull out info from options
  strategy        = setStrategyEnum(options.trackingType);
  aperturetype    = setApertureEnum(options.apertureType);
  beampiperadius  = options.beampipeRadius;
  trackingsteps   = options.trackingSteps;
  defaultaperture = new TRKApertureCircular(beampiperadius);
  dontuseaperture = options.dontUseAperture;
  circular        = options.circular;
}

std::ostream& operator<< (std::ostream& out, const TRKFactory& factory)
{
  out << "TRKFactory Instance - details:"                      << std::endl
      << "Tracking Strategy (enum): " << factory.strategy         << std::endl
      << "Aperture Type:            " << factory.aperturetype     << std::endl
      << "Beam Pipe Radius (m):     " << factory.beampiperadius   << std::endl
      << "Tracking Steps:           " << factory.trackingsteps    << std::endl
      << "Default Aperture Type:    " << *factory.defaultaperture << std::endl
      << "Don't Use Aperture:       " << factory.dontuseaperture;
  return out;
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

TRKStrategy* TRKFactory::createStrategy()
{
  switch(strategy)
    {
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

TRK::Aperture TRKFactory::setApertureEnum(std::string aIn)
{
  if (aIn == "circular")         {return TRK::CIRCULAR;}
  else if (aIn == "rectangular") {return TRK::RECTANGULAR;}
  else if (aIn == "ellipsoidal") {return TRK::ELLIPSOIDAL;}
  else {
    std::cout << "Unknown aperture type " << aIn << std::endl;
    exit(1);
  }
}

TRKAperture* TRKFactory::createAperture(GMAD::Element& element)
{
  //this is annoyingly complex because of the poor implementation of aperture
  //in bdsim.  this is made to allow both general form and individual elements
  //to have their own aperture definitions. individual aperture type is not 
  //possible. it will just default to the general kind.
  //default case = aperturetype
  
  if (dontuseaperture)
    {return NULL;} //no aperture at all - will never be check with this setting so no seg fault
  else
    {return new TRKApertureCircular(element.aper1);}
  /*
    // THIS SHOULD BE FIXED GIVEN THE NEW APERTURE MODELS AVAILABLE
  else if ((element.aperX != 0) && (element.aperY !=0)) {
    //must have been specified - now check whether one of the asymmetric aperture types is specified
    if (aperturetype == TRK::RECTANGULAR) {
      return new TRKApertureRectangular(element.aperX,element.aperY);}
    else {
      //in effect the default if two apertures are specified for an element
      return new TRKApertureEllipsoidal(element.aperX,element.aperY);}
  }
  else if (aperturetype == TRK::RECTANGULAR) {
    //no specified x and y aper, but told its rectangular -> square
    return new TRKApertureRectangular(beampiperadius,beampiperadius);}
  else {
    //must be circular then
    return defaultaperture;}
  */
}

TRKLine* TRKFactory::createLine(GMAD::FastList<GMAD::Element>& beamline_list)
{
  TRKLine* line = new TRKLine("beamline",circular);
  
  for(auto it : beamline_list)
    {
      TRKElement* element = createElement(it);
      if (element)
	{
	  line->AddElement(element);
#ifdef TRKDEBUG
	  std::cout << " Element created: " << *element << std::endl;
#endif
	  // update placement
	}
    }
  return line;
}


TRKElement* TRKFactory::createElement(GMAD::Element& element)
{
  TRKElement* trkelement = NULL;
  switch (element.type)
    {
    case GMAD::ElementType::_LINE:
      trkelement = createLine(element);
      break;
    case GMAD::ElementType::_DRIFT:
      trkelement = createDrift(element);
      break;
    case GMAD::ElementType::_SBEND:
      trkelement = createSBend(element);
      break;
    case GMAD::ElementType::_RBEND:
      trkelement = createRBend(element);;
      break;
    case GMAD::ElementType::_QUAD:
      trkelement = createQuadrupole(element);
      break;
    case GMAD::ElementType::_SEXTUPOLE:
      trkelement = createSextupole(element);
      break;
    case GMAD::ElementType::_OCTUPOLE:
      trkelement = createOctupole(element);
      break;
    // case GMAD::ElementType::_DECAPOLE:
    //   trkelement = createDecapole(element);
    //    break;
    case GMAD::ElementType::_SOLENOID:
      trkelement = createSolenoid(element);
      break;
    case GMAD::ElementType::_MULT:
      //TEMPORARY
      trkelement = createDrift(element);
      break;
    case GMAD::ElementType::_ELEMENT:
      //TEMPORARY
      trkelement = createDrift(element);
      break;
    case GMAD::ElementType::_SAMPLER:
      trkelement = createSampler(element);
      break;
    case GMAD::ElementType::_RCOL:
      trkelement = createDrift(element);
      break;
    case GMAD::ElementType::_TRANSFORM3D:
      //TEMPORARY
      trkelement = NULL;
      break;
    case GMAD::ElementType::_VKICK:
    case GMAD::ElementType::_HKICK:
      trkelement = createDipole(element);
      break;
    default:
      trkelement = NULL;
      break;
  }
  
  /* types not specified - see parser/enums.h for types
  _SCREEN
  _CSAMPLER
  */

  if (trkelement)
    {addCommonProperties(trkelement,element);}

  return trkelement;
}

void TRKFactory::addCommonProperties(TRKElement* trkelement, GMAD::Element& element)
{
  // offset and tilt
  //  if (element.phi!=0.0 || element.theta!=0.0 || element.psi!=0.0) {
  // only tilt for now
  if (element.tilt!=0.0)
    {trkelement->SetTilt(element.tilt,0,0);} // todo check rotation correct!
  if (element.xdir!=0.0 || element.ydir!=0.0)
    {trkelement->SetOffset(element.xdir, element.ydir);}
}

TRKElement* TRKFactory::createLine(GMAD::Element& /*element*/)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << std::endl;
#endif
  // method not needed?
  return NULL;
}

TRKElement* TRKFactory::createDrift(GMAD::Element& element)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__;
#endif
  TRKAperture* aperture = createAperture(element);
  return new TRKDrift(element.name,
		      element.l,
		      aperture,
		      placement);
}

TRKElement* TRKFactory::createDipole(GMAD::Element& /*element*/)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__;
#endif
  // bfield , see componentfactory and bdskicker.cc
  // strength (bprime)
  //TRKAperture* aperture = createAperture(element);
  return NULL;
}

TRKElement* TRKFactory::createQuadrupole(GMAD::Element& element)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__;
#endif
  TRKAperture* aperture = createAperture(element);
  return new TRKQuadrupole(element.k1,
			   element.name,
			   element.l,
			   aperture,
			   placement);
}

TRKElement* TRKFactory::createSextupole(GMAD::Element& element)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__;
#endif
  double bPrime = - brho * (element.k2 / CLHEP::m3); // to be checked
  TRKAperture* aperture = createAperture(element);
  return new TRKSextupole(bPrime,
			  element.name,
			  element.l,
			  aperture,
			  placement);
}

TRKElement* TRKFactory::createOctupole(GMAD::Element& element)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__;
#endif
  double bPrime = - brho * (element.k3 / CLHEP::m2 / CLHEP::m2); // to be checked
  TRKAperture* aperture = createAperture(element);
  return new TRKOctupole(bPrime,
			 element.name,
			 element.l,
			 aperture,
			 placement);
}

TRKElement* TRKFactory::createDecapole(GMAD::Element& /*element*/)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__;
#endif
  //TRKAperture* aperture = createAperture(element);
  return NULL;
}

TRKElement* TRKFactory::createSolenoid(GMAD::Element& element)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__;
#endif
  //
  // magnetic field
  //
  // B = B/Brho * Brho = ks * Brho
  // brho is in Geant4 units, but ks is not -> multiply ks by m^-1
  double bField;
  if(element.B != 0){
    bField = element.B * CLHEP::tesla;
    //    element.ks  = (bField/brho) / CLHEP::m;
  }
  else{
    bField = (element.ks/CLHEP::m) * brho;
    //    element.B = bField/CLHEP::tesla;
  }

  TRKAperture* aperture = createAperture(element);
  return new TRKSolenoid(bField,
			 element.name,
			 element.l,
			 aperture,
			 placement);
}

TRKElement* TRKFactory::createSBend(GMAD::Element& element)
{
  #ifdef TRKDEBUG
  std::cout << __METHOD_NAME__;
  #endif
  
  double angle;
  // B Field used to define bend: theta = qBl / p 
  if(element.B != 0)
  {    
    angle = charge * (element.B * CLHEP::tesla) * element.l / momentum;
  }
  else
  {
    angle = element.angle;
  }
  
  TRKAperture* aperture = createAperture(element);
  return new TRKSBend(angle,
      element.name,
      element.l,
      aperture,
      placement);
}

TRKElement* TRKFactory::createRBend(GMAD::Element& element)
{
  #ifdef TRKDEBUG
  std::cout << __METHOD_NAME__;
  #endif
  
  TRKAperture* aperture = createAperture(element);
  return new TRKRBend(element.angle,
      element.name,
      element.l,
      aperture,
      placement);
}

TRKElement* TRKFactory::createSampler(GMAD::Element& element)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__;
#endif
  return new TRKSampler(element.name);
}
