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
#ifndef TRKFactory_H
#define TRKFactory_H

#include "TRKElement.hh"
#include "TRK.hh"

#include "parser/element.h"
#include "parser/fastlist.h"


class BDSParticleDefinition;

namespace trk
{
  class EventOutput;
  class OpticsAccumulator;
}

namespace GMAD
{
  class Beam;
  class Options;
}

#include <string>

struct Element;

class TRKAperture;
class TRKLine;
class TRKPlacement;
class TRKStrategy;
class TRKOpticsSampler;


/**
 * @brief factory to create beamline elements
 */

class TRKFactory
{
 public:
  TRKFactory(const GMAD::Options&   options,
	     BDSParticleDefinition* particle,
	     std::shared_ptr<trk::EventOutput> = nullptr,
	     std::shared_ptr<trk::OpticsAccumulator> = nullptr);
  ~TRKFactory();
  TRKLine*     CreateLine(const GMAD::FastList<GMAD::Element>& beamline_list);
  TRKStrategy* CreateStrategy();

  friend std::ostream& operator<< (std::ostream& out, const TRKFactory &factory);

private:
  TRK::Strategy SetStrategyEnum(std::string sIn);
  TRK::Aperture SetApertureEnum(std::string aIn);
  
  TRKAperture* CreateAperture(GMAD::Element& element);
  TRKElement*  CreateElement(GMAD::Element& element);

  TRKElement* CreateLine(GMAD::Element& element);
  TRKElement* CreateDrift(GMAD::Element& element);
  TRKElement* CreateSBend(GMAD::Element& element);
  TRKElement* CreateRBend(GMAD::Element& element);
  TRKElement* CreateDipole(GMAD::Element& element);
  TRKElement* CreateQuadrupole(GMAD::Element& element);
  TRKElement* CreateSextupole(GMAD::Element& element);
  TRKElement* CreateOctupole(GMAD::Element& element);
  TRKElement* CreateDecapole(GMAD::Element& element);
  TRKElement* CreateKicker(GMAD::Element& element);
  TRKElement* CreateHKicker(GMAD::Element& element);
  TRKElement* CreateVKicker(GMAD::Element& element);
  
  TRKElement* CreateSampler(std::string name, int samplerIndex, double s);
  TRKElement* CreateSampler(GMAD::Element& element, double s);
  TRKElement* CreateOpticsSampler(const GMAD::Element &element, int analysesIndex,
					double s);

  /// set common properties
  void AddCommonProperties(TRKElement* trkel, GMAD::Element& el);

  BDSParticleDefinition* particle;
  /// Cache of main output so samplers can be constructed with this
  /// output instance.
  std::shared_ptr<trk::EventOutput> eventOutput;
  std::shared_ptr<trk::OpticsAccumulator> optics;

  /// global placement position
  TRKPlacement* placement;
  
  int ngenerate;
  int nturns;
  bool circular;

  
  /// tracking strategy
  TRK::Strategy strategy;
  int   trackingsteps;
  
  bool   useaperture;


};

#endif
