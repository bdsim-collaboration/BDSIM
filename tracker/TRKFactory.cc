/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "BDSDebug.hh"
#include "BDSParticleDefinition.hh"
#include "BDSSamplerRegistry.hh"

#include "TRKAperture.hh"
#include "TRKApertureCircular.hh"
#include "TRKApertureEllipsoidal.hh"
#include "TRKApertureRectangular.hh"
#include "TRKDecapole.hh"
#include "TRKDipoleFringe.hh"
#include "TRKElement.hh"
#include "TRKElementLine.hh"
#include "TRKFactory.hh"
#include "TRKKicker.hh"
#include "TRKLine.hh"
#include "TRKOctupole.hh"
#include "EventOutput.hh"
#include "TRKQuadrupole.hh"
#include "TRKRBend.hh"
#include "TRKSBend.hh"
#include "TRKSampler.hh"
#include "TRKOpticsSampler.hh"
#include "TRKSextupole.hh"
#include "OpticsAccumulator.hh"

//tracking strategies / routines
#include "TRK.hh"
#include "TRKParticleDefinition.hh"
#include "TRKDefaultStrategy.hh"
//#include "TRKHybrid.hh"
//#include "TRKStrategy.hh"
//#include "TRKThick.hh"
//#include "TRKThin.hh"
//#include "TRKThinSymplectic.hh"

#include "parser/beam.h"
#include "parser/element.h"
#include "parser/elementtype.h"
#include "parser/options.h"

#include "analysis/SamplerAnalysis.hh"

#include "CLHEP/Units/SystemOfUnits.h"

TRKFactory::TRKFactory(const GMAD::Options&   options,
		       BDSParticleDefinition* particleIn,
		       std::shared_ptr<trk::EventOutput> eventOutputIn,
		       std::shared_ptr<trk::OpticsAccumulator> opticsIn):
  particle(particleIn),
  eventOutput(std::move(eventOutputIn)),
  placement(nullptr),
  ngenerate(options.nGenerate),
  nturns(options.nturns),
  circular(nturns > 1),
  strategy(SetStrategyEnum(options.trackingType)),
  trackingsteps(options.trackingSteps),
  useaperture(options.useAperture),
  optics(opticsIn)
{
}

TRKFactory::~TRKFactory() = default;

std::ostream& operator<< (std::ostream& out, const TRKFactory& factory)
{
  out << "TRKFactory Instance - details:"                      << std::endl
      << "Tracking Strategy (enum): " << factory.strategy         << std::endl
      << "Tracking Steps:           " << factory.trackingsteps    << std::endl
      << "Use Aperture:             " << factory.useaperture;
  return out;
}

TRK::Strategy TRKFactory::SetStrategyEnum(std::string sIn)
{
  if (sIn == "thin") {return TRK::THIN;}
  else if (sIn == "default") {return TRK::DEFAULT;}
  else if (sIn == "thinsymplectic") {return TRK::THINSYMPLECTIC;}
  else if (sIn == "thick") {return TRK::THICK;}
  else if (sIn == "hybrid") {return TRK::HYBRID;}
  else
    {
      std::cout << "Unknown strategy " << sIn << std::endl;
      exit(1);
    }
}

TRKStrategy* TRKFactory::CreateStrategy()
{
  TRKStrategy* result = nullptr;
  switch(strategy)
    {
//    case TRK::THIN:
//      {result = new TRKThin(trackingsteps); break;}
    case TRK::DEFAULT:
      {result = new TRKDefaultStrategy(); break;}
//    case TRK::THINSYMPLECTIC:
//      {result = new TRKThinSymplectic(trackingsteps); break;}
//    case TRK::THICK:
//      {result = new TRKThick(trackingsteps); break;}
//    case TRK::HYBRID:
//      {result = new TRKHybrid(trackingsteps); break;}
    default:
      {break;}

      if (result)
      {result->SetReferenceParticle(particle);}
    }
  return result;
}

TRK::Aperture TRKFactory::SetApertureEnum(std::string aper)
{
  if      (aper == "circular")    {return TRK::CIRCULAR;}
  else if (aper == "rectangular") {return TRK::RECTANGULAR;}
  else if (aper == "ellipsoidal") {return TRK::ELLIPSOIDAL;}
  else
    {
      auto msg = std::string("Unknown aperture type " + aper);
      throw std::runtime_error(msg);
    }
}

TRKAperture* TRKFactory::CreateAperture(GMAD::Element& element)
{
  //this is annoyingly complex because of the poor implementation of aperture
  //in bdsim.  this is made to allow both general form and individual elements
  //to have their own aperture definitions. individual aperture type is not
  //possible. it will just default to the general kind.
  //default case = aperturetype

  if (useaperture)
    {
      return new TRKApertureCircular(element.aper1 * CLHEP::m);
    }
  else
    {
      return NULL;  //no aperture at all - will never be check with this setting so no seg fault
    }
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

TRKLine* TRKFactory::CreateLine(const GMAD::FastList<GMAD::Element>& beamline_list)
{
  TRKLine* line = new TRKLine("beamline",circular);
  auto s = 0.0;
  auto samplerAnalysesIndex = 0;
  for (auto gmadel : beamline_list)
    {
      TRKElement* element = CreateElement(gmadel);
      if (element)
	{
	  if (optics)
	    { // Precede every element with an optics sampler.
            auto osampler =
                CreateOpticsSampler(gmadel, samplerAnalysesIndex++, s);
            line->AddElement(osampler);
	    }

	  line->AddElement(element);
	  s += element->GetLength();
	  // if the element is flagged as a sampler, we create
	  // a sampler element and put it in the beam line
	  if (gmadel.samplerType != "none")
	    {
	      TRKElement* sampler = CreateSampler(gmadel, s);
	      line->AddElement(sampler);
	    }
	}
    }

  if (optics && !beamline_list.empty()) // Put last optics sampler at end.
    {
      auto osampler = CreateOpticsSampler(*std::prev(beamline_list.end()),
					  samplerAnalysesIndex++,
					  s);
      line->AddElement(osampler);
    }

  return line;
}


TRKElement* TRKFactory::CreateElement(GMAD::Element& element)
{
  TRKElement* trkelement = nullptr;
  switch (element.type)
    {
    case GMAD::ElementType::_LINE:
    case GMAD::ElementType::_MARKER:
      trkelement = CreateLine(element);
      break;
    case GMAD::ElementType::_DRIFT:
      trkelement = CreateDrift(element);
      break;
    case GMAD::ElementType::_SBEND:
      trkelement = CreateSBend(element);
      break;
    case GMAD::ElementType::_RBEND:
      trkelement = CreateRBend(element);;
      break;
    case GMAD::ElementType::_QUAD:
      trkelement = CreateQuadrupole(element);
      break;
    case GMAD::ElementType::_SEXTUPOLE:
      trkelement = CreateSextupole(element);
      break;
    case GMAD::ElementType::_OCTUPOLE:
      trkelement = CreateOctupole(element);
      break;
    // case GMAD::ElementType::_DECAPOLE:
    //   trkelement = CreateDecapole(element);
    //    break;
    case GMAD::ElementType::_MULT:
      //TEMPORARY
      trkelement = CreateDrift(element);
      break;
    case GMAD::ElementType::_ELEMENT:
      //TEMPORARY
      trkelement = CreateDrift(element);
      break;
    case GMAD::ElementType::_ECOL:
    case GMAD::ElementType::_JCOL:
    case GMAD::ElementType::_RCOL:
      trkelement = CreateDrift(element);
      break;
    case GMAD::ElementType::_TRANSFORM3D:
      //TEMPORARY
      trkelement = NULL;
      break;
    case GMAD::ElementType::_KICKER:
    case GMAD::ElementType::_TKICKER:
      trkelement = CreateKicker(element);
      break;
    case GMAD::ElementType::_HKICKER:
      trkelement = CreateHKicker(element);
      break;
    case GMAD::ElementType::_VKICKER:
      trkelement = CreateVKicker(element);
      break;
    default:
      throw std::runtime_error(std::string("Unknown element type: ") +
			       GMAD::typestr(element.type));

      break;
  }
  // TBC - implement sampler here based on element.samplerType - str - defualt 'none'

  if (trkelement)
    {AddCommonProperties(trkelement,element);}

  return trkelement;
}

void TRKFactory::AddCommonProperties(TRKElement* trkelement, GMAD::Element& element)
{
  // offset and tilt
  //  if (element.phi!=0.0 || element.theta!=0.0 || element.psi!=0.0) {
  // only tilt for now
  if (element.tilt!=0.0)
    {trkelement->SetTilt(element.tilt,0,0);} // todo check rotation correct!
  if (element.xdir!=0.0 || element.ydir!=0.0)
    {trkelement->SetOffset(element.xdir, element.ydir);}
}

TRKElement* TRKFactory::CreateLine(GMAD::Element& /*element*/)
{
  return nullptr;
}

TRKElement* TRKFactory::CreateDrift(GMAD::Element& element)
{
  TRKAperture* aperture = CreateAperture(element);
  return new TRKDrift(element.name,
		      element.l * CLHEP::m,
		      aperture,
		      placement);
}

TRKElement* TRKFactory::CreateQuadrupole(GMAD::Element& element)
{
  TRKAperture* aperture = CreateAperture(element);
  return new TRKQuadrupole(element.name,
			   element.l * CLHEP::m,
			   element.k1 / CLHEP::m2,  // m^-2
			   aperture,
			   placement);
}

TRKElement* TRKFactory::CreateSextupole(GMAD::Element& element)
{
  TRKAperture* aperture = CreateAperture(element);
  return new TRKSextupole(element.name,
			  element.l * CLHEP::m,
			  element.k2 / CLHEP::m3,
			  aperture,
			  placement);
}

TRKElement* TRKFactory::CreateOctupole(GMAD::Element& element)
{
  TRKAperture* aperture = CreateAperture(element);
  return new TRKOctupole(element.name,
			 element.l * CLHEP::m,
			 element.k3 / CLHEP::m2 / CLHEP::m2,
			 aperture,
			 placement);
}

TRKElement* TRKFactory::CreateDecapole(GMAD::Element& /*element*/)
{
  throw std::runtime_error("Decapole is currently unsupported.");
}

TRKElement* TRKFactory::CreateSBend(GMAD::Element& element)
{
  TRKAperture *aperture = CreateAperture(element);
  return new TRKSBend(element.name,
                      element.l * CLHEP::m,
		      element.angle * CLHEP::rad,
		      element.k1 / CLHEP::m2,
		      aperture,
		      placement);
}

TRKElement* TRKFactory::CreateRBend(GMAD::Element& element)
{
  TRKAperture* aperture = CreateAperture(element);

    double angle = element.angle * CLHEP::rad;
    double length = element.l * CLHEP::m;

    double poleface = angle / 2;
    double k0 = angle / length;

    std::string name = element.name;

    auto fringeIn = new TRKDipoleFringe(name + "_fringeIn", poleface, aperture, nullptr, k0);
    auto fringeOut = new TRKDipoleFringe(name + "_fringeOut", poleface, aperture, nullptr, k0);

    auto body = new TRKSBend(element.name,
                             length,
			     element.angle,
                             element.k1 / CLHEP::m2,
                             aperture,
                             placement
    );

    auto rbendLine = new TRKElementLine(name);
    rbendLine->AddElement(fringeIn);
    rbendLine->AddElement(body);
    rbendLine->AddElement(fringeOut);

    return rbendLine;
}

TRKElement* TRKFactory::CreateSampler(GMAD::Element& element, double s)
{
  std::string name = element.name;
  int samplerIndex = BDSSamplerRegistry::Instance()->RegisterSampler(name, nullptr);
  return CreateSampler(name, samplerIndex, s);
}

TRKElement* TRKFactory::CreateSampler(std::string name, int samplerIndex,
				      double s) {
  auto result = new TRKSampler(name, samplerIndex, s, eventOutput);
  eventOutput->PushBackSampler(name, nturns * ngenerate);
  return result;
}

TRKElement* TRKFactory::CreateOpticsSampler(const GMAD::Element &element,
                                            int analysesIndex, double s) {
  auto name = element.name;
  auto opticsSampler = new TRKOpticsSampler(element.name,
					    analysesIndex,
					    s,
					    optics);
  optics->AppendAnalysis(s);
  return opticsSampler;
}

TRKElement *TRKFactory::CreateKicker(GMAD::Element& element) {
  TRKAperture *aperture = CreateAperture(element);
  return new TRKKicker(element.name,
                       element.l * CLHEP::m,
		       element.hkick,
		       element.vkick, 
		       aperture,
		       nullptr);
}

TRKElement *TRKFactory::CreateHKicker(GMAD::Element& element) {
  TRKAperture *aperture = CreateAperture(element);
  return new TRKKicker(element.name,
		       element.l * CLHEP::m,
		       element.hkick,
		       0,
                       aperture,
		       nullptr);
}

TRKElement *TRKFactory::CreateVKicker(GMAD::Element& element) {
  TRKAperture *aperture = CreateAperture(element);
  return new TRKKicker(element.name,
		       element.l * CLHEP::m,
		       0.,
		       element.vkick,
                       aperture,
		       nullptr);
}
